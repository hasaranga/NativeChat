
#pragma once

#include "rfc/rfc.h"
#include <stdio.h>

class VectorGen
{
protected:
    KString programPath;
    KString tempFilePath;
    KString modelFilePath;
    KString cmdline;

    // free using: HeapFree(GetProcessHeap(), 0, output);
    // commandline must begin with space
    char* RunProgram(const wchar_t* executablePath, const wchar_t* commandline)
    {
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;  // Handle inheritance
        saAttr.lpSecurityDescriptor = NULL;

        // Create pipes for stdout
        HANDLE hStdOutRead = NULL;
        HANDLE hStdOutWrite = NULL;
        if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0)) {
            return NULL;
        }

        // Ensure the read handle to the pipe is not inherited
        if (!SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0)) {
            CloseHandle(hStdOutRead);
            CloseHandle(hStdOutWrite);
            return NULL;
        }

        // Create process startup info
        STARTUPINFOW si = { 0 };
        si.cb = sizeof(STARTUPINFOW);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;  // Added STARTF_USESHOWWINDOW
        si.wShowWindow = SW_HIDE;  // Hide the window
        si.hStdOutput = hStdOutWrite;
        si.hStdError = hStdOutWrite;
        si.hStdInput = NULL;

        // Create process info
        PROCESS_INFORMATION pi = { 0 };

        // Create command line buffer
        wchar_t* cmdLine = _wcsdup(commandline);
        if (!cmdLine) {
            CloseHandle(hStdOutRead);
            CloseHandle(hStdOutWrite);
            return NULL;
        }

        // Create the process with CREATE_NO_WINDOW flag
        BOOL success = CreateProcessW(
            executablePath,
            cmdLine,
            NULL,
            NULL,
            TRUE,
            CREATE_NO_WINDOW,  // Added CREATE_NO_WINDOW flag
            NULL,
            NULL,
            &si,
            &pi
        );

        free(cmdLine);
        CloseHandle(hStdOutWrite); // Close unused write handle

        if (!success) {
            CloseHandle(hStdOutRead);
            return NULL;
        }

        // Read output from pipe
        DWORD bytesRead;
        char buffer[4096];
        HANDLE heap = GetProcessHeap();
        SIZE_T outputSize = 0;
        SIZE_T outputCapacity = 4096;
        char* output = (char*)HeapAlloc(heap, 0, outputCapacity);

        if (!output) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            CloseHandle(hStdOutRead);
            return NULL;
        }

        while (TRUE) {
            if (!ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) || bytesRead == 0) {
                break;
            }

            if (outputSize + bytesRead + 1 > outputCapacity) {
                outputCapacity *= 2;
                char* newOutput = (char*)HeapReAlloc(heap, 0, output, outputCapacity);
                if (!newOutput) {
                    HeapFree(heap, 0, output);
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                    CloseHandle(hStdOutRead);
                    return NULL;
                }
                output = newOutput;
            }

            memcpy(output + outputSize, buffer, bytesRead);
            outputSize += bytesRead;
        }

        output[outputSize] = '\0';

        // Wait for process to complete
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Clean up process handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hStdOutRead);

        return output;
    }

    // returns NULL on error
    // must free the returned array using free.
    float* ExtractEmbeddings(const char* input) {
        const int EMBEDDING_SIZE = 768;
        float* embedding = (float*)malloc(EMBEDDING_SIZE * sizeof(float));
        if (embedding == NULL) {
            return NULL;
        }

        const char* start = strstr(input, "\"embedding\": [");
        if (!start) {
            free(embedding);
            return NULL;
        }

        start += strlen("\"embedding\": [");
        char* end;

        for (int i = 0; i < EMBEDDING_SIZE; i++) {
            embedding[i] = strtof(start, &end);
            if (start == end) {
                free(embedding);
                return NULL;
            }
            start = end + 1; // Move past the comma or closing bracket
        }

        return embedding;
    }

    void WriteQueryFile(const KString& filename, const char* query)
    {
        ::DeleteFileW(filename);

        KFile file(filename);
        const char* prefix = "search_query: ";
        file.WriteFile((void*)prefix, (DWORD)strlen(prefix));
        file.WriteFile((void*)query, (DWORD)strlen(query));
    }

    void WriteDocumentFile(const KString& filename, const char* query)
    {
        ::DeleteFileW(filename);

        KFile file(filename);
        const char* prefix = "search_document: ";
        file.WriteFile((void*)prefix, (DWORD)strlen(prefix));
        file.WriteFile((void*)query, (DWORD)strlen(query));
    }

public:
    VectorGen(const KString& appFolder)
    {
        programPath = appFolder + CONST_TXT("\\llama-embedding.exe");
        tempFilePath = appFolder + CONST_TXT("\\temp.txt");
        modelFilePath = appFolder + CONST_TXT("\\nomic-embed-text-v1.5.Q8_0.gguf");
        cmdline = CONST_TXT(" -m \"") + modelFilePath + CONST_TXT("\" -c 8192 -b 8192 --rope-scaling yarn --rope-freq-scale .75 -f \"") +
            tempFilePath + CONST_TXT("\" --embd-output-format json --embd-normalize 2 --embd-separator \"<#sep#>\"");
    }

    bool IsEmbeddingModelExists()
    {
        return KFile::IsFileExists(modelFilePath);
    }

    // caller must free the result.
    float* VectorQuery(const char* query)
    {
        this->WriteQueryFile(tempFilePath, query);
        char* output = this->RunProgram(programPath, cmdline);

        float* embeddings = NULL;
        if (output)
        {
            embeddings = this->ExtractEmbeddings(output);
            ::HeapFree(::GetProcessHeap(), 0, output);
        }
        else
        {
            ::MessageBoxW(0, L"llama-embedding.exe running error!", L"Error", MB_ICONERROR);
        }

        ::DeleteFileW(tempFilePath);

        return embeddings;
    }

    // caller must free result.
    float* VectorDocument(const char* document)
    {
        this->WriteDocumentFile(tempFilePath, document);
        char* output = this->RunProgram(programPath, cmdline);

        float* embeddings = NULL;
        if (output)
        {
            embeddings = this->ExtractEmbeddings(output);
            ::HeapFree(::GetProcessHeap(), 0, output);
        }
        else
        {
            ::MessageBoxW(0, L"llama-embedding.exe running error!", L"Error", MB_ICONERROR);
        }

        ::DeleteFileW(tempFilePath);

        return embeddings;
    }

};

