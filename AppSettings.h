
// (c) 2024 CrownSoft
// https://www.crownsoft.net

#pragma once

#include "rfc/rfc.h"

#define NCHAT_FORMAT_ID KFORMAT_ID('NCS1')

class AppSettings
{
	KString settingsFilePath;

public:
	KString modelFileName; // not full path
	KString promptFileName; // not full path
	KString commandline;
	bool removeMarkdown;
	bool showInitLog;
	bool useKnowledgebase;
	bool showVecDBResponse;
	KString adapterFileName; // not full path

	AppSettings()
	{
		modelFileName = L"none";
		promptFileName = L"Default.txt";
		adapterFileName = L"none";

		this->ResetConfig();

		settingsFilePath = KDirectory::GetModuleDir(NULL) + L"\\settings.cfg";
	}

	void ResetConfig()
	{
		showInitLog = false;
		removeMarkdown = false;
		useKnowledgebase = false;
		showVecDBResponse = false;
		commandline = L"-cnv -n 4096 --simple-io -mli --no-warmup";
	}

	void LoadSettings()
	{
		KSettingsReader settingsReader;
		if (settingsReader.OpenFile(settingsFilePath, NCHAT_FORMAT_ID))
		{
			modelFileName = settingsReader.ReadString();
			promptFileName = settingsReader.ReadString();
			commandline = settingsReader.ReadString();
			removeMarkdown = settingsReader.ReadBool();
			showInitLog = settingsReader.ReadBool();
			useKnowledgebase = settingsReader.ReadBool();
			showVecDBResponse = settingsReader.ReadBool();
			adapterFileName = settingsReader.ReadString();
		}
	}

	void SaveSettings()
	{
		KSettingsWriter settingsWriter;
		if (settingsWriter.OpenFile(settingsFilePath, NCHAT_FORMAT_ID))
		{
			settingsWriter.WriteString(modelFileName);
			settingsWriter.WriteString(promptFileName);
			settingsWriter.WriteString(commandline);
			settingsWriter.WriteBool(removeMarkdown);
			settingsWriter.WriteBool(showInitLog);
			settingsWriter.WriteBool(useKnowledgebase);
			settingsWriter.WriteBool(showVecDBResponse);
			settingsWriter.WriteString(adapterFileName);
		}
	}
};

