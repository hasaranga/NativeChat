
// NativeChat v0.1
// (c) 2024 R.Hasaranga
// https://www.hasaranga.com

#include "rfc/rfc.h"
#include "ResponseTextArea.h"
#include "CustomMessages.h"
#include "ChatTextArea.h"
#include "AppSettings.h"
#include "SettingsWindow.h"
#include "LlamaCLIHost.h"
#include "VectorDB.h"
#include "VectorGen.h"
#include "resource.h"
#include <time.h>

#define MAIN_WINDOW_WIDTH 800
#define MAIN_WINDOW_HEIGHT 600

class MainWindow : public KFrame, public KMenuItemListener, 
	public LlamaCLIResponseHandler, public KRunnable, public KButtonListener
{
protected:
	KMenuBar menuBar;
	KMenu mFile, mPrompts, mModels, mAdapters, mHelp;
	KMenuItem miUseKB, miAddToKB, miResetKB, miSettings, miExit, miAbout;
	KGlyphButton btnSend, btnStop;
	KLabel lblCompany;
	KToolTip tipSend;

	KFont* fontChat;
	KFont* fontWingdings;
	KIcon windowIcon;

	ResponseTextArea textResponse;
	ChatTextArea textChat;
	KString strResponseBackup;
	KString strResponseWithMarkdown;

	LlamaCLIHost cliHost;
	KThread cliThread;
	HANDLE evtSendText;
	volatile bool running;
	volatile bool shouldBlockCtrlS;
	volatile bool forceStopped;

	KString chatText; // textbox content
	COLORREF colTextBoxBack = RGB(37,37,38);
	COLORREF colTextBoxText = RGB(210, 210, 210);

	SettingsWindow settingsWindow;
	AppSettings appSettings;
	KPointerList<KString*>* modelFileList;
	KPointerList<KMenuItem*> modelMenuItemList;
	KPointerList<KString*>* promptFileList;
	KPointerList<KMenuItem*> promptMenuItemList;
	KPointerList<KString*> adapterFileList;
	KPointerList<KMenuItem*> adapterMenuItemList;
	KString appDir;

	VectorDB* vectorDB;
	VectorGen* vectorGen;

	void ScrollToBottom()
	{
		::SendMessageW(textResponse.GetHWND(), EM_LINESCROLL, 0, 1000);
	}

	void AppendToResponse(const KString& text)
	{
		KString currentText(textResponse.GetText());
		textResponse.SetText(currentText + text);

		this->ScrollToBottom();
	}

	LRESULT OnColorStatic(WPARAM wParam, LPARAM lParam)
	{
		HWND ctlHwnd = (HWND)lParam;
		if (ctlHwnd == textResponse.GetHWND())
		{
			HDC hdc = (HDC)wParam;
			::SetTextColor(hdc, colTextBoxText);
			::SetBkColor(hdc, colTextBoxBack);
			::SetDCBrushColor(hdc, colTextBoxBack);
			return (LRESULT)::GetStockObject(DC_BRUSH); // return a DC brush.
		}
		else
		{
			return KFrame::WindowProc(compHWND, WM_CTLCOLORSTATIC, wParam, lParam);
		}
	}

	LRESULT OnColorEdit(WPARAM wParam, LPARAM lParam)
	{
		HWND ctlHwnd = (HWND)lParam;
		if (ctlHwnd == textChat.GetHWND())
		{
			HDC hdc = (HDC)wParam;
			::SetTextColor(hdc, colTextBoxText);
			::SetBkColor(hdc, colTextBoxBack);
			::SetDCBrushColor(hdc, colTextBoxBack);
			return (LRESULT)::GetStockObject(DC_BRUSH); // return a DC brush.
		}
		else
		{
			return KFrame::WindowProc(compHWND, WM_CTLCOLOREDIT, wParam, lParam);
		}
	}

	wchar_t* EscapeAmpersands(const wchar_t* src) {
		if (src == NULL) {
			return NULL;
		}

		size_t src_len = wcslen(src);
		size_t amp_count = 0;

		// First pass: Count the number of '&' characters
		for (size_t i = 0; i < src_len; ++i) {
			if (src[i] == L'&') {
				amp_count++;
			}
		}

		// Calculate the length of the new string
		// Each '&' is replaced by '&&', so we need amp_count additional characters
		size_t new_len = src_len + amp_count;

		// Allocate memory for the new string (+1 for the null terminator)
		wchar_t* dest = (wchar_t*)malloc((new_len + 1) * sizeof(wchar_t));
		if (dest == NULL) {
			// Memory allocation failed
			return NULL;
		}

		// Second pass: Populate the new string with '&&' in place of '&'
		size_t j = 0; // Index for dest
		for (size_t i = 0; i < src_len; ++i) {
			if (src[i] == L'&') {
				dest[j++] = L'&'; // First '&'
				dest[j++] = L'&'; // Second '&'
			}
			else {
				dest[j++] = src[i];
			}
		}

		// Null-terminate the new string
		dest[j] = L'\0';

		return dest;
	}

	void AddToAdapterList(const wchar_t* adapterFile)
	{
		KString strAdapterFile(adapterFile);

		if (!KFile::IsFileExists(appDir + L"\\adapters\\" + strAdapterFile))
			return;

		adapterFileList.AddPointer(new KString(strAdapterFile));

		KString adapterFileNameWithoutExt(strAdapterFile.SubString(0,
			strAdapterFile.GetLength() - 6));

		KMenuItem* mItem = new KMenuItem();
		mItem->SetText(adapterFileNameWithoutExt);
		mItem->SetListener(this);

		adapterMenuItemList.AddPointer(mItem);
		mAdapters.AddMenuItem(mItem);
	}

	void SplitAndProcessLines(const wchar_t* str) {
		const wchar_t* start = str;
		const wchar_t* end;

		while ((end = wcsstr(start, L"\r\n")) != nullptr) {
			size_t lineLength = end - start;

			// Allocate buffer for line and copy it as a null-terminated string
			wchar_t* line = new wchar_t[lineLength + 1];
			wcsncpy_s(line, lineLength + 1, start, lineLength);
			line[lineLength] = L'\0';

			this->AddToAdapterList(line);  // Call ProcessLine with the line

			delete[] line;  // Free the allocated buffer
			start = end + 2;  // Move start past the "\r\n"
		}

		// Process the last line if there's no trailing "\r\n"
		if (*start != L'\0') {
			size_t remainingLength = wcslen(start);
			wchar_t* line = new wchar_t[remainingLength + 1];
			wcscpy_s(line, remainingLength + 1, start);
			this->AddToAdapterList(line);
			delete[] line;
		}
	}

	void PopulateAdapterListForModel(const KString& modelFileNameWithoutExt)
	{
		// add first item (none)

		adapterFileList.AddPointer(new KString(L"none"));

		KMenuItem* mItem = new KMenuItem();
		mItem->SetText(CONST_TXT("none"));

		if (appSettings.adapterFileName.Compare(L"none"))
			mItem->SetCheckedState(true);

		mItem->SetListener(this);

		adapterMenuItemList.AddPointer(mItem);
		mAdapters.AddMenuItem(mItem);

		// read adapter list for given model file
		KString adapterListFilePath(appDir + L"\\adapters\\" + modelFileNameWithoutExt + L".txt");

		if (KFile::IsFileExists(adapterListFilePath))
		{
			KFile adapterListFile;
			adapterListFile.OpenFile(adapterListFilePath, KFile::KREAD);

			KString strAdapterList = adapterListFile.ReadAsString(false);
			this->SplitAndProcessLines(strAdapterList);

			// check saved adapter
			bool found = false;
			for (int i = 0; i < adapterFileList.GetSize(); i++)
			{
				if (appSettings.adapterFileName.Compare(*adapterFileList[i]))
				{
					adapterMenuItemList[i]->SetCheckedState(true);
					found = true;
					break;
				}			
			}

			if (!found)
			{
				mItem->SetCheckedState(true);
				appSettings.adapterFileName = L"none";
			}
		}
		else
		{
			mItem->SetCheckedState(true);
			appSettings.adapterFileName = L"none";
		}

	}

public:
	MainWindow() : cliHost(this) , modelMenuItemList(20), promptMenuItemList(32)
	{
		appDir = KDirectory::GetModuleDir(NULL);
		appSettings.LoadSettings();	

		KString dbPath(appDir + L"\\embeddings.db");
		KString vecExtPath(appDir + L"\\vec0.dll");
		char* ansiDBPath = KString::ToAnsiString(dbPath);
		char* ansiExtPath = KString::ToAnsiString(vecExtPath);
		vectorDB = new VectorDB(ansiExtPath, ansiDBPath);
		vectorDB->CreateTables();
		::free(ansiDBPath);
		::free(ansiExtPath);

		vectorGen = new VectorGen(appDir);

		forceStopped = false;
		shouldBlockCtrlS = true;
		running = false;
		this->SetText(L"NativeChat");
		this->Create();

		windowIcon.SetResource(IDI_ICON1);
		this->SetIcon(&windowIcon);
		
		const int clientWidth = KDPIUtility::ScaleToNewDPI(MAIN_WINDOW_WIDTH, this->GetDPI());
		const int clientHeight = KDPIUtility::ScaleToNewDPI(MAIN_WINDOW_HEIGHT, this->GetDPI());
		this->SetClientAreaSize(clientWidth, clientHeight);

		fontChat = new KFont(L"Segoe UI", 18, false, false, false, true, this->GetDPI());
		fontWingdings = new KFont(L"Wingdings", 20, false, false, false, true, this->GetDPI());

		textResponse.SetPosition(5, 5);
		textResponse.SetSize(MAIN_WINDOW_WIDTH - 10, 400);
		textResponse.SetFont(fontChat);
		
		textChat.SetPosition(5, textResponse.GetY() + textResponse.GetHeight()+5);
		textChat.SetSize(MAIN_WINDOW_WIDTH - 10, 130);
		textChat.SetFont(fontChat);

		btnSend.SetText(L"Send");
		btnSend.SetFont(fontChat);
		btnSend.SetGlyph(L"\xD8", fontWingdings, RGB(0, 0, 255), 13);
		btnSend.SetPosition((MAIN_WINDOW_WIDTH - 5) - btnSend.GetWidth(), textChat.GetY() + textChat.GetHeight() + 5);
		btnSend.SetListener(this);

		btnStop.SetText(L"Stop");
		btnStop.SetFont(fontChat);
		btnStop.SetGlyph(L"\x6E", fontWingdings, RGB(255, 0, 0), 13);
		btnStop.SetPosition(btnSend.GetX() - (btnStop.GetWidth() + 5), btnSend.GetY());
		btnStop.SetEnabled(false);
		btnStop.SetListener(this);

		lblCompany.SetText(L"NativeChat v0.1");
		lblCompany.SetFont(fontChat);
		lblCompany.SetSize(130, lblCompany.GetHeight());
		lblCompany.SetEnabled(false);
		lblCompany.SetPosition(10, textChat.GetY() + textChat.GetHeight() + 10);

		this->AddComponent(&textResponse);
		this->AddComponent(&textChat);
		this->AddComponent(&btnStop);
		this->AddComponent(&btnSend);
		this->AddComponent(&lblCompany);

		tipSend.SetText(L"Shortcut: Ctrl+S");
		tipSend.AttachToComponent(this, &btnSend);

		miUseKB.SetText(L"Answer Using Vector Database");
		if(appSettings.useKnowledgebase)
			miUseKB.SetCheckedState(true);

		miAddToKB.SetText(L"Add to Vector Database...");
		miResetKB.SetText(L"Reset Vector Database...");
		miSettings.SetText(L"Settings...");
		miExit.SetText(L"Exit");
		miAbout.SetText(L"About...");

		miUseKB.SetListener(this);
		miAddToKB.SetListener(this);
		miSettings.SetListener(this);
		miResetKB.SetListener(this);
		miExit.SetListener(this);
		miAbout.SetListener(this);

		// add menu items into menu
		mFile.AddSubMenu(L"Prompt", &mPrompts);
		mFile.AddSubMenu(L"Model", &mModels);
		mFile.AddSubMenu(L"LoRA Adapter", &mAdapters);
		mFile.AddSeperator();
		mFile.AddMenuItem(&miUseKB);
		mFile.AddMenuItem(&miAddToKB);
		mFile.AddMenuItem(&miResetKB);
		mFile.AddSeperator();
		mFile.AddMenuItem(&miSettings);
		mFile.AddSeperator();
		mFile.AddMenuItem(&miExit);

		mHelp.AddMenuItem(&miAbout);

		// add menu into menubar
		menuBar.AddMenu(L"File", &mFile);
		menuBar.AddMenu(L"Help", &mHelp);

		menuBar.AddToWindow(this); // add menubar into the window

		settingsWindow.appSettings = &appSettings;
		settingsWindow.fontWingdings = fontWingdings;
		settingsWindow.SetParentHWND(compHWND);
		settingsWindow.Create();

		evtSendText = ::CreateEventW(NULL, FALSE, FALSE, NULL);

		btnSend.SetEnabled(false);

		// scan for model dir...
		modelFileList = KDirectory::ScanFolderForExtension(appDir + L"\\models", L"gguf");
		for (int i = 0; i < modelFileList->GetSize(); i++)
		{
			KMenuItem* mItem = new KMenuItem();
			KString modelFileName = *modelFileList->GetPointer(i);
			mItem->SetText(modelFileName.SubString(0, modelFileName.GetLength() - 6)); // remove .gguf

			if (appSettings.modelFileName.Compare(modelFileName))
				mItem->SetCheckedState(true);

			mItem->SetListener(this);

			modelMenuItemList.AddPointer(mItem);
			mModels.AddMenuItem(mItem);
		}

		// scan for prompt dir...
		promptFileList = KDirectory::ScanFolderForExtension(appDir + L"\\prompts", L"txt");
		for (int i = 0; i < promptFileList->GetSize(); i++)
		{
			KMenuItem* mItem = new KMenuItem();
			KString promptFileName = *promptFileList->GetPointer(i);
			KString fnameWithoutExt = promptFileName.SubString(0, promptFileName.GetLength() - 5);

			mItem->SetText(KString(EscapeAmpersands(fnameWithoutExt), KString::FREE_TEXT_WHEN_DONE));

			if (appSettings.promptFileName.Compare(promptFileName))
				mItem->SetCheckedState(true);

			mItem->SetListener(this);

			promptMenuItemList.AddPointer(mItem);
			mPrompts.AddMenuItem(mItem);
		}

		if (appSettings.modelFileName.Compare(L"none") ||
			(!KFile::IsFileExists(appDir + L"\\models\\" + appSettings.modelFileName)))
		{
			// get first model in model dir
			if (modelFileList->GetSize())
			{
				appSettings.modelFileName = *modelFileList->GetPointer(0);			
			}
			else
			{
				// do not start thread if there are no models
				this->SetText(L"NativeChat - no models");
				return;
			}
		}

		KString modelFileNameWithoutExt(appSettings.modelFileName.SubString(0,
			appSettings.modelFileName.GetLength() - 6));

		this->PopulateAdapterListForModel(modelFileNameWithoutExt);

		if (!KFile::IsFileExists(appDir + L"\\prompts\\" + appSettings.promptFileName))
			appSettings.promptFileName = L"Default.txt";

		this->SetText(L"NativeChat - " + appSettings.promptFileName.SubString(0,
			appSettings.promptFileName.GetLength() - 5) + L" - " + modelFileNameWithoutExt);
		
		cliThread.SetRunnable(this);
		cliThread.StartThread();
	}

	void OnCustomMessage(WPARAM msgID, LPARAM param) override
	{
		if (msgID == MODEL_LOADING_MSG)
		{
			if (!appSettings.showInitLog)
				this->AppendToResponse(L"Loading model...\r\n");
		}
		else if ((msgID == MODEL_LOADED_MSG) || (msgID == MODEL_PROCESSING_DONE_MSG))
		{
			if (!appSettings.showInitLog)
			{
				if (msgID == MODEL_LOADED_MSG)
				{
					this->AppendToResponse(L"Done.\r\n--------------------------------------"
						"----------------------------------------------------------------\r\n");
				}
			}

			btnSend.SetEnabled(true);
			btnStop.SetEnabled(false);
			shouldBlockCtrlS = false;
		}
		else if (msgID == MODEL_PROCESSING_BEGIN_MSG)
		{
			btnSend.SetEnabled(false);
			btnStop.SetEnabled(true);
		}
		else if (msgID == APPEND_TEXT_MSG)
		{
			char* text = (char*)param;
			KString unicodeText(text);
			strResponseWithMarkdown = strResponseWithMarkdown + unicodeText;
			this->AppendToResponse(unicodeText);
			::free(text);
		}
		else if (msgID == FINISH_APPEND_TEXT_MSG)
		{
			const wchar_t* lineBreak = L"--------------------------------------"
				"----------------------------------------------------------------\r\n";

			const wchar_t* lineBreakForceStopped = L"\r\n--------------------------------------"
				"----------------------------------------------------------------\r\n";

			if (appSettings.removeMarkdown)
			{
				wchar_t* txtWithoutMarkdown = LlamaCLIHost::RemoveMarkdown(strResponseWithMarkdown);
				textResponse.SetText(strResponseBackup + KString(txtWithoutMarkdown, KString::FREE_TEXT_WHEN_DONE) +
					(forceStopped ? lineBreakForceStopped : lineBreak));

				this->ScrollToBottom();
			}
			else
			{
				this->AppendToResponse((forceStopped ? lineBreakForceStopped : lineBreak));
			}
			forceStopped = false;
			shouldBlockCtrlS = false;
		}
		else if (msgID == CTRL_S_PRESSED_MSG)
		{
			if (!shouldBlockCtrlS)
				this->OnButtonPress(&btnSend);
		}
	}

	void OnLlamaData(const char* text, bool emptyText, bool finish) override
	{
		if (!emptyText)
			this->PostCustomMessage(APPEND_TEXT_MSG, (LPARAM)_strdup(text));

		if(finish)
			this->PostCustomMessage(FINISH_APPEND_TEXT_MSG, 0);
	}

	void Run(KThread* thread) override
	{
		this->PostCustomMessage(MODEL_LOADING_MSG, 0);
		
		KString promptArg;
		if (appSettings.promptFileName.Compare(L"Default.txt"))
			promptArg = L"-p \"You are PC. You are a helpful assistant.\" ";
		else
			promptArg = L"-f \"" + appDir + L"\\prompts\\" + appSettings.promptFileName + L"\" ";

		KString cmdline = L" -m \""+ appDir + L"\\models\\"+appSettings.modelFileName + L"\" " +
			promptArg + appSettings.commandline;

		if (!appSettings.adapterFileName.Compare(L"none"))
		{
			cmdline = cmdline + L" --lora \"" + appDir + L"\\adapters\\" +
				appSettings.adapterFileName + L"\"";
		}

		cliHost.Start(appDir + L"\\llama-cli.exe",
			cmdline, appSettings.showInitLog);

		this->PostCustomMessage(MODEL_LOADED_MSG, 0);

		while (thread->ShouldRun())
		{
			::WaitForSingleObject(evtSendText, INFINITE);

			if (!thread->ShouldRun())
				break;

			running = true;
			this->PostCustomMessage(MODEL_PROCESSING_BEGIN_MSG, 0);

			char* utf8Text = KString::ToAnsiString(chatText);
			if (appSettings.useKnowledgebase)
			{
				float* vector = vectorGen->VectorQuery(utf8Text);
				if (vector)
				{
					char* strParagraph = vectorDB->Query(vector);
					if (strParagraph)
					{
						const char* txt1 = "you can use the provided context to answer the following question."
							" ignore the context if it does not have the relevant information."
							"\n\nQuestion: ";

						const char* txt2 = "\n\nContext: ";
						const int szFinalText = (int)strlen(txt1) + (int)strlen(utf8Text) + 
							(int)strlen(txt2) + (int)strlen(strParagraph) + 1;

						char* finalPrompt = (char*)malloc(szFinalText);
						finalPrompt[0] = 0;

						::strcat_s(finalPrompt, szFinalText, txt1);
						::strcat_s(finalPrompt, szFinalText, utf8Text);
						::strcat_s(finalPrompt, szFinalText, txt2);
						::strcat_s(finalPrompt, szFinalText, strParagraph);

						if (appSettings.showVecDBResponse)
						{
							this->PostCustomMessage(APPEND_TEXT_MSG, (LPARAM)_strdup("VecDB: "));
							this->PostCustomMessage(APPEND_TEXT_MSG, (LPARAM)_strdup(strParagraph));
							this->PostCustomMessage(APPEND_TEXT_MSG, (LPARAM)_strdup("\r\n\r\n"));
							this->PostCustomMessage(APPEND_TEXT_MSG, (LPARAM)_strdup("LLM: "));
						}

						cliHost.Send(finalPrompt);

						::free(finalPrompt);
						::free(strParagraph);
					}
					else
					{
						cliHost.Send(utf8Text); // no data from vecdb.
					}
					::free(vector);
				}
				else
				{
					this->PostCustomMessage(APPEND_TEXT_MSG, (LPARAM)_strdup("vector generation failed!\r\n"));
					this->PostCustomMessage(FINISH_APPEND_TEXT_MSG, 0);
				}
			}
			else
			{
				cliHost.Send(utf8Text);
			}
			::free(utf8Text);

			this->PostCustomMessage(MODEL_PROCESSING_DONE_MSG, 0);
			running = false;
		}

		cliHost.Stop();
	}

	void OnButtonPress(KButton* button) override
	{
		if (button == &btnSend)
		{
			if (appSettings.useKnowledgebase)
			{
				if (!vectorGen->IsEmbeddingModelExists())
				{
					this->ShowEmbeddingModelNotFoundError();
					return;
				}
			}

			chatText = textChat.GetText();
			if (chatText.GetLength())
			{
				shouldBlockCtrlS = true;

				this->AppendToResponse(CONST_TXT("[USER] ") + chatText +
					CONST_TXT("\r\n--------------------------------------------------"
						"----------------------------------------------------\r\n[PC] "));

				strResponseBackup = textResponse.GetText();
				strResponseWithMarkdown = KString();

				::SetEvent(evtSendText);
				
				textChat.SetText(KString(L""));
			}
		}
		else if (button == &btnStop)
		{
			btnStop.SetEnabled(false);

			if (running)
			{
				forceStopped = true;
				cliHost.SendCtrlC();
			}
		}
	}

	void ShowEmbeddingModelNotFoundError()
	{
		::MessageBoxW(compHWND, L"Please download the \"nomic-embed-text-v1.5.Q8_0.gguf\" file from Hugging Face!"
			"Then copy it to the program folder.", L"Error", MB_ICONERROR);
		::ShellExecuteW(0, L"open", L"https://huggingface.co/nomic-ai/nomic-embed-text-v1.5-GGUF/tree/main", 0, 0, SW_SHOWNORMAL);
	}

	void vectorParagraph(int rowID, const char* paragraph)
	{
		float* vector = vectorGen->VectorDocument(paragraph);
		if (vector)
		{				
			vectorDB->Insert(rowID, vector, paragraph);
			::free(vector);
		}
		else
		{
			::MessageBoxW(compHWND, L"paragraph vector generation error!", L"Error", MB_ICONERROR);
		}
	}

	// split the input string into paragraphs and process each one
	void processParagraphs(const char* input) {
		const char* paragraphDelim = "\r\n\r\n";  // Windows-style paragraph delimiter
		char* inputCopy = _strdup(input);

		char* context = NULL;
		char* token = strtok_s(inputCopy, paragraphDelim, &context);

		int nextRowID = vectorDB->GetNextRowID();	

		while (token != NULL) 
		{
			this->vectorParagraph(nextRowID, token);
			++nextRowID;

			token = strtok_s(NULL, paragraphDelim, &context);
		}

		free(inputCopy);
	}

	void OnMenuItemPress(KMenuItem* menuItem) override
	{
		if (menuItem == &miUseKB)
		{
			if (!vectorGen->IsEmbeddingModelExists())
			{
				this->ShowEmbeddingModelNotFoundError();
				return;
			}

			appSettings.useKnowledgebase = !appSettings.useKnowledgebase;
			appSettings.SaveSettings();

			miUseKB.SetCheckedState(appSettings.useKnowledgebase);
		}
		else if (menuItem == &miAddToKB)
		{
			if (!vectorGen->IsEmbeddingModelExists())
			{
				this->ShowEmbeddingModelNotFoundError();
				return;
			}

			KString fileName;
			if (KCommonDialogBox::ShowOpenFileDialog(this, L"Add text file...", KFILE_FILTER("Text Files", "txt"), &fileName))
			{
				KFile txtFile(fileName);
				KString content(txtFile.ReadAsString(false)); // utf8 text file
				if (content.GetLength() > 0)
				{
					char* ansiContent = content.ToAnsiString(content);
					const int currentRowID = vectorDB->GetNextRowID();

					vectorDB->BeginInsert();
					this->processParagraphs(ansiContent);
					vectorDB->EndInsert();

					::free(ansiContent);	

					::MessageBoxW(compHWND, L"Operation complete! New rows: " + KString(vectorDB->GetNextRowID() - currentRowID),
						L"Finished", MB_ICONINFORMATION);
				}
				else 
				{
					::MessageBoxW(compHWND, L"File is empty!", L"Error", MB_ICONERROR);
				}
			}
		}
		else if (menuItem == &miResetKB)
		{
			const int result = MessageBoxW(compHWND, L"Are you sure you want to clear the vector database?", 
				L"Confirm", MB_YESNO | MB_ICONQUESTION);

			if (result == IDYES)
				vectorDB->ResetDB();
		}
		else if (menuItem == &miSettings)
		{
			settingsWindow.ShowSettings();
		}
		else if (menuItem == &miExit)
		{
			::PostMessageW(compHWND, WM_CLOSE, 0, 0);
		}
		else if (menuItem == &miAbout)
		{
			MSGBOXPARAMSW msgparams;
			ZeroMemory(&msgparams, sizeof(MSGBOXPARAMSW));
			msgparams.cbSize = sizeof(MSGBOXPARAMSW);
			msgparams.hwndOwner = compHWND;
			msgparams.hInstance = KApplication::hInstance;
			msgparams.dwStyle = MB_OK | MB_USERICON;
			msgparams.lpszIcon = MAKEINTRESOURCEW(IDI_ICON1);
			msgparams.lpszText = L"NativeChat v0.1\nLightweight front-end for llama-cli\n"
				"Developed by R.Hasaranga\nhttps://github.com/hasaranga/NativeChat\n\n"
				"llama-cli: https://github.com/ggerganov/llama.cpp\nsqlite-vec: https://github.com/asg017/sqlite-vec\n"
				"sqlite3: https://sqlite.org\nrfc: https://github.com/hasaranga/RFC-Framework\n"
				"prompts: https://github.com/f/awesome-chatgpt-prompts\nicon: https://www.vexels.com";
			msgparams.lpszCaption = L"About";

			::MessageBoxIndirectW(&msgparams);
		}
		else
		{
			for (int i = 0; i < modelMenuItemList.GetSize(); i++)
			{
				if (menuItem == modelMenuItemList[i])
				{
					appSettings.modelFileName = *modelFileList->GetPointer(i);
					appSettings.SaveSettings();

					this->OnClose();

					::ShellExecuteW(NULL, L"open", KDirectory::GetModuleFilePath(NULL), 
						NULL, NULL, SW_SHOWNORMAL);

					return;
				}
			}

			for (int i = 0; i < promptMenuItemList.GetSize(); i++)
			{
				if (menuItem == promptMenuItemList[i])
				{
					appSettings.promptFileName = *promptFileList->GetPointer(i);
					appSettings.SaveSettings();

					this->OnClose();

					::ShellExecuteW(NULL, L"open", KDirectory::GetModuleFilePath(NULL),
						NULL, NULL, SW_SHOWNORMAL);

					return;
				}
			}

			for (int i = 0; i < adapterMenuItemList.GetSize(); i++)
			{
				if (menuItem == adapterMenuItemList[i])
				{
					appSettings.adapterFileName = *adapterFileList.GetPointer(i);
					appSettings.SaveSettings();

					this->OnClose();

					::ShellExecuteW(NULL, L"open", KDirectory::GetModuleFilePath(NULL),
						NULL, NULL, SW_SHOWNORMAL);

					return;
				}
			}

		}
	}

	void OnClose() override
	{
		settingsWindow.Destroy();
		this->SetVisible(false);
		cliThread.ThreadShouldStop();
		::SetEvent(evtSendText);

		if (running)
			cliHost.SendCtrlC();

		cliThread.WaitUntilThreadFinish(false);
		
		modelMenuItemList.DeleteAll(false);
		promptMenuItemList.DeleteAll(false);
		adapterMenuItemList.DeleteAll(false);

		KFrame::OnClose();
	}

	// put given window into random position
	BOOL SetWindowPosVaried(HWND hWnd)
	{
		// Get the work area of the primary monitor
		RECT workArea;
		::SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);

		// Calculate available space
		int availWidth = workArea.right - workArea.left;
		int availHeight = workArea.bottom - workArea.top;

		// Get current window size
		RECT windowRect;
		::GetWindowRect(hWnd, &windowRect);
		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		// Calculate maximum x and y positions
		int maxX = availWidth - windowWidth;
		int maxY = availHeight - windowHeight;

		// Generate random position within the work area
		::srand((unsigned int)::time(NULL));
		int x = workArea.left + (::rand() % (maxX > 0 ? maxX : 1));
		int y = workArea.top + (::rand() % (maxY > 0 ? maxY : 1));

		// Set the window position, maintaining current size
		return ::SetWindowPos(hWnd, NULL, x, y, 0, 0,
			SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	~MainWindow()
	{
		modelFileList->DeleteAll(false);
		delete modelFileList;

		promptFileList->DeleteAll(false);
		delete promptFileList;

		adapterFileList.DeleteAll(false);

		::CloseHandle(evtSendText);
		delete fontChat;
		delete fontWingdings;

		delete vectorDB;
		delete vectorGen;
	}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_CTLCOLORSTATIC, OnColorStatic)
		ON_KMSG(WM_CTLCOLOREDIT, OnColorEdit)
	END_KMSG_HANDLER(KFrame)
};


class MyApplication : public KApplication
{
public:

	int Main(KString** argv, int argc)
	{
		MainWindow window;

		const bool noCenter = (argc > 1) && argv[1]->Compare(L"-nocenter");

		if (noCenter)
			window.SetWindowPosVaried(window.GetHWND());
		else
			window.CenterScreen();
		
		window.SetVisible(true);
		::SetForegroundWindow(window); // bring to top

		KApplication::MessageLoop();

		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::STANDARD_MODE);