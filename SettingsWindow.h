
// (c) 2024 CrownSoft
// https://www.crownsoft.net

#pragma once

#include "rfc/rfc.h"
#include "AppSettings.h"

class SettingsWindow : public KDialog, public KButtonListener
{
	KTextBox txtCmdline;
	KScopedClassPointer<KFont> fontSegoeUI;
	KLabel lblCmdline;
	KCheckBox chkRemoveMarkdown;
	KCheckBox chkShowInit;
	KCheckBox chkShowVecDBResp;
	KGlyphButton btnOK, btnReset;

public:
	AppSettings* appSettings;
	KFont* fontWingdings;

	SettingsWindow()
	{
		appSettings = nullptr;

		this->SetText(L"Settings");
		this->SetCloseOperation(KCloseOperation::Hide);
	}

	bool Create(bool requireInitialMessages = false) override
	{
		KDialog::Create();

		const int clientWidth = KDPIUtility::ScaleToNewDPI(600, this->GetDPI());
		const int clientHeight = KDPIUtility::ScaleToNewDPI(145, this->GetDPI());
		this->SetClientAreaSize(clientWidth, clientHeight);
		this->CenterOnSameMonitor(compParentHWND);

		fontSegoeUI = new KFont(L"Segoe UI", 14, false, false, false, true, this->GetDPI());

		lblCmdline.SetText(L"Commandline");
		lblCmdline.SetFont(fontSegoeUI);
		lblCmdline.SetPosition(10, 22);
		lblCmdline.SetSize(80, lblCmdline.GetHeight());

		txtCmdline.SetSize(490, txtCmdline.GetHeight());
		txtCmdline.SetFont(fontSegoeUI);
		txtCmdline.SetPosition(lblCmdline.GetX() + lblCmdline.GetWidth() + 5, 20);

		chkRemoveMarkdown.SetText(L"Remove Markdown");
		chkRemoveMarkdown.SetFont(fontSegoeUI);
		chkRemoveMarkdown.SetSize(120, chkRemoveMarkdown.GetHeight());
		chkRemoveMarkdown.SetPosition(txtCmdline.GetX(), txtCmdline.GetY() + txtCmdline.GetHeight() + 15);

		chkShowInit.SetText(L"Show Initialization");
		chkShowInit.SetFont(fontSegoeUI);
		chkShowInit.SetSize(120, chkShowInit.GetHeight());
		chkShowInit.SetPosition(txtCmdline.GetX() + 170, chkRemoveMarkdown.GetY());

		chkShowVecDBResp.SetText(L"Show VecDB Result");
		chkShowVecDBResp.SetFont(fontSegoeUI);
		chkShowVecDBResp.SetSize(120, chkShowVecDBResp.GetHeight());
		chkShowVecDBResp.SetPosition(chkShowInit.GetX() + 170, chkRemoveMarkdown.GetY());

		btnOK.SetText(L"Save");
		btnOK.SetFont(fontSegoeUI);
		btnOK.SetGlyph(L"\x3C", fontWingdings, RGB(0, 0, 255), 13);
		btnOK.SetPosition(txtCmdline.GetX() + txtCmdline.GetWidth() - 100, txtCmdline.GetY() + txtCmdline.GetHeight() + 60);
		btnOK.SetListener(this);

		btnReset.SetText(L"Reset");
		btnReset.SetFont(fontSegoeUI);
		btnReset.SetGlyph(L"\xC3", fontWingdings, RGB(0, 180, 0), 13);
		btnReset.SetPosition(btnOK.GetX() - (btnReset.GetWidth() + 5), txtCmdline.GetY() + txtCmdline.GetHeight() + 60);
		btnReset.SetListener(this);

		this->AddComponent(&lblCmdline);
		this->AddComponent(&txtCmdline);
		this->AddComponent(&chkRemoveMarkdown);
		this->AddComponent(&chkShowInit);
		this->AddComponent(&chkShowVecDBResp);
		this->AddComponent(&btnReset);
		this->AddComponent(&btnOK);

		return true;
	}

	void OnButtonPress(KButton* button) override
	{
		if (button == &btnOK)
		{
			KString cmdline(txtCmdline.GetText());
			if (cmdline.GetLength() > 0)
			{
				appSettings->commandline = cmdline;
				appSettings->removeMarkdown = chkRemoveMarkdown.IsChecked();
				appSettings->showInitLog = chkShowInit.IsChecked();
				appSettings->showVecDBResponse = chkShowVecDBResp.IsChecked();
				appSettings->SaveSettings();
				this->SetVisible(false);
			}
		}
		else if (button == &btnReset)
		{
			appSettings->ResetConfig();
			txtCmdline.SetText(appSettings->commandline);
			chkRemoveMarkdown.SetCheckedState(appSettings->removeMarkdown);
			chkShowInit.SetCheckedState(appSettings->showInitLog);
			chkShowVecDBResp.SetCheckedState(appSettings->showVecDBResponse);
		}
	}

	void ShowSettings()
	{
		if (this->IsVisible())
			return;

		// apply settings
		txtCmdline.SetText(appSettings->commandline);
		chkRemoveMarkdown.SetCheckedState(appSettings->removeMarkdown);
		chkShowInit.SetCheckedState(appSettings->showInitLog);
		chkShowVecDBResp.SetCheckedState(appSettings->showVecDBResponse);

		this->SetVisible(true);
	}
};
