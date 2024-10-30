
// (c) 2024 CrownSoft
// https://www.crownsoft.net

#pragma once

#include <windows.h>

class LlamaCLIResponseHandler
{
public:
    // if finish is true the text can be empty.
    virtual void OnLlamaData(const char* text, bool emptyText, bool finish) = 0;
};

class LlamaCLIHost
{
    LlamaCLIResponseHandler* handler;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    HANDLE hChildStdoutRd, hChildStdoutWr;
    HANDLE hChildStdinRd, hChildStdinWr;

    void ReadUntilPrompt(bool ignoreData)
    {
        // Reading output from the child process's stdout
        CHAR buffer[4096];
        buffer[0] = 0;
        DWORD read = 0;
        BOOL success = FALSE;

        for (;;) {
            DWORD bytesAvailable = 0;

            // Peek into the pipe to check if data is available
            if (!PeekNamedPipe(hChildStdoutRd, NULL, 0, NULL, &bytesAvailable, NULL)) {
                break; // error: PeekNamedPipe failed
            }

            // continue the loop if no data is available
            if (bytesAvailable == 0) {
                Sleep(10);
                continue;
            }

            // Now read from the pipe if data is available
            success = ReadFile(hChildStdoutRd, buffer, sizeof(buffer) - 1, &read, NULL);
            if (!success || read == 0)
                break;

            // Null-terminate and print the output from the child process
            buffer[read] = '\0';

            bool shouldQuit = false;
            if (read >= 4)
            {
                if ((buffer[read - 4] == '\r') && (buffer[read - 3] == '\n') && (buffer[read - 2] == '>') && (buffer[read - 1] == ' '))
                {
                    buffer[read - 4] = 0;
                    shouldQuit = true;
                }
            }

            if (!ignoreData)
                handler->OnLlamaData(buffer, ((read == 4) && shouldQuit), shouldQuit);

            if (shouldQuit)
                break;
        }
    }

public:
    LlamaCLIHost(LlamaCLIResponseHandler* handler)
    {
        this->handler = handler;
    }

    static wchar_t* RemoveMarkdown(const wchar_t* input) {
        if (input == NULL) return NULL;

        size_t len = wcslen(input);
        wchar_t* output = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
        if (output == NULL) return NULL;

        int in_codeblock = 0;
        int in_inline_code = 0;
        size_t i = 0, j = 0;
        int at_line_start = 1;  // Flag to check if we're at the start of a line

        while (input[i] != L'\0') {
            if (in_codeblock) {
                if (wcsncmp(&input[i], L"```", 3) == 0) {
                    in_codeblock = 0;
                    i += 3;
                    // Preserve newline after closing code block
                    if (input[i] == L'\n') output[j++] = input[i++];
                    continue;
                }
                output[j++] = input[i++];
            }
            else if (in_inline_code) {
                if (input[i] == L'`') {
                    in_inline_code = 0;
                    i++;
                }
                else {
                    output[j++] = input[i++];
                }
            }
            else {
                if (wcsncmp(&input[i], L"```", 3) == 0) {
                    in_codeblock = 1;
                    i += 3;
                    // Skip language name if present
                    while (input[i] != L'\0' && input[i] != L'\n') {
                        i++;
                    }
                    // Always preserve the newline, whether it's after a language name or just after ```
                    if (input[i] == L'\n') {
                        output[j++] = input[i++];
                    }
                }
                else if (input[i] == L'`') {
                    in_inline_code = 1;
                    i++;
                }
                else if (input[i] == L'*' || input[i] == L'_') {
                    // Skip bold and italic markers
                    i++;
                }
                else if (input[i] == L'#' && at_line_start) {
                    // Only skip heading markers at the start of a line
                    while (input[i] == L'#') i++;
                    // Preserve the space after the heading markers
                    if (input[i] == L' ') i++;
                    at_line_start = 0;  // No longer at the start of a line
                }
                else if (wcsncmp(&input[i], L"[", 1) == 0) {
                    // Handle link text
                    i++;
                    while (input[i] != L']' && input[i] != L'\0') {
                        output[j++] = input[i++];
                    }
                    if (input[i] == L']') i++;
                    // Skip the URL part
                    if (input[i] == L'(') {
                        while (input[i] != L')' && input[i] != L'\0') i++;
                        if (input[i] == L')') i++;
                    }
                }
                else {
                    output[j++] = input[i++];
                    at_line_start = (input[i - 1] == L'\n');  // Update line start flag
                }
            }
        }

        output[j] = L'\0';
        return output;
    }

    // commandline must begin with space
    // commandline: -m qwen2.5-math-1.5b-instruct-q5_k_m.gguf -cnv -p "You are Qwen, created by Alibaba Cloud.You are a helpful assistant." -n 1024 --simple-io -mli
    void Start(const wchar_t* executablePath, const wchar_t* commandline, bool showBootInfo)
    {
        ZeroMemory(&si, sizeof(STARTUPINFO));
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        si.cb = sizeof(STARTUPINFO);
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;  // Handle inheritance
        sa.lpSecurityDescriptor = NULL;

        ::CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &sa, 0);
        ::SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);
        ::CreatePipe(&hChildStdinRd, &hChildStdinWr, &sa, 0);
        ::SetHandleInformation(hChildStdinWr, HANDLE_FLAG_INHERIT, 0);

        // Set up the STARTUPINFO structure for the child process.
        // Redirect the standard input/output to the pipes created above.
        si.hStdOutput = hChildStdoutWr;
        si.hStdError = hChildStdoutWr;
        si.hStdInput = hChildStdinRd;
        si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        wchar_t* lpCommandLine = ::_wcsdup(commandline);

        ::CreateProcessW(executablePath,
            lpCommandLine,  // Command to run
            NULL,            // Process security attributes
            NULL,            // Primary thread security attributes
            TRUE,            // Handles are inherited
            0,               // Creation flags
            NULL,            // Use parent's environment
            NULL,            // Use parent's starting directory
            &si,             // Pointer to STARTUPINFO structure
            &pi);

        // Close unused handles
        ::CloseHandle(hChildStdoutWr);
        ::CloseHandle(hChildStdinRd);

        this->ReadUntilPrompt(!showBootInfo);

        ::free(lpCommandLine);
    }

    // text: "hello how are you?\nwhat is the answer for 4+4?"
    void Send(const char* text)
    {
        DWORD written = 0;
        ::WriteFile(hChildStdinWr, text, (DWORD)strlen(text), &written, NULL);

        // send ending marker
        const char* endMarker = ".\\\n";
        ::WriteFile(hChildStdinWr, endMarker, (DWORD)strlen(endMarker), &written, NULL);

        this->ReadUntilPrompt(false);
    }

    void SendCtrlC()
    {
        // https://blog.codetitans.pl/post/sending-ctrl-c-signal-to-another-application-on-windows/

        // Disable Ctrl-C handling for our program
        ::SetConsoleCtrlHandler(NULL, true);

        if (::AttachConsole(pi.dwProcessId))
        {
            ::GenerateConsoleCtrlEvent(CTRL_C_EVENT, pi.dwProcessId);
            ::FreeConsole();
        }
    }

    void Stop()
    {
        this->SendCtrlC();

        ::TerminateProcess(pi.hProcess, 0);

        ::CloseHandle(hChildStdoutRd);
        ::CloseHandle(hChildStdinWr);

        // Close remaining handles
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
    }
};

