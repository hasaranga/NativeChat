
// ========== RFC Generator v1.0 ==========


#include "rfc.h"


// =========== Core.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/


void InitRFCModules()
{
	// initialize modules
	RFCModuleInitFunc* initFuncList = KModuleManager::RFCModuleInitFuncList();
	for (int i = 0; i < MAX_RFC_MODULE_COUNT; ++i)
	{
		if (initFuncList[i])
		{
			if (!initFuncList[i]())
			{
				char strIndex[10];
				::_itoa_s(i, strIndex, 10);
				char msg[64];
				::strcpy_s(msg, "RFC Module Initialization error!\n\nModule index: ");
				::strcat_s(msg, strIndex);
				::MessageBoxA(0, msg, "RFC - Error", MB_ICONERROR);
				::ExitProcess(1);
			}
		}
	}
}

void DeInitRFCModules()
{
	// free modules
	RFCModuleFreeFunc* freeFuncList = KModuleManager::RFCModuleFreeFuncList();
	for (int i = (MAX_RFC_MODULE_COUNT-1); i >= 0; --i)
	{
		if (freeFuncList[i])
			freeFuncList[i]();
	}
}

void RFCDllInit()
{
	CoreModuleInitParams::hInstance = 0;
	CoreModuleInitParams::initCOMAsSTA = false;
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE;
	::InitRFCModules();
}

void RFCDllFree()
{
	::DeInitRFCModules();
}

// =========== CoreModule.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/


HINSTANCE CoreModuleInitParams::hInstance = 0;
bool CoreModuleInitParams::initCOMAsSTA = true;
KDPIAwareness CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE;

class RFC_CoreModule {
public:
	static bool RFCModuleInit()
	{
		if (!CoreModuleInitParams::hInstance)
		{
			//hInstance = ::GetModuleHandleW(NULL); // not work for dll
			::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
				GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
				(LPCWSTR)&InitRFCModules, &CoreModuleInitParams::hInstance);
		}

		KApplication::hInstance = CoreModuleInitParams::hInstance;
		KApplication::dpiAwareness = CoreModuleInitParams::dpiAwareness;

		if (KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE)
		{
			KDPIUtility::InitDPIFunctions();
			KDPIUtility::MakeProcessDPIAware(KApplication::dpiAwareness);
		}

		if (CoreModuleInitParams::initCOMAsSTA)
			::CoInitialize(NULL); //Initializes COM as STA.

		return true;
	}

	static void RFCModuleFree()
	{
		if (CoreModuleInitParams::initCOMAsSTA)
			::CoUninitialize();
	}
};

REGISTER_RFC_MODULE(0, RFC_CoreModule)

// =========== KApplication.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/


HINSTANCE KApplication::hInstance = 0;
KDPIAwareness KApplication::dpiAwareness;
bool KApplication::dpiAwareAPICalled = false;

KApplication::KApplication()
{
}

void KApplication::ModifyModuleInitParams()
{

}

int KApplication::Main(KString **argv, int argc)
{
	return 0;
}

bool KApplication::AllowMultipleInstances()
{
	return true;
}

int KApplication::AnotherInstanceIsRunning(KString **argv, int argc)
{
	return 0;
}

const wchar_t* KApplication::GetApplicationID()
{
	return L"RFC_APPLICATION";
}

void KApplication::MessageLoop(bool handleTabKey)
{
	MSG msg;

	while (::GetMessageW(&msg, NULL, 0, 0))
	{
		if (handleTabKey)
		{
			if (::IsDialogMessage(::GetActiveWindow(), &msg))
				continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
}

KApplication::~KApplication()
{
}

// =========== KDPIUtility.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/


KGetDpiForMonitor KDPIUtility::pGetDpiForMonitor = nullptr;
KSetProcessDpiAwarenessContext KDPIUtility::pSetProcessDpiAwarenessContext = nullptr;
KSetProcessDpiAwareness KDPIUtility::pSetProcessDpiAwareness = nullptr;
KSetProcessDPIAware KDPIUtility::pSetProcessDPIAware = nullptr;
KSetThreadDpiAwarenessContext KDPIUtility::pSetThreadDpiAwarenessContext = nullptr;

void KDPIUtility::InitDPIFunctions()
{
	HMODULE hShcore = ::LoadLibraryW(L"Shcore.dll");
	if (hShcore)
	{
		KDPIUtility::pGetDpiForMonitor =
			reinterpret_cast<KGetDpiForMonitor>
			(::GetProcAddress(hShcore, "GetDpiForMonitor")); // win 8.1

		KDPIUtility::pSetProcessDpiAwareness =
			reinterpret_cast<KSetProcessDpiAwareness>
			(::GetProcAddress(hShcore, "SetProcessDpiAwareness")); // win 8.1
	}

	HMODULE hUser32 = ::LoadLibraryW(L"User32.dll");
	if (hUser32)
	{
		KDPIUtility::pSetThreadDpiAwarenessContext =
			reinterpret_cast<KSetThreadDpiAwarenessContext>
			(::GetProcAddress(hUser32, "SetThreadDpiAwarenessContext")); // win10

		KDPIUtility::pSetProcessDpiAwarenessContext =
			reinterpret_cast<KSetProcessDpiAwarenessContext>
			(::GetProcAddress(hUser32, "SetProcessDpiAwarenessContext")); // win10

		KDPIUtility::pSetProcessDPIAware =
			reinterpret_cast<KSetProcessDPIAware>
			(::GetProcAddress(hUser32, "SetProcessDPIAware")); // win7,8
	}
}

// https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-3/
WORD KDPIUtility::GetWindowDPI(HWND hWnd)
{
	if (KDPIUtility::pGetDpiForMonitor != nullptr)
	{
		HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
		UINT uiDpiX, uiDpiY;
		HRESULT hr = KDPIUtility::pGetDpiForMonitor(hMonitor, 0, &uiDpiX, &uiDpiY);

		if (SUCCEEDED(hr))
			return static_cast<WORD>(uiDpiX);
	}

	// for win8 & win7
	HDC hScreenDC = ::GetDC(0);
	int iDpiX = ::GetDeviceCaps(hScreenDC, LOGPIXELSX);
	::ReleaseDC(0, hScreenDC);

	return static_cast<WORD>(iDpiX);
}

void KDPIUtility::MakeProcessDPIAware(KDPIAwareness dpiAwareness)
{
	if (dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY)
	{
		if (KDPIUtility::pSetProcessDpiAwarenessContext)
		{
			KDPIUtility::pSetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			KApplication::dpiAwareAPICalled = true;
		}
	}
	else if (dpiAwareness == KDPIAwareness::STANDARD_MODE)
	{
		if (KDPIUtility::pSetProcessDpiAwarenessContext)
		{
			KDPIUtility::pSetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			KApplication::dpiAwareAPICalled = true;
		}
		else if (KDPIUtility::pSetProcessDpiAwareness)
		{
			KDPIUtility::pSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
			KApplication::dpiAwareAPICalled = true;
		}
		else if (KDPIUtility::pSetProcessDPIAware)
		{
			KDPIUtility::pSetProcessDPIAware();
			KApplication::dpiAwareAPICalled = true;
		}
	}
}

// https://stackoverflow.com/questions/70976583/get-real-screen-resolution-using-win32-api
float KDPIUtility::GetMonitorScalingRatio(HMONITOR monitor)
{
	MONITORINFOEXW info = {};
	info.cbSize = sizeof(MONITORINFOEXW);
	::GetMonitorInfoW(monitor, &info);
	DEVMODEW devmode = {};
	devmode.dmSize = sizeof(DEVMODEW);
	::EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
	return (info.rcMonitor.right - info.rcMonitor.left) / static_cast<float>(devmode.dmPelsWidth);
}

float KDPIUtility::GetScaleForMonitor(HMONITOR monitor)
{
	return (float)(::GetDpiForSystem() / 96.0 / GetMonitorScalingRatio(monitor));
}

int KDPIUtility::ScaleToWindowDPI(int valueFor96DPI, HWND window)
{
	return KDPIUtility::ScaleToNewDPI(valueFor96DPI, KDPIUtility::GetWindowDPI(window));
}

int KDPIUtility::ScaleToNewDPI(int valueFor96DPI, int newDPI)
{
	return ::MulDiv(valueFor96DPI, newDPI, USER_DEFAULT_SCREEN_DPI);
}

// =========== KModuleManager.cpp ===========

/*
    Copyright (C) 2013-2022 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/



bool KModuleManager::RegisterRFCModule(int index, RFCModuleInitFunc initFunc, RFCModuleFreeFunc freeFunc)
{
	KModuleManager::RFCModuleInitFuncList()[index] = initFunc;
	KModuleManager::RFCModuleFreeFuncList()[index] = freeFunc;
	return true;
}

RFCModuleInitFunc* KModuleManager::RFCModuleInitFuncList()
{
	static RFCModuleInitFunc funcList[MAX_RFC_MODULE_COUNT] = {0};
	return funcList;
}

RFCModuleFreeFunc* KModuleManager::RFCModuleFreeFuncList()
{
	static RFCModuleFreeFunc funcList[MAX_RFC_MODULE_COUNT] = {0};
	return funcList;
}

// =========== KString.cpp ===========

/*
	Copyright (C) 2013-2024 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#include <stdio.h>

const KString operator+ (const char* const string1, const KString& string2)
{
	KString s(string1);
	return s.Append(string2);
}

const KString operator+ (const wchar_t* const string1, const KString& string2)
{
	KString s(string1);
	return s.Append(string2);
}

const KString operator+ (const KString& string1, const KString& string2)
{
	return string1.Append(string2);
}

KString::KString()
{
	isZeroLength = true;
	stringHolder = nullptr;
	isStaticText = false;
}

KString::KString(const KString& other)
{
	isZeroLength = other.isZeroLength;

	if (other.isStaticText)
	{
		stringHolder = nullptr;

		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->AddReference();
		stringHolder = other.stringHolder;

		isStaticText = false;
	}
	else
	{
		stringHolder = nullptr;
		isStaticText = false;
	}
}

KString::KString(const char* const text, UINT codePage)
{
	isStaticText = false;

	if (text != nullptr)
	{
		int count = ::MultiByteToWideChar(codePage, 0, text, -1, 0, 0); // get char count with null character
		if (count)
		{
			wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
			if (::MultiByteToWideChar(codePage, 0, text, -1, w_text, count))
			{
				count--; // ignore null character

				stringHolder = new KStringHolder(w_text, count);
				isZeroLength = (count == 0);
				return;
			}
			else
			{
				::free(w_text);
			}
		}
	}

	isZeroLength = true;
	stringHolder = nullptr;
}

KString::KString(const wchar_t* const text, unsigned char behaviour, int length)
{
	if (text != nullptr)
	{
		staticTextLength = ((length == -1) ? (int)::wcslen(text) : length);
		if (staticTextLength)
		{
			isZeroLength = false;
			isStaticText = (behaviour == STATIC_TEXT_DO_NOT_FREE);

			if (isStaticText)
			{
				staticText = (wchar_t*)text;
				stringHolder = nullptr;
				return;
			}

			stringHolder = new KStringHolder(((behaviour == FREE_TEXT_WHEN_DONE) ? (wchar_t*)text : _wcsdup(text)), staticTextLength);
			return;
		}
	}

	isZeroLength = true;
	isStaticText = false;
	stringHolder = nullptr;
}

KString::KString(const int value, const int radix)
{
	stringHolder = new KStringHolder((wchar_t *)::malloc(33 * sizeof(wchar_t)), 0); // max 32 digits
	::_itow_s(value, stringHolder->w_text, 33, radix);

	stringHolder->count = (int)::wcslen(stringHolder->w_text);
	isZeroLength = (stringHolder->count == 0);
	isStaticText = false;
}

KString::KString(const float value, const int numDecimals, bool compact)
{
	isStaticText = false;

	// round it to given digits
	char *str_fmtp = (char*)malloc(32);
	char *str_buf = (char*)malloc(64);

	sprintf_s(str_fmtp, 32, "%%.%df", numDecimals);
	sprintf_s(str_buf, 64, str_fmtp, value);

	if (compact)
	{
		int len = (int)strlen(str_buf) - 1;
		for (int i = 0; i < numDecimals; i++) // kill ending zeros
		{
			if (str_buf[len - i] == '0')
				str_buf[len - i] = 0; // kill it
			else
				break;
		}

		// kill if last char is dot
		len = (int)strlen(str_buf) - 1;
		if (str_buf[len] == '.')
			str_buf[len] = 0; // kill it
	}

	int count = ::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, 0, 0); // get char count with null character
	if (count)
	{
		wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
		if (::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, w_text, count))
		{
			count--; // ignore null character

			stringHolder = new KStringHolder(w_text, count);
			isZeroLength = (count == 0);

			::free(str_buf);
			::free(str_fmtp);
			return;
		}
		else
		{
			::free(w_text);
		}
	}

	::free(str_buf);
	::free(str_fmtp);

	isZeroLength = true;
	stringHolder = nullptr;
}

const KString& KString::operator= (const KString& other)
{
	if (stringHolder)
		stringHolder->ReleaseReference();

	if (other.isStaticText)
	{
		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->AddReference();
		isStaticText = false;
	}
	else // other is empty
	{
		isStaticText = false;
	}

	stringHolder = other.stringHolder;
	isZeroLength = other.isZeroLength;

	return *this;
}

const KString& KString::operator= (const wchar_t* const other)
{
	isStaticText = false;

	if (stringHolder)
		stringHolder->ReleaseReference();

	if (other != 0)
	{
		const int count = (int)::wcslen(other);
		if (count)
		{
			stringHolder = new KStringHolder(::_wcsdup(other), count);
			isZeroLength = false;
			return *this;
		}
	}

	isZeroLength = true;
	stringHolder = nullptr;
	return *this;
}

const KString KString::operator+ (const KString& stringToAppend)
{
	return Append(stringToAppend);
}

const KString KString::operator+ (const wchar_t* const textToAppend)
{
	return Append(KString(textToAppend, USE_COPY_OF_TEXT, -1));
}

KString::operator const wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return L"";
	}
}

KString::operator wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return (wchar_t*)L"";
	}
}

const wchar_t KString::operator[](const int index)const
{
	if (!isZeroLength)
	{
		if (isStaticText)
		{
			if ((0 <= index) && (index <= (staticTextLength - 1)))
				return staticText[index];
		}
		else if(stringHolder != nullptr)
		{
			if ((0 <= index) && (index <= (stringHolder->count - 1)))
				return stringHolder->w_text[index];
		}
	}
	return -1;
}

KString KString::Append(const KString& otherString)const
{
	if (!otherString.isZeroLength)
	{
		if (!this->isZeroLength)
		{
			const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + (otherString.isStaticText ? otherString.staticTextLength : otherString.stringHolder->count);
			wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

			::wcscpy_s(destText, (totalCount + 1), isStaticText ? staticText : stringHolder->w_text);
			::wcscat_s(destText, (totalCount + 1), otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text);

			return KString(destText, FREE_TEXT_WHEN_DONE, totalCount);
		}
		else // this string is empty
		{
			return otherString;
		}
	}
	else // other string is empty
	{
		return *this;
	}
}

KString KString::AppendStaticText(const wchar_t* const text, int length, bool appendToEnd)const
{
	if (!this->isZeroLength)
	{
		const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + length;
		wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

		::wcscpy_s(destText, (totalCount + 1), appendToEnd ? (isStaticText ? staticText : stringHolder->w_text) : text);
		::wcscat_s(destText, (totalCount + 1), appendToEnd ? text : (isStaticText ? staticText : stringHolder->w_text));

		return KString(destText, FREE_TEXT_WHEN_DONE, totalCount);
	}
	else // this string is empty
	{
		return KString(text, KString::STATIC_TEXT_DO_NOT_FREE, length);
	}
}

void KString::AssignStaticText(const wchar_t* const text, int length)
{
	if (stringHolder)
		stringHolder->ReleaseReference();
	
	stringHolder = nullptr;
	isZeroLength = false;
	isStaticText = true;
	staticText = (wchar_t*)text;
	staticTextLength = length;
}

KString KString::SubString(int start, int end)const
{
	const int count = this->GetLength();

	if ((0 <= start) && (start <= (count - 1)))
	{
		if ((start < end) && (end <= (count - 1)))
		{
			int size = (end - start) + 1;
			wchar_t* buf = (wchar_t*)::malloc((size + 1) * sizeof(wchar_t));
			wchar_t* src = (isStaticText ? staticText : stringHolder->w_text);
			::wcsncpy_s(buf, (size + 1), &src[start], size);
			buf[size] = 0;

			return KString(buf, FREE_TEXT_WHEN_DONE, size);
		}
	}
	return KString();
}

bool KString::CompareIgnoreCase(const KString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::_wcsicmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool KString::Compare(const KString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool KString::CompareWithStaticText(const wchar_t* const text)const
{
	if (!this->isZeroLength)
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), text) == 0);

	return false;
}

bool KString::StartsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[0] == character) : (stringHolder->w_text[0] == character));

	return false;
}

bool KString::EndsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[staticTextLength - 1] == character) : (stringHolder->w_text[stringHolder->count - 1] == character));

	return false;
}

bool KString::IsQuotedString()const
{
	if ((isStaticText && (staticTextLength > 1)) || ((stringHolder != 0) && (stringHolder->count > 1))) // not empty + count greater than 1
		return (StartsWithChar(L'\"') && EndsWithChar(L'\"'));

	return false;
}

wchar_t KString::GetCharAt(int index)const
{
	const int count = this->GetLength();

	if ((0 <= index) && (index <= (count - 1)))
		return (isStaticText ? staticText[index] : stringHolder->w_text[index]);

	return -1;
}

int KString::GetLength()const
{
	return (isStaticText ? staticTextLength : ((stringHolder != 0) ? stringHolder->count : 0));
}

bool KString::IsEmpty()const
{
	return isZeroLength;
}

bool KString::IsNotEmpty()const
{
	return !isZeroLength;
}

int KString::GetIntValue()const
{
	if (isZeroLength)
		return 0;

	return ::_wtoi(isStaticText ? staticText : stringHolder->w_text);
}

KString KString::ToUpperCase()const
{
	if (this->GetLength() == 0)
		return KString();

	KString result((const wchar_t*)*this, KString::USE_COPY_OF_TEXT);
	::CharUpperBuffW((wchar_t*)result, result.GetLength());

	return result;
}

KString KString::ToLowerCase()const
{
	if (this->GetLength() == 0)
		return KString();

	KString result((const wchar_t*)*this, KString::USE_COPY_OF_TEXT);
	::CharLowerBuffW((wchar_t*)result, result.GetLength());

	return result;
}

char* KString::ToAnsiString(const wchar_t* text)
{
	if (text != nullptr)
	{
		const int length = ::WideCharToMultiByte(CP_UTF8, 0, text, -1, 0, 0, 0, 0);
		if (length)
		{
			char* retText = (char*)::malloc(length);
			if (::WideCharToMultiByte(CP_UTF8, 0, text, -1, retText, length, 0, 0))
				return retText;

			// conversion error
			retText[0] = 0;
			return retText;
		}
	}

	char* retText = (char*)::malloc(1);
	retText[0] = 0;
	return retText;
}

wchar_t* KString::ToUnicodeString(const char* text)
{
	if (text != nullptr)
	{
		const int length = ::MultiByteToWideChar(CP_UTF8, 0, text, -1, 0, 0);
		if (length)
		{
			wchar_t* retText = (wchar_t*)::malloc(length * sizeof(wchar_t));
			if (::MultiByteToWideChar(CP_UTF8, 0, text, -1, retText, length))
				return retText;

			// conversion error
			retText[0] = 0;
			return retText;
		}
	}

	wchar_t* retText = (wchar_t*)::malloc(sizeof(wchar_t));
	retText[0] = 0;
	return retText;	
}

KString::~KString()
{
	if (stringHolder)
		stringHolder->ReleaseReference();
}

// =========== KStringHolder.cpp ===========

/*
	Copyright (C) 2013-2024 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/



KStringHolder::KStringHolder(wchar_t *w_text, int count)
{
	refCount = 1;
	this->w_text = w_text;
	this->count = count;
}

KStringHolder::~KStringHolder()
{

}

void KStringHolder::AddReference()
{
	::InterlockedIncrement(&refCount);
}

void KStringHolder::ReleaseReference()
{
	const LONG res = ::InterlockedDecrement(&refCount);
	if(res == 0)
	{
		if(w_text)
			::free(w_text);

		delete this;
	}
}

// =========== KDirectory.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/



KDirectory::KDirectory(){}

KDirectory::~KDirectory(){}

bool KDirectory::IsDirExists(const KString& dirName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool KDirectory::CreateDir(const KString& dirName)
{
	return (::CreateDirectoryW(dirName, NULL) == 0 ? false : true);
}

bool KDirectory::RemoveDir(const KString& dirName)
{
	return (::RemoveDirectoryW(dirName) == 0 ? false : true);
}

KString KDirectory::GetModuleDir(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetModuleFilePath(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t* path = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetParentDir(const KString& filePath)
{
	wchar_t *path = ::_wcsdup(filePath);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetTempDir()
{
	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH + 1) * sizeof(wchar_t) );
	path[0] = 0;
	::GetTempPathW(MAX_PATH + 1, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetAllUserDataDir()
{
	wchar_t *path = (wchar_t*)::malloc( MAX_PATH * sizeof(wchar_t) );
	path[0] = 0;
	::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetLoggedInUserFolderPath(int csidl)
{
	DWORD dwProcessId;
	::GetWindowThreadProcessId(::GetShellWindow(), &dwProcessId);

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);

	HANDLE tokenHandle = NULL;
	::OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_IMPERSONATE, &tokenHandle);
	::CloseHandle(hProcess);

	wchar_t* path = (wchar_t*)::malloc(MAX_PATH * sizeof(wchar_t));
	path[0] = 0;
	::SHGetFolderPathW(NULL, csidl, tokenHandle, 0, path);
	::CloseHandle(tokenHandle);

	KString configDir(path, KString::FREE_TEXT_WHEN_DONE);
	return configDir;
}

KString KDirectory::GetRoamingFolder()
{
	return KDirectory::GetLoggedInUserFolderPath(CSIDL_APPDATA);
}

KString KDirectory::GetNonRoamingFolder()
{
	return KDirectory::GetLoggedInUserFolderPath(CSIDL_LOCAL_APPDATA);
}

KPointerList<KString*>* KDirectory::ScanFolderForExtension(const KString& folderPath, const KString& extension)
{
	KPointerList<KString*>* result = new KPointerList<KString*>(32, false);
	WIN32_FIND_DATAW findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	KString searchPath = folderPath + L"\\*." + extension;

	hFind = ::FindFirstFileW(searchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		return result;

	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			result->AddPointer(new KString(findData.cFileName));
		}
	} while (::FindNextFileW(hFind, &findData) != 0);

	::FindClose(hFind);

	return result;
}

// =========== KFile.cpp ===========

/*
	RFC - KFile.cpp
	Copyright (C) 2013-2019 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/


KFile::KFile()
{
	autoCloseHandle = false;
	desiredAccess = KFile::KBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

KFile::KFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool KFile::OpenFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool KFile::CloseFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE KFile::GetFileHandle()
{
	return fileHandle;
}

KFile::operator HANDLE()const
{
	return fileHandle;
}

DWORD KFile::ReadFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD KFile::WriteFile(void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool KFile::SetFilePointerToStart()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ? false : true;
}

bool KFile::SetFilePointerTo(long distance, DWORD startingPoint)
{
	return (::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::GetFilePointerPosition()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool KFile::SetFilePointerToEnd()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::GetFileSize()
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;
}

void* KFile::ReadAsData()
{
	const DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		const DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool KFile::WriteString(const KString& text, bool isUnicode)
{
	if (isUnicode)
	{
		void* buffer = (void*)(const wchar_t*)text;
		const DWORD numberOfBytesToWrite = text.GetLength() * sizeof(wchar_t);
		const DWORD numberOfBytesWritten = this->WriteFile(buffer, numberOfBytesToWrite);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
	else
	{
		void* buffer = (void*)KString::ToAnsiString(text);
		const DWORD numberOfBytesToWrite = text.GetLength() * sizeof(char);
		const DWORD numberOfBytesWritten = this->WriteFile(buffer, numberOfBytesToWrite);
		::free(buffer);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
}

KString KFile::ReadAsString(bool isUnicode)
{
	DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize + 2); // +2 is for null
		const DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			buffer[fileSize + 1] = 0; // null for the unicode encoding

			if (isUnicode)
			{
				return KString((const wchar_t*)buffer, KString::FREE_TEXT_WHEN_DONE);
			}
			else
			{
				KString strData((const char*)buffer);
				::free(buffer);
				return strData;
			}
		}

		::free(buffer); // cannot read entire file!
	}

	return KString();
}

bool KFile::DeleteFile(const KString& fileName)
{
	return (::DeleteFileW(fileName) == 0) ? false : true;
}

bool KFile::CopyFile(const KString& sourceFileName, const KString& destFileName)
{
	return (::CopyFileW(sourceFileName, destFileName, FALSE) == 0) ? false : true;
}

KString KFile::GetFileNameFromPath(const KString& path)
{
	const wchar_t* pathStr = (const wchar_t*)path;
	const wchar_t* fileNamePtr = ::PathFindFileNameW(pathStr);

	if (pathStr != fileNamePtr)
		return KString(fileNamePtr);

	return KString();
}

bool KFile::IsFileExists(const KString& fileName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

KFile::~KFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}

// =========== KLogger.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KLogger::KLogger(DWORD bufferSize)
{
	buffer = (char*)malloc(bufferSize);
	this->bufferSize = bufferSize;
	bufferIndex = 0;
	totalEvents = 0;
	totalMills = 0;
	bufferFull = false;
	isFirstCall = true;
}

bool KLogger::WriteNewEvent(unsigned char eventType)
{
	if (!bufferFull)
	{
		if ((bufferIndex + 300) >= bufferSize) // assume each event data is not greater than 300 bytes
		{
			bufferFull = true;
			return false;
		}

		unsigned short secs = 0;
		unsigned short mills = 0;

		if (isFirstCall)
		{
			pCounter.StartCounter();
			isFirstCall = false;
			totalMills = 0;
		}
		else{
			const double deltaMills = pCounter.EndCounter();
			totalMills += (unsigned int)deltaMills;

			secs = (unsigned short)(totalMills/1000);
			mills = (unsigned short)(totalMills % 1000);

			pCounter.StartCounter();
		}

		buffer[bufferIndex] = eventType; // write event type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = secs; // write secs
		bufferIndex += sizeof(unsigned short);

		*((unsigned short*)&buffer[bufferIndex]) = mills; // write mills
		bufferIndex += sizeof(unsigned short);

		totalEvents++;

		return true;
	}
	return false;
}

bool KLogger::EndEvent()
{
	if (!bufferFull)
	{
		buffer[bufferIndex] = EVT_END; // write event end
		bufferIndex += sizeof(unsigned char);

		return true;
	}
	return false;
}

bool KLogger::AddTextParam(const char *text, unsigned char textLength)
{
	if( (textLength < 255) && (!bufferFull) )
	{
		buffer[bufferIndex] = PARAM_STRING; // write param type
		bufferIndex += sizeof(unsigned char);

		buffer[bufferIndex] = textLength; // write data size
		bufferIndex += sizeof(unsigned char);

		for (int i = 0; i < textLength; i++) // write data
		{
			buffer[bufferIndex] = text[i];
			bufferIndex += sizeof(unsigned char);
		}

		return true;
	}
	return false;
}

bool KLogger::AddIntParam(int value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_INT32; // write param type
		bufferIndex += sizeof(unsigned char);

		*((int*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(int);

		return true;
	}
	return false;
}

bool KLogger::AddShortParam(unsigned short value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_SHORT16; // write param type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(unsigned short);

		return true;
	}
	return false;
}

bool KLogger::AddFloatParam(float value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_FLOAT; // write param type
		bufferIndex += sizeof(unsigned char);

		*((float*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(float);

		return true;
	}
	return false;
}
	
bool KLogger::AddDoubleParam(double value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_DOUBLE; // write param type
		bufferIndex += sizeof(unsigned char);

		*((double*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(double);

		return true;
	}
	return false;
}

bool KLogger::IsBufferFull()
{
	return bufferFull;
}

bool KLogger::WriteToFile(const KString &filePath)
{
	KFile file;

	if (KFile::IsFileExists(filePath))
		KFile::DeleteFile(filePath);

	if (file.OpenFile(filePath, KFile::KWRITE))
	{
		file.WriteFile((void*)"RLOG", 4);
		file.WriteFile(&totalEvents, 4);
		file.WriteFile(buffer, bufferIndex);

		return true;
	}

	return false;
}

KLogger::~KLogger()
{
	free(buffer);
}

// =========== KSettingsReader.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KSettingsReader::KSettingsReader()
{

}

bool KSettingsReader::OpenFile(const KString& fileName, int formatID)
{
	if (!KFile::IsFileExists(fileName))
		return false;

	if (!settingsFile.OpenFile(fileName, KFile::KREAD))
		return false;

	settingsFile.SetFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.ReadFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void KSettingsReader::ReadData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.ReadFile(buffer, size);
}

KString KSettingsReader::ReadString()
{
	int size = 0;
	settingsFile.ReadFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.ReadFile(buffer, size);

		return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
	}
	else
	{
		return KString();
	}
}

int KSettingsReader::ReadInt()
{
	int value = 0;
	settingsFile.ReadFile(&value, sizeof(int));

	return value;
}

float KSettingsReader::ReadFloat()
{
	float value = 0;
	settingsFile.ReadFile(&value, sizeof(float));

	return value;
}

double KSettingsReader::ReadDouble()
{
	double value = 0;
	settingsFile.ReadFile(&value, sizeof(double));

	return value;
}

bool KSettingsReader::ReadBool()
{
	bool value = 0;
	settingsFile.ReadFile(&value, sizeof(bool));

	return value;
}

KSettingsReader::~KSettingsReader()
{

}

// =========== KSettingsWriter.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/


KSettingsWriter::KSettingsWriter()
{

}

bool KSettingsWriter::OpenFile(const KString& fileName, int formatID)
{
	if (KFile::IsFileExists(fileName))
		KFile::DeleteFile(fileName);

	if (!settingsFile.OpenFile(fileName, KFile::KWRITE))
		return false;

	settingsFile.SetFilePointerToStart();
	settingsFile.WriteFile(&formatID, sizeof(int));

	return true;
}

void KSettingsWriter::WriteData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.WriteFile(buffer, size);
}

void KSettingsWriter::WriteString(const KString& text)
{
	int size = text.GetLength();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		settingsFile.WriteFile(&size, sizeof(int));

		settingsFile.WriteFile((wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.WriteFile(&size, sizeof(int));
	}
}

void KSettingsWriter::WriteInt(int value)
{
	settingsFile.WriteFile(&value, sizeof(int));
}

void KSettingsWriter::WriteFloat(float value)
{
	settingsFile.WriteFile(&value, sizeof(float));
}

void KSettingsWriter::WriteDouble(double value)
{
	settingsFile.WriteFile(&value, sizeof(double));
}

void KSettingsWriter::WriteBool(bool value)
{
	settingsFile.WriteFile(&value, sizeof(bool));
}

KSettingsWriter::~KSettingsWriter()
{

}

// =========== GUIModule.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#include <commctrl.h>

class RFC_GUIModule 
{
public:
	static bool RFCModuleInit()
	{		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);
		
		KGUIProc::AtomComponent = ::GlobalAddAtomW(L"RFCComponent");
		KGUIProc::AtomOldProc = ::GlobalAddAtomW(L"RFCOldProc");

		return true;
	}

	static void RFCModuleFree()
	{
		::GlobalDeleteAtom(KGUIProc::AtomComponent);
		::GlobalDeleteAtom(KGUIProc::AtomOldProc);

		// delete singletons
		KFont::DeleteDefaultFont();
		delete KIDGenerator::GetInstance();
	}
};

REGISTER_RFC_MODULE(1, RFC_GUIModule)

// =========== KBitmap.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KBitmap::KBitmap()
{
	hBitmap = 0;
}

KBitmap::operator HBITMAP()const
{
	return hBitmap;
}

bool KBitmap::LoadFromResource(WORD resourceID)
{
	hBitmap = (HBITMAP)::LoadImageW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

bool KBitmap::LoadFromFile(const KString& filePath)
{
	hBitmap = (HBITMAP)::LoadImageW(KApplication::hInstance, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

void KBitmap::DrawOnHDC(HDC hdc, int x, int y, int width, int height)
{
	HDC memHDC = ::CreateCompatibleDC(hdc);

	::SelectObject(memHDC, hBitmap);
	::BitBlt(hdc, x, y, width, height, memHDC, 0, 0, SRCCOPY);

	::DeleteDC(memHDC);
}

HBITMAP KBitmap::GetHandle()
{
	return hBitmap;
}

KBitmap::~KBitmap()
{
	if(hBitmap)
		::DeleteObject(hBitmap);
}

// =========== KButton.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KButton::KButton() : KComponent(false)
{
	listener = nullptr;

	compClassName.AssignStaticText(TXT_WITH_LEN("BUTTON"));
	compText.AssignStaticText(TXT_WITH_LEN("Button"));

	compWidth = 100;
	compHeight = 30;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY | WS_TABSTOP;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KButton::SetListener(KButtonListener *listener)
{
	this->listener = listener;
}

KButtonListener* KButton::GetListener()
{
	return listener;
}

void KButton::OnPress()
{
	if(listener)
		listener->OnButtonPress(this);
}

bool KButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == BN_CLICKED))
	{
		this->OnPress();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KButton::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KButton::~KButton()
{
}

// =========== KButtonListener.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/


KButtonListener::KButtonListener(){}

KButtonListener::~KButtonListener(){}

void KButtonListener::OnButtonPress(KButton* button){}

// =========== KCheckBox.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KCheckBox::KCheckBox()
{
	checked = false;
	compText.AssignStaticText(TXT_WITH_LEN("CheckBox"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_NOTIFY | WS_TABSTOP;
}

bool KCheckBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KCheckBox::OnPress()
{
	if(::SendMessageW(compHWND, BM_GETCHECK, 0, 0) == BST_CHECKED)
		checked = true;
	else
		checked = false;

	if(listener)
		listener->OnButtonPress(this);
}

bool KCheckBox::IsChecked()
{
	return checked;
}

void KCheckBox::SetCheckedState(bool state)
{
	checked = state;

	if(compHWND)
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
}

KCheckBox::~KCheckBox()
{
}

// =========== KComboBox.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/


KComboBox::KComboBox(bool sort) : KComponent(false)
{
	listener = nullptr;
	selectedItemIndex = -1;

	compClassName.AssignStaticText(TXT_WITH_LEN("COMBOBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = WS_VSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;

	if(sort)
		compDwStyle = compDwStyle | CBS_SORT;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	stringList = new KPointerList<KString*>(50);
}

void KComboBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KComboBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, CB_DELETESTRING, index, 0);
}

void KComboBox::RemoveItem(const KString& text)
{
	const int itemIndex = this->GetItemIndex(text);
	if(itemIndex > -1)
		this->RemoveItem(itemIndex);
}

int KComboBox::GetItemIndex(const KString& text)
{
	const int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if(stringList->GetPointer(i)->Compare(text))
				return i;
		}
	}
	return -1;
}

int KComboBox::GetItemCount()
{
	return stringList->GetSize();
}

int KComboBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, CB_GETCURSEL, 0, 0);
		if(index != CB_ERR)
			return index;
	}
	return -1;		
}

KString KComboBox::GetSelectedItem()
{
	const int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

void KComboBox::ClearList()
{
	stringList->DeleteAll(true);
	if(compHWND)
		::SendMessageW(compHWND, CB_RESETCONTENT, 0, 0);
}

void KComboBox::SelectItem(int index)
{
	selectedItemIndex = index;
	if(compHWND)
		::SendMessageW(compHWND, CB_SETCURSEL, index, 0);
}

bool KComboBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELENDOK))
	{
		this->OnItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KComboBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register COMBOBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
		}

		if(selectedItemIndex > -1)
			::SendMessageW(compHWND, CB_SETCURSEL, selectedItemIndex, 0);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KComboBox::SetListener(KComboBoxListener *listener)
{
	this->listener = listener;
}

void KComboBox::OnItemSelect()
{
	if(listener)
		listener->OnComboBoxItemSelect(this);
}

KComboBox::~KComboBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}



// =========== KComboBoxListener.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KComboBoxListener::KComboBoxListener(){}

KComboBoxListener::~KComboBoxListener(){}

void KComboBoxListener::OnComboBoxItemSelect(KComboBox* comboBox){}

// =========== KCommonDialogBox.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/



const wchar_t* const RFC_OSD_REG_LOCATION = L"Software\\CrownSoft\\RFC\\OSD";

bool KCommonDialogBox::ShowOpenFileDialog(KWindow* window, 
	const KString& title, 
	const wchar_t* filter, 
	KString* fileName, 
	bool saveLastLocation, 
	const wchar_t* dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!	
	wchar_t *buff = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	buff[0] = 0;
 
	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	KString lastLocation;
	if (saveLastLocation)
	{
		KRegistry::ReadString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.GetLength() > 0)
			ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
	}

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetOpenFileNameW(&ofn))
	{
		KString path(buff, KString::FREE_TEXT_WHEN_DONE);
		*fileName = path;

		if (saveLastLocation)
		{
			KString parentDir(KDirectory::GetParentDir(path).AppendStaticText(L"\\", 1, true));

			KRegistry::CreateKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::WriteString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, parentDir);
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}

bool KCommonDialogBox::ShowSaveFileDialog(KWindow* window, 
	const KString& title, 
	const wchar_t* filter, 
	KString* fileName,
	bool saveLastLocation,
	const wchar_t* dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!
	wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
	buff[0] = 0;

	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	KString lastLocation;
	if (saveLastLocation)
	{		
		KRegistry::ReadString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.GetLength() > 0)
			ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
	}

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetSaveFileNameW(&ofn))
	{
		KString path(buff, KString::FREE_TEXT_WHEN_DONE);
		*fileName = path;

		if (saveLastLocation)
		{
			KString parentDir(KDirectory::GetParentDir(path).AppendStaticText(L"\\", 1, true));

			KRegistry::CreateKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::WriteString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, parentDir);
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}

// =========== KComponent.cpp ===========

/*
	RFC - KComponent.cpp
	Copyright (C) 2013-2019 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/



KComponent::KComponent(bool generateWindowClassDetails)
{
	isRegistered = false;

	KIDGenerator *idGenerator = KIDGenerator::GetInstance();
	compCtlID = idGenerator->GenerateControlID();

	compHWND = 0;
	compParentHWND = 0;
	compDwStyle = 0;
	compDwExStyle = 0;
	cursor = 0;
	compX = 0;
	compY = 0;
	compWidth = CW_USEDEFAULT;
	compHeight = CW_USEDEFAULT;
	compDPI = USER_DEFAULT_SCREEN_DPI;
	compVisible = true;
	compEnabled = true;

	if (generateWindowClassDetails)
	{
		compClassName = idGenerator->GenerateClassName();
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = 0;
		wc.lpszMenuName = 0;
		wc.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIconSm = 0;
		wc.style = 0;
		wc.hInstance = KApplication::hInstance;
		wc.lpszClassName = compClassName;

		wc.lpfnWndProc = KGUIProc::WindowProc;
	}

	compFont = KFont::GetDefaultFont();
}

KComponent::operator HWND()const
{
	return compHWND;
}

void KComponent::OnHotPlug()
{

}

void KComponent::HotPlugInto(HWND component, bool fetchInfo)
{
	compHWND = component;

	if (fetchInfo)
	{
		wchar_t *clsName = (wchar_t*)::malloc(256 * sizeof(wchar_t));  // assume 256 is enough
		clsName[0] = 0;
		::GetClassNameW(compHWND, clsName, 256);
		compClassName = KString(clsName, KString::FREE_TEXT_WHEN_DONE);

		::GetClassInfoExW(KApplication::hInstance, compClassName, &wc);

		compCtlID = (UINT)::GetWindowLongPtrW(compHWND, GWL_ID);

		RECT rect;
		::GetWindowRect(compHWND, &rect);
		compWidth = rect.right - rect.left;
		compHeight = rect.bottom - rect.top;
		compX = rect.left;
		compY = rect.top;

		compVisible = (::IsWindowVisible(compHWND) ? true : false);
		compEnabled = (::IsWindowEnabled(compHWND) ? true : false);

		compDwStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_STYLE);
		compDwExStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_EXSTYLE);

		compParentHWND = ::GetParent(compHWND);

		wchar_t *buff = (wchar_t*)::malloc(256 * sizeof(wchar_t)); // assume 256 is enough
		buff[0] = 0;
		::GetWindowTextW(compHWND, buff, 256);
		compText = KString(buff, KString::FREE_TEXT_WHEN_DONE);
	}

	KGUIProc::AttachRFCPropertiesToHWND(compHWND, (KComponent*)this);	

	this->OnHotPlug();
}

UINT KComponent::GetControlID()
{
	return compCtlID;
}

void KComponent::SetMouseCursor(KCursor *cursor)
{
	this->cursor = cursor;
	if(compHWND)
		::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());
}

KString KComponent::GetComponentClassName()
{
	return compClassName;
}

bool KComponent::Create(bool requireInitialMessages)
{
	if(!::RegisterClassExW(&wc))
		return false;

	isRegistered = true;

	KGUIProc::CreateComponent(this, requireInitialMessages);

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled ? TRUE : FALSE);
		::ShowWindow(compHWND, compVisible ? SW_SHOW : SW_HIDE);

		if(cursor)
			::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());

		return true;
	}
	return false;
}

void KComponent::Destroy()
{
	if (compHWND)
		::DestroyWindow(compHWND);
}

LRESULT KComponent::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomOldProc));
	if(lpfnOldWndProc)
		if((void*)lpfnOldWndProc != (void*)KGUIProc::WindowProc) // it's a subclassed common control or hot-plugged dialog! RFCOldProc property of subclassed control|dialog is not KGUIProc::WindowProc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
	return ::DefWindowProcW(hwnd, msg, wParam, lParam); // custom control or window
}

bool KComponent::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	return false;
}

void KComponent::SetFont(KFont *compFont)
{
	this->compFont = compFont;
	if(compHWND)
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0));
}

KFont* KComponent::GetFont()
{
	return compFont;
}

KString KComponent::GetText()
{
	return compText;
}

void KComponent::SetText(const KString& compText)
{
	this->compText = compText;
	if(compHWND)
		::SetWindowTextW(compHWND, this->compText);
}

void KComponent::SetHWND(HWND compHWND)
{
	this->compHWND = compHWND;
}

HWND KComponent::GetHWND()
{
	return compHWND;
}

void KComponent::SetParentHWND(HWND compParentHWND)
{
	this->compParentHWND = compParentHWND;
	if(compHWND)
		::SetParent(compHWND, compParentHWND);
}

HWND KComponent::GetParentHWND()
{
	return compParentHWND;
}

DWORD KComponent::GetStyle()
{
	return compDwStyle;
}

void KComponent::SetStyle(DWORD compStyle)
{
	this->compDwStyle = compStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_STYLE, compStyle);
}

DWORD KComponent::GetExStyle()
{
	return compDwExStyle;
}

void KComponent::SetExStyle(DWORD compDwExStyle)
{
	this->compDwExStyle = compDwExStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_EXSTYLE, compDwExStyle);
}

int KComponent::GetDPI()
{
	return compDPI;
}

int KComponent::GetX()
{
	return compX; 
}

int KComponent::GetY()
{
	return compY;
}

int KComponent::GetWidth()
{
	return compWidth;
}

int KComponent::GetHeight()
{
	return compHeight;
}

void KComponent::SetDPI(int newDPI)
{
	if (newDPI == compDPI)
		return;

	const int oldDPI = compDPI;
	compDPI = newDPI;

	if (compDwStyle & WS_CHILD) // do not change position and font size of top level windows.
	{
		this->compX = ::MulDiv(compX, newDPI, oldDPI);
		this->compY = ::MulDiv(compY, newDPI, oldDPI);

		if (!compFont->IsDefaultFont())
			compFont->SetDPI(newDPI);
	}

	this->compWidth = ::MulDiv(compWidth, newDPI, oldDPI);
	this->compHeight = ::MulDiv(compHeight, newDPI, oldDPI);

	if (compHWND)
	{
		::SetWindowPos(compHWND, 0, compX, compY, compWidth, compHeight, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
		if((!compFont->IsDefaultFont()) && (compDwStyle & WS_CHILD))
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0));
	}
}

void KComponent::SetSize(int compWidth, int compHeight)
{
	this->compWidth = compWidth;
	this->compHeight = compHeight;

	if(compHWND)
		::SetWindowPos(compHWND, 0, 0, 0, compWidth, compHeight, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::SetPosition(int compX, int compY)
{
	this->compX = compX;
	this->compY = compY;

	if(compHWND)
		::SetWindowPos(compHWND, 0, compX, compY, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::SetVisible(bool state)
{
	compVisible = state;
	if(compHWND)
		::ShowWindow(compHWND, state ? SW_SHOW : SW_HIDE);
}

bool KComponent::IsVisible()
{
	if (compHWND)
	{
		compVisible = (::IsWindowVisible(compHWND) == TRUE);
		return compVisible;
	}

	return false;
}

bool KComponent::IsEnabled()
{
	if (compHWND)
		compEnabled = (::IsWindowEnabled(compHWND) == TRUE);

	return compEnabled;
}

void KComponent::SetEnabled(bool state)
{
	compEnabled = state;

	if(compHWND)
		::EnableWindow(compHWND, compEnabled);
}

void KComponent::BringToFront()
{
	if(compHWND)
		::BringWindowToTop(compHWND);
}

void KComponent::SetKeyboardFocus()
{
	if(compHWND)
		::SetFocus(compHWND);
}

void KComponent::Repaint()
{
	if (compHWND)
	{
		::InvalidateRect(compHWND, NULL, TRUE);
		::UpdateWindow(compHWND); // instant update
	}
}

KComponent::~KComponent()
{
	if(isRegistered)
		::UnregisterClassW(compClassName, KApplication::hInstance);
}

// =========== KCursor.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KCursor::KCursor()
{
	hCursor = 0;
}

bool KCursor::LoadFromResource(WORD resourceID)
{
	hCursor = (HCURSOR)::LoadImageW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

bool KCursor::LoadFromFile(const KString& filePath)
{
	hCursor = (HCURSOR)::LoadImageW(KApplication::hInstance, filePath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

HCURSOR KCursor::GetHandle()
{
	return hCursor;
}

KCursor::operator HCURSOR()const
{
	return hCursor;
}

KCursor::~KCursor()
{
	if(hCursor)
		::DestroyCursor(hCursor);
}

// =========== KFont.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KFont* KFont::defaultInstance=0;

KFont::KFont()
{
	hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	customFont = false;
}

KFont::KFont(const KString& face, int sizeFor96DPI, bool bold, bool italic, bool underline, bool antiAliased, int requiredDPI)
{
	this->fontFace = face;
	this->fontSizeFor96DPI = sizeFor96DPI;
	this->isBold = bold;
	this->isItalic = italic;
	this->isUnderline = underline;
	this->isAntiAliased = antiAliased;
	this->currentDPI = requiredDPI;

	hFont = ::CreateFontW(fontSizeFor96DPI * requiredDPI / USER_DEFAULT_SCREEN_DPI, 
		0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, underline ? TRUE : FALSE, 0, DEFAULT_CHARSET,
		0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, face);

	if(hFont)
		customFont = true;
}

void KFont::SetDPI(int newDPI)
{
	if( customFont && (currentDPI != newDPI) )
	{
		::DeleteObject(hFont);
		hFont = ::CreateFontW(fontSizeFor96DPI * newDPI / USER_DEFAULT_SCREEN_DPI, 0, 0, 0, isBold ? FW_BOLD : FW_NORMAL,
			isItalic ? TRUE : FALSE, isUnderline ? TRUE : FALSE, 0, DEFAULT_CHARSET, 0, 0,
			isAntiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, 
			fontFace);

		currentDPI = newDPI;
	}
}

KFont* KFont::GetDefaultFont()
{
	if(KFont::defaultInstance == 0)
		KFont::defaultInstance = new KFont();

	return KFont::defaultInstance;
}

void KFont::DeleteDefaultFont()
{
	if (KFont::defaultInstance)
	{
		delete KFont::defaultInstance;
		KFont::defaultInstance = 0;
	}
}

bool KFont::IsDefaultFont()
{
	return !customFont;
}

bool KFont::LoadFont(const KString& path)
{
	return (::AddFontResourceExW(path, FR_PRIVATE, 0) == 0) ? false : true;
}

void KFont::RemoveFont(const KString& path)
{
	::RemoveFontResourceExW(path, FR_PRIVATE, 0);
}

HFONT KFont::GetFontHandle()
{
	return hFont;
}

KFont::operator HFONT()const
{
	return hFont;
}

KFont::~KFont()
{
	if(customFont)
		::DeleteObject(hFont);
}

// =========== KGlyphButton.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/

#include <commctrl.h>

KGlyphButton::KGlyphButton()
{
	glyphFont = nullptr;
	glyphChar = nullptr;
	glyphLeft = 6;
}

KGlyphButton::~KGlyphButton()
{
}

void KGlyphButton::SetGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->Repaint();
}

void KGlyphButton::SetDPI(int newDPI)
{
	if (glyphFont)
		glyphFont->SetDPI(newDPI);

	KButton::SetDPI(newDPI);
}

bool KGlyphButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if (glyphFont)
	{
		if (msg == WM_NOTIFY)
		{
			if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
			{
				LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW)lParam;

				*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

				if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
				{
					*result = CDRF_NOTIFYPOSTPAINT;
				}
				else if (CDDS_POSTPAINT == lpNMCD->dwDrawStage) //  postpaint stage
				{
					const RECT rc = lpNMCD->rc;
					const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED | CDIS_GRAYED)) != 0;

					HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, glyphFont->GetFontHandle());
					const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);
					const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

					RECT rcIcon = { rc.left + ::MulDiv(glyphLeft, compDPI, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw glyph

					::SetBkMode(lpNMCD->hdc, oldBkMode);
					::SetTextColor(lpNMCD->hdc, oldTextColor);
					::SelectObject(lpNMCD->hdc, oldFont);

					*result = CDRF_DODEFAULT;
				}

				return true; // indicate that we processed this msg & result is valid.
			}
		}
	}

	return KButton::EventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KGraphics.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KGraphics::KGraphics(){}

KGraphics::~KGraphics(){}

void KGraphics::Draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	KGraphics::Draw3dRect(hdc, startX, startY, 2, 
		height, ::GetSysColor(COLOR_BTNSHADOW), 
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	KGraphics::Draw3dRect(hdc, startX, startY, width, 
		2, ::GetSysColor(COLOR_BTNSHADOW), 
		::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::Draw3dRect(hdc, lpRect->left, lpRect->top, 
		lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 
		clrTopLeft, clrBottomRight);
}

void KGraphics::Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	KGraphics::FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void KGraphics::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	KGraphics::FillSolidRect(hdc, &rect, color);
}

void KGraphics::FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	const COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}

RECT KGraphics::CalculateTextSize(const wchar_t* text, HFONT hFont)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = {0, 0, 0, 0};

	::DrawTextW(hDC, text, ::lstrlenW(text), &sz, DT_CALCRECT | DT_NOPREFIX);
	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz;
}

int KGraphics::CalculateTextHeight(const wchar_t* text, HFONT hFont, int width)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = { 0, 0, width, 0 };

	::DrawTextW(hDC, text, ::lstrlenW(text), &sz, 
		DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT);

	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz.bottom;
}

// https://devblogs.microsoft.com/oldnewthing/20210915-00/?p=105687
void KGraphics::MakeBitmapOpaque(HDC hdc, int x, int y, int cx, int cy)
{
	BITMAPINFO bi = {};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = 1;
	bi.bmiHeader.biHeight = 1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD bitmapBits = { 0x00, 0x00, 0x00, 0xFF };

	::StretchDIBits(hdc, x, y, cx, cy,
		0, 0, 1, 1, &bitmapBits, &bi,
		DIB_RGB_COLORS, SRCPAINT);
}

void KGraphics::SetBitmapAlphaChannel(HDC hdc, int x, int y, int cx, int cy, BYTE alpha)
{
	BITMAPINFO bi = {};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = 1;
	bi.bmiHeader.biHeight = 1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	if (alpha != 255) {
		RGBQUAD zeroAlpha = { 0xFF, 0xFF, 0xFF, 0x00 };
		::StretchDIBits(hdc, x, y, cx, cy,
			0, 0, 1, 1, &zeroAlpha, &bi,
			DIB_RGB_COLORS, SRCAND);
	}

	RGBQUAD alphaOnly = { 0x00, 0x00, 0x00, alpha };
	::StretchDIBits(hdc, x, y, cx, cy,
		0, 0, 1, 1, &alphaOnly, &bi,
		DIB_RGB_COLORS, SRCPAINT);
}

// =========== KGridView.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#include <commctrl.h>

KGridView::KGridView(bool sortItems) : KComponent(false)
{
	itemCount = 0;
	colCount = 0;
	listener = nullptr;

	compClassName.AssignStaticText(TXT_WITH_LEN("SysListView32"));

	compWidth = 300;
	compHeight = 200;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_TABSTOP | WS_BORDER | 
		LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	compDwExStyle = WS_EX_WINDOWEDGE;

	if (sortItems)
		compDwStyle |= LVS_SORTASCENDING;
}

KGridView::~KGridView(){}

void KGridView::SetListener(KGridViewListener* listener)
{
	this->listener = listener;
}

KGridViewListener* KGridView::GetListener()
{
	return listener;
}

void KGridView::InsertRecord(KString** columnsData)
{
	LVITEMW lvi = {};
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (*columnsData[0]);
	lvi.iItem = itemCount;

	const int row = (int)::SendMessageW(compHWND, 
		LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem = {};
		lvItem.iSubItem = i;
		lvItem.pszText = (*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
			(WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

void KGridView::InsertRecordTo(int rowIndex, KString **columnsData)
{
	LVITEMW lvi = {};
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (*columnsData[0]);
	lvi.iItem = rowIndex;

	const int row = (int)::SendMessageW(compHWND, 
		LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem= {};
		lvItem.iSubItem = i;
		lvItem.pszText = (*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
			(WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

KString KGridView::GetRecordAt(int rowIndex, int columnIndex)
{
	wchar_t *buffer = (wchar_t*)::malloc(512 * sizeof(wchar_t));
	buffer[0] = 0;

	LV_ITEMW lvi = {};
	lvi.iSubItem = columnIndex;
	lvi.cchTextMax = 512;
	lvi.pszText = buffer;

	::SendMessageW(compHWND, LVM_GETITEMTEXTW, 
		(WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_GetItemText macro. it relies on preprocessor defs.

	return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
}

int KGridView::GetSelectedRow()
{
	return ListView_GetNextItem(compHWND, -1, LVNI_SELECTED);
}

void KGridView::RemoveRecordAt(int rowIndex)
{
	if (ListView_DeleteItem(compHWND, rowIndex))
		--itemCount;
}

void KGridView::RemoveAll()
{
	ListView_DeleteAllItems(compHWND);
	itemCount = 0;
}

void KGridView::UpdateRecordAt(int rowIndex, int columnIndex, const KString& text)
{
	LV_ITEMW lvi = {};
	lvi.iSubItem = columnIndex;
	lvi.pszText = text;

	::SendMessageW(compHWND, LVM_SETITEMTEXTW, 
		(WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_SetItemText macro. it relies on preprocessor defs.
}

void KGridView::SetColumnWidth(int columnIndex, int columnWidth)
{
	ListView_SetColumnWidth(compHWND, columnIndex, columnWidth);
}

int KGridView::GetColumnWidth(int columnIndex)
{
	return ListView_GetColumnWidth(compHWND, columnIndex);
}

void KGridView::CreateColumn(const KString& text, int columnWidth)
{
	LVCOLUMNW lvc = {};

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = columnWidth;
	lvc.pszText = text;
	lvc.iSubItem = colCount;

	::SendMessageW(compHWND, LVM_INSERTCOLUMNW, 
		(WPARAM)colCount, (LPARAM)&lvc);

	++colCount;
}

bool KGridView::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_NOTIFY)
	{
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) // List view item selection changed (mouse or keyboard)
		{
			LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)lParam;
			if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
			{
				this->OnItemSelect();
				*result = 0;
				return true;
			}
		}
		else if (((LPNMHDR)lParam)->code == NM_RCLICK) // List view item right click
		{
			this->OnItemRightClick();
			*result = 0;
			return true;
		}
		else if (((LPNMHDR)lParam)->code == NM_DBLCLK) // List view item double click
		{
			this->OnItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KGridView::Create(bool requireInitialMessages)
{
	if (!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register WC_LISTVIEWW class!

	if (compHWND)
	{
		ListView_SetExtendedListViewStyle(compHWND, 
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KGridView::OnItemSelect()
{
	if (listener)
		listener->OnGridViewItemSelect(this);
}

void KGridView::OnItemRightClick()
{
	if (listener)
		listener->OnGridViewItemRightClick(this);
}

void KGridView::OnItemDoubleClick()
{
	if (listener)
		listener->OnGridViewItemDoubleClick(this);
}


// =========== KGridViewListener.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KGridViewListener::KGridViewListener(){}

KGridViewListener::~KGridViewListener(){}

void KGridViewListener::OnGridViewItemSelect(KGridView* gridView){}

void KGridViewListener::OnGridViewItemRightClick(KGridView* gridView){}
 
void KGridViewListener::OnGridViewItemDoubleClick(KGridView* gridView){}

// =========== KGroupBox.cpp ===========

/*
	Copyright (C) 2013-2023 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/


KGroupBox::KGroupBox()
{
	compText.AssignStaticText(TXT_WITH_LEN("GroupBox"));
	compWidth = 100;
	compHeight = 100;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_GROUPBOX;
	compDwExStyle = WS_EX_WINDOWEDGE;
}


KGroupBox::~KGroupBox()
{
}

// =========== KGUIProc.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


class RFCInternalVariables
{
public:
	static KComponent* currentComponent;
	static HHOOK wnd_hook;
};

KComponent* RFCInternalVariables::currentComponent = nullptr;
HHOOK RFCInternalVariables::wnd_hook = 0;

ATOM KGUIProc::AtomComponent;
ATOM KGUIProc::AtomOldProc;

LRESULT CALLBACK RFCCTL_CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(RFCInternalVariables::wnd_hook, nCode, wParam, lParam);

	if (nCode == HCBT_CREATEWND) {
		HWND hwnd = (HWND)wParam;
		LPCBT_CREATEWNDW cbtCreateWnd = (LPCBT_CREATEWNDW)lParam;
		if (cbtCreateWnd)
		{
			if (cbtCreateWnd->lpcs)
			{
				if (cbtCreateWnd->lpcs->lpCreateParams == RFCInternalVariables::currentComponent) // only catch what we created. ignore unknown windows.
				{
					KGUIProc::AttachRFCPropertiesToHWND(hwnd, RFCInternalVariables::currentComponent);

					// Call the next hook, if there is one
					const LRESULT result = ::CallNextHookEx(RFCInternalVariables::wnd_hook, 
						nCode, wParam, lParam);

					// we subclassed what we created. so remove the hook.
					::UnhookWindowsHookEx(RFCInternalVariables::wnd_hook); // unhooking at here will allow child creation at WM_CREATE. otherwise this will hook child also!

					return result;
				}
			}
		}
	}

	// Call the next hook, if there is one
	return ::CallNextHookEx(RFCInternalVariables::wnd_hook, nCode, wParam, lParam);
}

void KGUIProc::AttachRFCPropertiesToHWND(HWND hwnd, KComponent* component)
{
	::SetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomComponent), (HANDLE)component);

	FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	::SetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomOldProc), (HANDLE)lpfnOldWndProc);

	if (lpfnOldWndProc != (void*)KGUIProc::WindowProc) // sublcass only if window proc is not KGUIProc::WindowProc (common control or dialog)
		::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)KGUIProc::WindowProc); // subclassing...
}

LRESULT CALLBACK KGUIProc::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	KComponent* component = (KComponent*)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomComponent));

	if (!component) // for safe!
		return ::DefWindowProcW(hwnd, msg, wParam, lParam);

	if (!component->GetHWND()) // window recieve msg for the first time!
		component->SetHWND(hwnd);

	if (msg == WM_NCDESTROY) {
		::RemovePropW(hwnd, MAKEINTATOM(KGUIProc::AtomComponent));

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomOldProc));
		::RemovePropW(hwnd, MAKEINTATOM(KGUIProc::AtomOldProc));

		if (lpfnOldWndProc)
		{
			if (lpfnOldWndProc != (void*)KGUIProc::WindowProc) // common control or dialog
			{
				::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
				return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
			}
		}
		// lpfnOldWndProc is not set or window or custom control
		return ::DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	return component->WindowProc(hwnd, msg, wParam, lParam);
}

INT_PTR CALLBACK KGUIProc::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		KComponent* comp = (KComponent*)lParam;
		if (comp)
			comp->HotPlugInto(hwndDlg, true);

		return FALSE;
	}
	return FALSE;
}

HWND KGUIProc::CreateComponent(KComponent* component, bool requireInitialMessages)
{
	if (requireInitialMessages)
	{
		RFCInternalVariables::currentComponent = component;

		// install hook to get called before WM_NCCREATE msg! (we can receive WM_NCCREATE msg even for a standard control using this technique)
		RFCInternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, ::GetCurrentThreadId());

		// pass current component as lpParam. so CBT proc can ignore other unknown windows.
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(),
			component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(),
			component->GetParentHWND(), (HMENU)(UINT_PTR)component->GetControlID(), KApplication::hInstance, (LPVOID)component);

		// unhook at here will cause catching childs which are created at WM_CREATE. so, unhook at CBT proc.
		//::UnhookWindowsHookEx(RFCInternalVariables::wnd_hook);

		return hwnd;
	}
	else
	{
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(),
			component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(),
			component->GetParentHWND(), (HMENU)(UINT_PTR)component->GetControlID(), KApplication::hInstance, 0);

		KGUIProc::AttachRFCPropertiesToHWND(hwnd, component);
		component->SetHWND(hwnd);

		return hwnd;
	}
}

int KGUIProc::HotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return (int)::DialogBoxParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, KGUIProc::DialogProc, (LPARAM)component);
}

HWND KGUIProc::HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return ::CreateDialogParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, KGUIProc::DialogProc, (LPARAM)component);
}

// =========== KHostPanel.cpp ===========

/*
	Copyright (C) 2013-2023 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/



KHostPanel::KHostPanel() : KComponent(true)
{
	compDwStyle = WS_CHILD | WS_CLIPCHILDREN;
	compDwExStyle = compDwExStyle | WS_EX_CONTROLPARENT; // non-top-level windows that contain child controls/dialogs must have the WS_EX_CONTROLPARENT style.
	// otherwise you will get infinite loop when calling IsDialogMessage.

	compText.AssignStaticText(TXT_WITH_LEN("KHostPanel"));
	enableDPIUnawareMode = false;
	componentList = nullptr;
}

void KHostPanel::SetComponentList(KPointerList<KComponent*>* componentList)
{
	this->componentList = componentList;
}

void KHostPanel::SetEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

bool KHostPanel::AddComponent(KComponent* component, bool requireInitialMessages)
{
	if (component)
	{
		if ((compHWND != 0) && (componentList != nullptr))
		{
			component->SetParentHWND(compHWND);

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
				component->SetDPI(compDPI);

			componentList->AddPointer(component);

			return component->Create(requireInitialMessages);
		}
	}
	return false;
}

void KHostPanel::RemoveComponent(KComponent* component)
{
	if (componentList == nullptr)
		return;

	int index = componentList->GetID(component);
	if (index != -1)
	{
		componentList->RemovePointer(index);
		component->Destroy();
	}
}

bool KHostPanel::AddContainer(KHostPanel* container, bool requireInitialMessages)
{
	if (container)
	{
		container->SetComponentList(componentList);
		container->SetEnableDPIUnawareMode(enableDPIUnawareMode);
		return this->AddComponent(static_cast<KComponent*>(container), requireInitialMessages);
	}
	return false;
}

LRESULT KHostPanel::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
		{
			if (wParam != 0) // ignore menus
			{
				KComponent* component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, 
					MAKEINTATOM(KGUIProc::AtomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
		}
		return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
		{
			KComponent* component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, 
				MAKEINTATOM(KGUIProc::AtomComponent));

			if (component)
			{
				LRESULT result = 0; // just for safe
				if (component->EventProc(msg, wParam, lParam, &result))
					return result;
			}
		}
		return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL:
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
		{
			KComponent* component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(KGUIProc::AtomComponent));
			if (component)
			{
				LRESULT result = 0; // just for safe
				if (component->EventProc(msg, wParam, lParam, &result))
					return result;
			}
		}
		return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
		{
			if (wParam != 0) // ignore menus
			{
				KComponent* component = (KComponent*)::GetPropW(GetDlgItem(hwnd, 
					((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(KGUIProc::AtomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
		}
		return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
		{
			KComponent* component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, 
				MAKEINTATOM(KGUIProc::AtomComponent));

			if (component)
			{
				LRESULT result = 0; // just for safe
				if (component->EventProc(msg, wParam, lParam, &result))
					return result;
			}
		}
		return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
		{
			KTimer* timer = KIDGenerator::GetInstance()->GetTimerByID((UINT)wParam);
			if (timer)
			{
				timer->OnTimer();
				break;
			}
		}
		return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
		{
			if ((HIWORD(wParam) == 0) && (lParam == 0)) // its menu item! unfortunately windows does not send menu handle with clicked event!
			{
				KMenuItem* menuItem = KIDGenerator::GetInstance()->GetMenuItemByID(LOWORD(wParam));
				if (menuItem)
				{
					menuItem->OnPress();
					break;
				}
			}
			else if (lParam)// send to appropriate component
			{
				KComponent* component = (KComponent*)::GetPropW((HWND)lParam, 
					MAKEINTATOM(KGUIProc::AtomComponent));

				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			else if (LOWORD(wParam) == IDOK) // enter key pressed. (lParam does not contain current comp hwnd)
			{
				HWND currentComponent = ::GetFocus();

				// simulate enter key pressed event into current component. (might be a window)
				::SendMessageW(currentComponent, WM_KEYDOWN, VK_RETURN, 0);
				::SendMessageW(currentComponent, WM_KEYUP, VK_RETURN, 0);
				::SendMessageW(currentComponent, WM_CHAR, VK_RETURN, 0);

				return 0;
			}
			else if (LOWORD(wParam) == IDCANCEL) // Esc key pressed. (lParam does not contain current comp hwnd)
			{
				HWND currentComponent = ::GetFocus();

				// simulate esc key pressed event into current component. (might be a window)
				::SendMessageW(currentComponent, WM_KEYDOWN, VK_ESCAPE, 0);
				::SendMessageW(currentComponent, WM_KEYUP, VK_ESCAPE, 0);
				::SendMessageW(currentComponent, WM_CHAR, VK_ESCAPE, 0);

				return 0;
			}
		}
		return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		default:
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

KHostPanel::~KHostPanel() {}

// =========== KIcon.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#include <commctrl.h>

KIcon::KIcon()
{
	resourceID = 0;
	hIcon = 0;
}

void KIcon::SetResource(WORD resourceID)
{
	this->resourceID = resourceID;
}

HICON KIcon::GetScaledIcon(int size)
{
	HICON icon = 0;
	::LoadIconWithScaleDown(KApplication::hInstance,
		MAKEINTRESOURCEW(resourceID), size, size,
		&icon);

	return icon;
}

bool KIcon::LoadFromResource(WORD resourceID)
{
	this->resourceID = resourceID;

	hIcon = (HICON)::LoadImageW(KApplication::hInstance, 
		MAKEINTRESOURCEW(resourceID), IMAGE_ICON, 0, 0, 
		LR_DEFAULTSIZE | LR_DEFAULTCOLOR);

	if(hIcon)
		return true;	
	return false;
}

bool KIcon::LoadFromFile(const KString& filePath)
{
	hIcon = (HICON)::LoadImageW(KApplication::hInstance, 
		filePath, IMAGE_ICON, 0, 0, 
		LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);

	if(hIcon)
		return true;	
	return false;
}

HICON KIcon::GetHandle()
{
	return hIcon;
}

KIcon::operator HICON()const
{
	return hIcon;
}

KIcon::~KIcon()
{
	if(hIcon)
		::DestroyIcon(hIcon);
}

// =========== KIDGenerator.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


static const int rfc_InitialMenuItemCount	= 400;
static const int rfc_InitialTimerCount		= 40;
static const int rfc_InitialControlID		= 100;
static const int rfc_InitialMenuItemID		= 30000;
static const int rfc_InitialTimerID			= 1000;

KIDGenerator* KIDGenerator::_instance = 0;

KIDGenerator::KIDGenerator()
{
	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	menuItemList = nullptr;
	timerList = nullptr;
}

KIDGenerator* KIDGenerator::GetInstance()
{
	if(_instance)
		return _instance;
	_instance = new KIDGenerator();
	return _instance;
}

UINT KIDGenerator::GenerateControlID()
{
	++controlCount;
	return controlCount + rfc_InitialControlID;
}

UINT KIDGenerator::GenerateMenuItemID(KMenuItem* menuItem)
{
	if (menuItemList == nullptr) // generate on first call
		menuItemList = new KPointerList<KMenuItem*>(rfc_InitialMenuItemCount);

	++menuItemCount;
	menuItemList->AddPointer(menuItem);

	return menuItemCount + rfc_InitialMenuItemID;
}

KMenuItem* KIDGenerator::GetMenuItemByID(UINT id)
{
	if (menuItemList)
		return menuItemList->GetPointer(id - (rfc_InitialMenuItemID + 1));
	return 0;
}

KString KIDGenerator::GenerateClassName()
{ 
	wchar_t *className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'R';
	className[1] = L'F';
	className[2] = L'C';
	className[3] = L'_';

	int hInstance32 = HandleToLong(KApplication::hInstance);
	if (hInstance32 < 0)
		hInstance32 *= -1;

	::_itow_s(hInstance32, &className[4],28, 10); // 28 is buffer size

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow_s(classCount, &className[lastPos + 1],32 - (lastPos + 1), 10);

	/*
	#ifdef _MSC_VER
		::swprintf(className, 32, L"RFC_%d_%d", (int)hInstance, classCount);
	#else
		::swprintf(className,L"RFC_%d_%d", (int)hInstance, classCount);
	#endif */

	++classCount;

	return KString(className, KString::FREE_TEXT_WHEN_DONE);
}

UINT KIDGenerator::GenerateTimerID(KTimer* timer)
{
	if (timerList == nullptr) // generate on first call
		timerList = new KPointerList<KTimer*>(rfc_InitialTimerCount);

	++timerCount;
	timerList->AddPointer(timer);

	return timerCount + rfc_InitialTimerID;
}

KTimer* KIDGenerator::GetTimerByID(UINT id)
{
	if (timerList)
		return timerList->GetPointer(id - (rfc_InitialTimerID + 1));
	return 0;
}

KIDGenerator::~KIDGenerator()
{
	if (menuItemList)
		delete menuItemList;

	if (timerList)
		delete timerList;
}

// =========== KLabel.cpp ===========

/*
	Copyright (C) 2013-2023 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/


KLabel::KLabel() : KComponent(false)
{
	compClassName.AssignStaticText(TXT_WITH_LEN("STATIC"));
	compText.AssignStaticText(TXT_WITH_LEN("Label"));

	compWidth = 100;
	compHeight = 25;

	compX = 0;
	compY = 0;

	autoResize = false;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KLabel::ResizeToTextSize()
{
	if (compText.IsNotEmpty())
	{
		RECT rect = KGraphics::CalculateTextSize(compText, compFont->GetFontHandle());
		this->SetSize(rect.right + AUTOSIZE_EXTRA_GAP, rect.bottom);
	}
	else // text is empty
	{
		this->SetSize(20, 25);
	}
}

void KLabel::EnableAutoResize(bool enable)
{
	autoResize = enable;

	if(autoResize)
		this->ResizeToTextSize();
}

void KLabel::SetText(const KString& compText)
{
	KComponent::SetText(compText);

	if (autoResize)
		this->ResizeToTextSize();
}

void KLabel::SetFont(KFont* compFont)
{
	KComponent::SetFont(compFont);

	if (autoResize)
		this->ResizeToTextSize();
}

void KLabel::SetDPI(int newDPI)
{
	if (newDPI == compDPI)
		return;

	int oldDPI = compDPI;
	compDPI = newDPI;

	this->compX = ::MulDiv(compX, newDPI, oldDPI);
	this->compY = ::MulDiv(compY, newDPI, oldDPI);

	if (!compFont->IsDefaultFont())
		compFont->SetDPI(newDPI);
	
	if (compText.IsNotEmpty() && autoResize)
	{
		RECT rect = KGraphics::CalculateTextSize(compText, compFont->GetFontHandle());
		this->compWidth = rect.right + AUTOSIZE_EXTRA_GAP;
		this->compHeight = rect.bottom;
	}
	else
	{
		this->compWidth = ::MulDiv(compWidth, newDPI, oldDPI);
		this->compHeight = ::MulDiv(compHeight, newDPI, oldDPI);
	}

	if (compHWND)
	{
		::SetWindowPos(compHWND, 0, compX, compY, compWidth, 
			compHeight, SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);

		if ((!compFont->IsDefaultFont()) && (compDwStyle & WS_CHILD))
			::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0));
	}
}

bool KLabel::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register Label class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KLabel::~KLabel()
{
}

// =========== KListBox.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KListBox::KListBox(bool multipleSelection, bool sort, bool vscroll) : KComponent(false)
{
	this->multipleSelection = multipleSelection;
	listener = nullptr;

	selectedItemIndex = -1;
	selectedItemEnd = -1;

	compClassName.AssignStaticText(TXT_WITH_LEN("LISTBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = LBS_NOTIFY | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;
	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	if(multipleSelection)
		compDwStyle = compDwStyle | LBS_MULTIPLESEL;
	if(sort)
		compDwStyle = compDwStyle | LBS_SORT;
	if(vscroll)
		compDwStyle = compDwStyle | WS_VSCROLL;

	stringList = new KPointerList<KString*>(100);
}

void KListBox::SetListener(KListBoxListener* listener)
{
	this->listener = listener;
}

void KListBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KListBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
}

void KListBox::RemoveItem(const KString& text)
{
	const int itemIndex = this->GetItemIndex(text);
	if(itemIndex > -1)
		this->RemoveItem(itemIndex);
}

int KListBox::GetItemIndex(const KString& text)
{
	const int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if (stringList->GetPointer(i)->Compare(text))
				return i;
		}
	}
	return -1;
}

int KListBox::GetItemCount()
{
	return stringList->GetSize();
}

int KListBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
	}
	return -1;	
}

KString KListBox::GetSelectedItem()
{
	const int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

int KListBox::GetSelectedItems(int* itemArray, int itemCountInArray)
{
	if(compHWND)
	{	 
		const int items = (int)::SendMessageW(compHWND, 
			LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);

		if(items != LB_ERR)
			return items;
	}
	return -1;
}

void KListBox::ClearList()
{
	stringList->DeleteAll(true);

	if(compHWND)
		::SendMessageW(compHWND, LB_RESETCONTENT, 0, 0);
}

void KListBox::SelectItem(int index)
{
	selectedItemIndex = index;

	if(compHWND)
		::SendMessageW(compHWND, LB_SETCURSEL, index, 0);
}

void KListBox::SelectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(compHWND)
			::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool KListBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if (msg == WM_COMMAND) 
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) // listbox sel change!
		{
			this->OnItemSelect();
			*result = 0;
			return true;
		}
		else if (HIWORD(wParam) == LBN_DBLCLK) // listbox double click
		{
			this->OnItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KListBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register LISTBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, 
			(WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
		}

		if(!multipleSelection) // single selction!
		{
			if(selectedItemIndex > -1)
				::SendMessageW(compHWND, LB_SETCURSEL, selectedItemIndex, 0);
		}else
		{
			if(selectedItemIndex>-1)
				::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(selectedItemIndex, selectedItemEnd));
		}

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KListBox::OnItemSelect()
{
	if(listener)
		listener->OnListBoxItemSelect(this);
}

void KListBox::OnItemDoubleClick()
{
	if(listener)
		listener->OnListBoxItemDoubleClick(this);
}

KListBox::~KListBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}

// =========== KListBoxListener.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KListBoxListener::KListBoxListener(){}
KListBoxListener::~KListBoxListener(){}

void KListBoxListener::OnListBoxItemSelect(KListBox* listBox){}

void KListBoxListener::OnListBoxItemDoubleClick(KListBox* listBox){}

// =========== KMenu.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KMenu::KMenu()
{
	hMenu = ::CreatePopupMenu();
}

void KMenu::AddMenuItem(KMenuItem* menuItem)
{
	menuItem->AddToMenu(hMenu);
}

void KMenu::AddSubMenu(const KString& text, KMenu* menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, 
		(UINT_PTR)menu->GetMenuHandle(), text);
}

void KMenu::AddSeperator()
{
	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_SEPARATOR;

	::InsertMenuItemW(hMenu, 0xFFFFFFFF, FALSE, &mii);
}

HMENU KMenu::GetMenuHandle()
{
	return hMenu;
}

void KMenu::PopUpMenu(HWND window, bool bringWindowToForeground)
{
	if (bringWindowToForeground)
		::SetForegroundWindow(window);

	POINT p;
	::GetCursorPos(&p);
	::TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, window, NULL);
}

KMenu::~KMenu()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuBar.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KMenuBar::KMenuBar()
{
	hMenu = ::CreateMenu();
}

void KMenuBar::AddMenu(const KString& text, KMenu* menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, 
		(UINT_PTR)menu->GetMenuHandle(), text);
}

void KMenuBar::AddToWindow(KWindow* window)
{
	HWND hwnd = window->GetHWND();
	if(hwnd)
		::SetMenu(hwnd, hMenu);
}

KMenuBar::~KMenuBar()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuButton.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#include <commctrl.h>

KMenuButton::KMenuButton()
{
	buttonMenu = nullptr;
	glyphFont = nullptr;
	glyphChar = nullptr;
	glyphLeft = 6;
	arrowFont = new KFont(CONST_TXT("Webdings"), 18, false, false, false, false, USER_DEFAULT_SCREEN_DPI);
}	

KMenuButton::~KMenuButton()
{
	delete arrowFont;
}

void KMenuButton::SetMenu(KMenu* buttonMenu)
{
	this->buttonMenu = buttonMenu;
}

void KMenuButton::SetGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->Repaint();
}

void KMenuButton::SetDPI(int newDPI)
{
	if (glyphFont)
		glyphFont->SetDPI(newDPI);

	arrowFont->SetDPI(newDPI);

	KButton::SetDPI(newDPI);
}

void KMenuButton::OnPress()
{
	if (buttonMenu)
	{
		POINT point = {compX, compY};
		::ClientToScreen(compParentHWND, &point); // get screen cordinates

		::TrackPopupMenu(buttonMenu->GetMenuHandle(), 
			TPM_LEFTBUTTON, point.x, point.y + compHeight, 
			0, compParentHWND, NULL);
	}
}

bool KMenuButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if (msg == WM_NOTIFY)
	{		
		if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
		{
			LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW) lParam;

			*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

			if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
			{
				*result = CDRF_NOTIFYPOSTPAINT;
			}
			else if ( CDDS_POSTPAINT== lpNMCD->dwDrawStage ) //  postpaint stage
			{
				const RECT rc = lpNMCD->rc;
				KGraphics::Draw3dVLine(lpNMCD->hdc, rc.right - ::MulDiv(22, compDPI, USER_DEFAULT_SCREEN_DPI),
					rc.top + ::MulDiv(6, compDPI, USER_DEFAULT_SCREEN_DPI), 
					rc.bottom - ::MulDiv(12, compDPI, USER_DEFAULT_SCREEN_DPI)); // draw line

				const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;

				HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, arrowFont->GetFontHandle());
				const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
				const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

				RECT rcIcon = { rc.right - ::MulDiv(18, compDPI, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
				::DrawTextW(lpNMCD->hdc, L"\x36", 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw arrow

				if (glyphFont) // draw glyph
				{
					::SelectObject(lpNMCD->hdc, glyphFont->GetFontHandle());
					::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);

					rcIcon = { rc.left + ::MulDiv(glyphLeft, compDPI, USER_DEFAULT_SCREEN_DPI), rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				}

				::SetBkMode(lpNMCD->hdc, oldBkMode);
				::SetTextColor(lpNMCD->hdc, oldTextColor);
				::SelectObject(lpNMCD->hdc, oldFont);

				*result = CDRF_DODEFAULT;
			}

			return true; // indicate that we processed this msg & result is valid.
		}
	}

	return KButton::EventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KMenuItem.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KMenuItem::KMenuItem()
{
	hMenu = 0;
	listener = nullptr;
	param = nullptr;
	intParam = -1;
	enabled = true;
	checked = false;
	itemID = KIDGenerator::GetInstance()->GenerateMenuItemID(this);
}

void KMenuItem::AddToMenu(HMENU hMenu)
{
	this->hMenu = hMenu;

	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
	mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);
	mii.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);
	mii.wID = itemID;
	mii.dwItemData = (ULONG_PTR)this; // for future!

	::InsertMenuItemW(hMenu, itemID, FALSE, &mii);

}

void KMenuItem::SetParam(void* param)
{
	this->param = param;
}

void KMenuItem::SetIntParam(int intParam)
{
	this->intParam = intParam;
}

int KMenuItem::GetIntParam()
{
	return intParam;
}

void* KMenuItem::GetParam()
{
	return param;
}

bool KMenuItem::IsChecked()
{
	return checked;
}

void KMenuItem::SetCheckedState(bool state)
{
	checked = state;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

bool KMenuItem::IsEnabled()
{
	return enabled; 
}

void KMenuItem::SetEnabled(bool state)
{
	enabled = state;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = enabled ? MFS_ENABLED : MFS_DISABLED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

void KMenuItem::SetText(const KString& text)
{
	itemText = text;
	if(hMenu) // already created menu item
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
		mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

KString KMenuItem::GetText()
{
	return itemText;
}

UINT KMenuItem::GetItemID()
{
	return itemID;
}

HMENU KMenuItem::GetMenuHandle()
{
	return hMenu;
}

void KMenuItem::SetListener(KMenuItemListener* listener)
{
	this->listener = listener;
}

KMenuItemListener* KMenuItem::GetListener()
{
	return listener;
}

void KMenuItem::OnPress()
{
	if(listener)
		listener->OnMenuItemPress(this);
}

KMenuItem::~KMenuItem()
{
}


// =========== KMenuItemListener.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KMenuItemListener::KMenuItemListener(){}

KMenuItemListener::~KMenuItemListener(){}

void KMenuItemListener::OnMenuItemPress(KMenuItem* menuItem){}

// =========== KNumericField.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/


KNumericField::KNumericField() : KTextBox(false)
{
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL | ES_NUMBER;
}

KNumericField::~KNumericField(){}

// =========== KPasswordBox.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KPasswordBox::KPasswordBox(bool readOnly):KTextBox(readOnly)
{
	pwdChar = L'*';
	compDwStyle = compDwStyle | ES_PASSWORD;
}

void KPasswordBox::SetPasswordChar(const wchar_t pwdChar)
{
	this->pwdChar = pwdChar;
	if(compHWND)
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		this->Repaint();
	}
}

wchar_t KPasswordBox::GetPasswordChar()
{
	return pwdChar;
}

bool KPasswordBox::Create(bool requireInitialMessages)
{
	if(KTextBox::Create(requireInitialMessages))
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		return true;
	}
	return false;
}

KPasswordBox::~KPasswordBox()
{
}

// =========== KProgressBar.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/

#include <commctrl.h>

KProgressBar::KProgressBar(bool smooth, bool vertical) : KComponent(false)
{
	value = 0;

	compClassName.AssignStaticText(TXT_WITH_LEN("msctls_progress32"));

	compWidth = 100;
	compHeight = 20;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS;
	compDwExStyle = WS_EX_WINDOWEDGE;

	if(smooth)
		compDwStyle = compDwStyle | PBS_SMOOTH;

	if(vertical)
		compDwStyle = compDwStyle | PBS_VERTICAL;
}

int KProgressBar::GetValue()
{
	return value;
}

void KProgressBar::SetValue(int value)
{
	this->value=value;

	if(compHWND)
		::SendMessageW(compHWND, PBM_SETPOS, value, 0);
}

bool KProgressBar::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register PROGRESS_CLASSW class!

	if(compHWND)
	{
		::SendMessageW(compHWND, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
		::SendMessageW(compHWND, PBM_SETPOS, value, 0); // set current value!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;	
}

KProgressBar::~KProgressBar()
{
}

// =========== KPushButton.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/


KPushButton::KPushButton()
{
	compText.AssignStaticText(TXT_WITH_LEN("Push Button"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | 
		BS_PUSHLIKE | BS_NOTIFY | WS_TABSTOP;
}

KPushButton::~KPushButton()
{
}

// =========== KRadioButton.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/


KRadioButton::KRadioButton()
{
	compText.AssignStaticText(TXT_WITH_LEN("RadioButton"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | 
		BS_RADIOBUTTON | BS_NOTIFY | WS_TABSTOP;
}

KRadioButton::~KRadioButton()
{
}

// =========== KTextArea.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KTextArea::KTextArea(bool autoScroll, bool readOnly):KTextBox(readOnly)
{
	compWidth = 200;
	compHeight = 100;

	compDwStyle = compDwStyle | ES_MULTILINE | ES_WANTRETURN;

	if(autoScroll)
		compDwStyle = compDwStyle | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
	else
		compDwStyle = compDwStyle | WS_HSCROLL | WS_VSCROLL;
}

LRESULT KTextArea::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS; // to catch TAB key
	return KTextBox::WindowProc(hwnd, msg, wParam, lParam);
}

KTextArea::~KTextArea()
{
}

// =========== KTextBox.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KTextBox::KTextBox(bool readOnly) : KComponent(false)
{
	compClassName.AssignStaticText(TXT_WITH_LEN("EDIT"));

	compWidth = 100;
	compHeight = 20;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL;

	if(readOnly)
		compDwStyle = compDwStyle | ES_READONLY;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

KString KTextBox::GetText()
{
	if(compHWND)
	{
		const int length = ::GetWindowTextLengthW(compHWND);
		if(length)
		{
			const int size = (length + 1) * sizeof(wchar_t);
			wchar_t *text = (wchar_t*)::malloc(size);
			text[0] = 0;
			::GetWindowTextW(compHWND, text, size);
			compText = KString(text, KString::FREE_TEXT_WHEN_DONE);
		}else
		{
			compText = KString();
		}
	}
	return compText;
}


bool KTextBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register EDIT class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KTextBox::~KTextBox()
{
}

// =========== KTimer.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KTimer::KTimer()
{
	resolution = 1000;
	started = false;
	listener = nullptr;
	timerID = KIDGenerator::GetInstance()->GenerateTimerID(this);
}

void KTimer::SetInterval(int resolution)
{
	this->resolution = resolution;
}

int KTimer::GetInterval()
{
	return resolution;
}

void KTimer::SetTimerWindow(KWindow* window)
{
	this->window = window;
}

void KTimer::SetTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT KTimer::GetTimerID()
{
	return timerID;
}

void KTimer::StartTimer()
{
	if(started)
		return;

	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			::SetTimer(hwnd, timerID, resolution, 0);
			started = true;
		}
	}
}

void KTimer::StopTimer()
{
	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			if(started)
				::KillTimer(hwnd, timerID);

			started = false;
		}
	}
}

bool KTimer::IsTimerRunning()
{
	return started;
}

void KTimer::OnTimer()
{
	if(listener)
		listener->OnTimer(this);
}

void KTimer::SetListener(KTimerListener* listener)
{
	this->listener = listener;
}

KTimer::~KTimer()
{
	if(started)
		this->StopTimer();
}

// =========== KTimerListener.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KTimerListener::KTimerListener(){}

KTimerListener::~KTimerListener(){}

void KTimerListener::OnTimer(KTimer* timer){}



// =========== KToolTip.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#include <commctrl.h>

KToolTip::KToolTip() : KComponent(false)
{
	attachedCompHWND = 0;
	compClassName.AssignStaticText(TXT_WITH_LEN("tooltips_class32"));

	compDwStyle = WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX;
}

KToolTip::~KToolTip()
{
}

void KToolTip::AttachToComponent(KWindow* parentWindow, KComponent* attachedComponent)
{
	compParentHWND = parentWindow->GetHWND();
	attachedCompHWND = attachedComponent->GetHWND();

	compHWND = ::CreateWindowExW(0, compClassName, NULL, 
		compDwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, compParentHWND, NULL, KApplication::hInstance, 0);

	if (compHWND)
	{
		::SetWindowPos(compHWND, HWND_TOPMOST, 0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		KGUIProc::AttachRFCPropertiesToHWND(compHWND, (KComponent*)this);

		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = compText;

		SendMessageW(compHWND, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}
}

bool KToolTip::Create(bool requireInitialMessages)
{
	return false;
}

void KToolTip::SetText(const KString& compText)
{
	this->compText = compText;
	if (compHWND)
	{
		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = compText;
		toolInfo.hinst = KApplication::hInstance;

		SendMessageW(compHWND, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}


// =========== KTrackBar.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/


#include <commctrl.h>

KTrackBar::KTrackBar(bool showTicks, bool vertical) : KComponent(false)
{
	listener = 0;
	rangeMin = 0;
	rangeMax = 100;
	value = 0;

	compWidth = 100;
	compHeight = 25;

	compX = 0;
	compY = 0;

	compDwStyle = (WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS) | 
		(showTicks ? TBS_AUTOTICKS : TBS_NOTICKS) | 
		(vertical ? TBS_VERT : TBS_HORZ);

	compDwExStyle = WS_EX_WINDOWEDGE;

	compClassName.AssignStaticText(TXT_WITH_LEN("msctls_trackbar32"));
}

void KTrackBar::SetRange(int min, int max)
{
	rangeMin = min;
	rangeMax = max;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));	
}

void KTrackBar::SetValue(int value)
{
	this->value = value;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);
}

void KTrackBar::OnChange()
{
	value = (int)::SendMessageW(compHWND, TBM_GETPOS, 0, 0);
	if(listener)
		listener->OnTrackBarChange(this);
}

void KTrackBar::SetListener(KTrackBarListener* listener)
{
	this->listener = listener;
}

int KTrackBar::GetValue()
{
	return value;
}

bool KTrackBar::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
	if( (msg == WM_HSCROLL) || (msg == WM_VSCROLL) )
	{
		const int nScrollCode = (int)LOWORD(wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || 
			(TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || 
			(TB_PAGEDOWN == nScrollCode) || (TB_THUMBPOSITION == nScrollCode)) // its trackbar!
		{
			this->OnChange();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KTrackBar::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	KGUIProc::CreateComponent(this, requireInitialMessages); // we dont need to register TRACKBAR_CLASSW class!

	if(compHWND)
	{
		::EnableWindow(compHWND, compEnabled);
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!	
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(rangeMin, rangeMax));	
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

KTrackBar::~KTrackBar()
{
}


// =========== KTrackBarListener.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/


KTrackBarListener::KTrackBarListener(){}

KTrackBarListener::~KTrackBarListener(){}

void KTrackBarListener::OnTrackBarChange(KTrackBar* trackBar){}

// =========== KTransparentBitmap.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/


KTransparentBitmap::KTransparentBitmap(void* data, int width, int height, int stride)
{
	if ((width * sizeof(unsigned int)) != stride) // this should not happen! no padding in 32bpp data.
	{
		hdcMem = 0;
		width = 0;
		height = 0;
		hbm = 0;
		hbmPrev = 0;
		pvBits = nullptr;
		return;
	}

	this->width = width;
	this->height = height;

	hdcMem = ::CreateCompatibleDC(NULL);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hbm = ::CreateDIBSection(hdcMem, &bmi,
		DIB_RGB_COLORS, &pvBits,
		NULL, 0);

	::memcpy(pvBits, data, height * width * sizeof(unsigned int));

	// the color format for each pixel is 0xaarrggbb  
	/*
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			((unsigned int*)pvBits)[x + y * width] = ((unsigned int*)data)[x + y * width];
	*/

	hbmPrev = (HBITMAP)::SelectObject(hdcMem, hbm);

}

KTransparentBitmap::KTransparentBitmap(int width, int height)
{
	this->CreateEmptyBitmap(width, height);
}

void KTransparentBitmap::CreateEmptyBitmap(int width, int height)
{
	this->width = width;
	this->height = height;

	hdcMem = ::CreateCompatibleDC(NULL);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hbm = ::CreateDIBSection(hdcMem, &bmi,
		DIB_RGB_COLORS, &pvBits,
		NULL, 0);

	::ZeroMemory(pvBits, height * width * sizeof(unsigned int));

	hbmPrev = (HBITMAP)::SelectObject(hdcMem, hbm);
}

void KTransparentBitmap::ReleaseResources()
{
	if (hdcMem == 0)
		return;

	::SelectObject(hdcMem, hbmPrev);
	::DeleteObject(hbm);
	::DeleteDC(hdcMem);
}

bool KTransparentBitmap::HitTest(int x, int y)
{
	if (pvBits == nullptr)
		return false;

	if ((x < 0) || (y < 0) || (x > (width - 1)) || (y > (height - 1)))
		return false;

	unsigned int pixelColor = ((unsigned int*)pvBits)[x + y * width];
	return ((pixelColor >> 24) == 0xff);
}

unsigned int KTransparentBitmap::GetPixel(int x, int y)
{
	if (pvBits == nullptr)
		return 0;

	if ((x < 0) || (y < 0) || (x > (width - 1)) || (y > (height - 1)))
		return 0;

	return ((unsigned int*)pvBits)[x + y * width];
}

int KTransparentBitmap::GetWidth()
{
	return width;
}

int KTransparentBitmap::GetHeight()
{
	return height;
}

void KTransparentBitmap::Resize(int width, int height)
{
	this->ReleaseResources();
	this->CreateEmptyBitmap(width, height);
}

HDC KTransparentBitmap::GetDC()
{
	return hdcMem;
}

void KTransparentBitmap::Draw(HDC destHdc, int destX, int destY, BYTE alpha)
{
	this->Draw(destHdc, destX, destY, width, height, 0, 0, width, height, alpha);
}

void KTransparentBitmap::Draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, BYTE alpha)
{
	this->Draw(destHdc, destX, destY, destWidth, destHeight, 0, 0, width, height, alpha);
}

void KTransparentBitmap::Draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight, BYTE alpha)
{
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
	::AlphaBlend(destHdc, destX, destY, destWidth, destHeight,
		hdcMem, srcX, srcY, srcWidth, srcHeight, bf);
}

KTransparentBitmap::~KTransparentBitmap()
{
	this->ReleaseResources();
}



// =========== KWindow.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KWindow::KWindow() : KComponent(true), componentList(50, false)
{
	compText.AssignStaticText(TXT_WITH_LEN("KWindow"));

	compWidth = 400;
	compHeight = 200;

	compVisible = false;
	enableDPIUnawareMode = false;
	prevDPIContext = 0;
	dpiAwarenessContextChanged = false;
	compDwStyle = WS_POPUP;
	compDwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	compCtlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
	dpiChangeListener = nullptr;
	windowIcon = nullptr;
	largeIconHandle = 0;
	smallIconHandle = 0;

	closeOperation = KCloseOperation::DestroyAndExit;
}

void KWindow::ApplyDPIUnawareModeToThread()
{
	if ((KApplication::dpiAwareness == KDPIAwareness::MIXEDMODE_ONLY) && KApplication::dpiAwareAPICalled && enableDPIUnawareMode)
	{
		if (KDPIUtility::pSetThreadDpiAwarenessContext)
		{
			prevDPIContext = KDPIUtility::pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
			dpiAwarenessContextChanged = true;
		}
	}
}

void KWindow::RestoreDPIModeOfThread()
{
	if (dpiAwarenessContextChanged)
		KDPIUtility::pSetThreadDpiAwarenessContext(prevDPIContext);
}

bool KWindow::Create(bool requireInitialMessages)
{
	if (enableDPIUnawareMode)
		this->ApplyDPIUnawareModeToThread();

	bool retVal = KComponent::Create(requireInitialMessages);

	if (enableDPIUnawareMode)
		this->RestoreDPIModeOfThread();

	if(retVal && (KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
	{
		int dpi = KDPIUtility::GetWindowDPI(compHWND);
		if (dpi != USER_DEFAULT_SCREEN_DPI)
			this->SetDPI(dpi);
	}

	return retVal;
}

void KWindow::Flash()
{
	::FlashWindow(compHWND, TRUE);
}

void KWindow::UpdateWindowIconForNewDPI()
{
	if (windowIcon == nullptr)
		return;

	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);

	// 32x32 for large icon
	// 16x16 for small icon

	if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled)
	{
		largeIconHandle = windowIcon->GetScaledIcon(KDPIUtility::ScaleToNewDPI(32, compDPI));
		smallIconHandle = windowIcon->GetScaledIcon(KDPIUtility::ScaleToNewDPI(16, compDPI));
	}
	else
	{
		largeIconHandle = windowIcon->GetScaledIcon(32);
		smallIconHandle = windowIcon->GetScaledIcon(16);
	}

	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)largeIconHandle);
	::SetClassLongPtrW(compHWND, GCLP_HICONSM, (LONG_PTR)smallIconHandle);
}

void KWindow::SetIcon(KIcon* icon)
{
	windowIcon = icon;

	if (compHWND)
		this->UpdateWindowIconForNewDPI();
}

void KWindow::SetCloseOperation(KCloseOperation closeOperation)
{
	this->closeOperation = closeOperation;
}

void KWindow::SetDPIChangeListener(KDPIChangeListener* dpiChangeListener)
{
	this->dpiChangeListener = dpiChangeListener;
}

void KWindow::SetEnableDPIUnawareMode(bool enable)
{
	enableDPIUnawareMode = enable;
}

void KWindow::OnClose()
{
	if (closeOperation == KCloseOperation::DestroyAndExit)
		this->Destroy();
	else if (closeOperation == KCloseOperation::Hide)
		this->SetVisible(false);
}

void KWindow::OnDestroy()
{
	if (closeOperation == KCloseOperation::DestroyAndExit)
		::PostQuitMessage(0);
}

void KWindow::PostCustomMessage(WPARAM msgID, LPARAM param)
{
	::PostMessageW(compHWND, RFC_CUSTOM_MESSAGE, msgID, param);
}

void KWindow::OnCustomMessage(WPARAM msgID, LPARAM param)
{

}

void KWindow::CenterScreen()
{
	this->SetPosition((::GetSystemMetrics(SM_CXSCREEN) - compWidth) / 2, (::GetSystemMetrics(SM_CYSCREEN) - compHeight) / 2);
}

void KWindow::CenterOnSameMonitor(HWND window)
{
	if (window)
	{
		HMONITOR hmon = ::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

		if (hmon != NULL)
		{
			MONITORINFO monitorInfo;
			::ZeroMemory(&monitorInfo, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);

			if (::GetMonitorInfoW(hmon, &monitorInfo))
			{
				const int posX = monitorInfo.rcMonitor.left + (((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) - compWidth) / 2);
				const int posY = monitorInfo.rcMonitor.top + (((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) - compHeight) / 2);
				this->SetPosition(posX, posY);

				return;
			}
		}
	}

	this->CenterScreen();
}

bool KWindow::AddComponent(KComponent* component, bool requireInitialMessages)
{
	if(component)
	{
		if(compHWND)
		{		
			component->SetParentHWND(compHWND);

			if ((KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE) && (!enableDPIUnawareMode) && KApplication::dpiAwareAPICalled )
				component->SetDPI(compDPI);

			componentList.AddPointer(component);

			if (enableDPIUnawareMode)
				this->ApplyDPIUnawareModeToThread();

			bool retVal = component->Create(requireInitialMessages);

			if (enableDPIUnawareMode)
				this->RestoreDPIModeOfThread();

			return retVal;
		}
	}
	return false;
}

void KWindow::RemoveComponent(KComponent* component)
{
	int index = componentList.GetID(component);
	if (index != -1)
	{
		componentList.RemovePointer(index);
		component->Destroy();
	}
}

bool KWindow::AddContainer(KHostPanel* container, bool requireInitialMessages)
{
	if (container)
	{
		container->SetComponentList(&componentList);
		container->SetEnableDPIUnawareMode(enableDPIUnawareMode);
		return this->AddComponent(static_cast<KComponent*>(container), requireInitialMessages);
	}
	return false;
}

bool KWindow::SetClientAreaSize(int width, int height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		wndRect.right = wndRect.left + width;
		wndRect.bottom = wndRect.top + height;

		::AdjustWindowRect(&wndRect, compDwStyle, ::GetMenu(compHWND) == NULL ? FALSE : TRUE);
		this->SetSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

		return true;
	}
	return false;
}

bool KWindow::IsOffScreen(int posX, int posY)
{
	POINT point;
	point.x = posX;
	point.y = posY;
	return (::MonitorFromPoint(point, MONITOR_DEFAULTTONULL) == NULL);
}

bool KWindow::GetClientAreaSize(int* width, int* height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		if (width)
			*width = wndRect.right - wndRect.left;

		if (height)
			*height = wndRect.bottom - wndRect.top;

		return true;
	}
	return false;
}

void KWindow::OnMoved()
{

}

void KWindow::OnResized()
{

}

LRESULT KWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent* component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::AtomComponent));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
			{
				KComponent* component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, MAKEINTATOM(KGUIProc::AtomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL:
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
			{
				KComponent* component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(KGUIProc::AtomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}				
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent* component = (KComponent*)::GetPropW(GetDlgItem(hwnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(KGUIProc::AtomComponent));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}			
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
			{
				KComponent* component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(KGUIProc::AtomComponent));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
			{
				KTimer* timer = KIDGenerator::GetInstance()->GetTimerByID((UINT)wParam);
				if(timer)
				{
					timer->OnTimer();
					break;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compWidth = rect.right - rect.left;
				this->compHeight = rect.bottom - rect.top;

				this->OnResized();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compX = rect.left;
				this->compY = rect.top;

				this->OnMoved();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_DPICHANGED:
			{
				if ((KApplication::dpiAwareness == KDPIAwareness::UNAWARE_MODE) || enableDPIUnawareMode || (!KApplication::dpiAwareAPICalled))
					return KComponent::WindowProc(hwnd, msg, wParam, lParam);

				this->compDPI = HIWORD(wParam);
				RECT* const prcNewWindow = (RECT*)lParam;

				this->compX = prcNewWindow->left;
				this->compY = prcNewWindow->top;
				this->compWidth = prcNewWindow->right - prcNewWindow->left;
				this->compHeight = prcNewWindow->bottom - prcNewWindow->top;

				::SetWindowPos(compHWND,
					NULL,
					this->compX,
					this->compY,
					this->compWidth,
					this->compHeight,
					SWP_NOZORDER | SWP_NOACTIVATE);

				this->UpdateWindowIconForNewDPI();

				::InvalidateRect(compHWND, NULL, TRUE);

				for (int i = 0; i < componentList.GetSize(); i++)
				{
					componentList[i]->SetDPI(compDPI);
				}

				if (dpiChangeListener)
					dpiChangeListener->OnDPIChange(compHWND, compDPI);

				return 0;
			}

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(wParam) == 0) && (lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					KMenuItem* menuItem = KIDGenerator::GetInstance()->GetMenuItemByID(LOWORD(wParam));
					if(menuItem)
					{
						menuItem->OnPress();
						break;
					}
				}
				else if(lParam)// send to appropriate component
				{
					KComponent* component = (KComponent*)::GetPropW((HWND)lParam, 
						MAKEINTATOM(KGUIProc::AtomComponent));

					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
				else if (LOWORD(wParam) == IDOK) // enter key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate enter key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_RETURN, 0); 

					return 0;
				}
				else if (LOWORD(wParam) == IDCANCEL) // Esc key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate esc key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_ESCAPE, 0); 

					return 0;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_ACTIVATE: // save last focused item when inactive
			if (wParam != WA_INACTIVE)
				return KComponent::WindowProc(hwnd, msg, wParam, lParam);
			this->lastFocusedChild = ::GetFocus();
			break;

		case WM_SETFOCUS:
			if (this->lastFocusedChild) // set focus to last item
			{
				::SetFocus(this->lastFocusedChild);
			}
			else // set focus to first child
			{
				// if hCtl is NULL, GetNextDlgTabItem returns first control of the window.
				HWND hFirstControl = ::GetNextDlgTabItem(this->compHWND, NULL, FALSE);
				if (hFirstControl)
				{
					if (::GetWindowLongPtrW(hFirstControl, GWL_STYLE) & WS_TABSTOP)
						::SetFocus(hFirstControl);
				}
			}
			break;

		case WM_CLOSE:
			this->OnClose();
			break;

		case WM_DESTROY:
			this->OnDestroy();
			break;

		case RFC_CUSTOM_MESSAGE:
			this->OnCustomMessage(wParam, lParam);
			break;

		default:
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


KWindow::~KWindow()
{
	if (largeIconHandle)
		::DestroyIcon(largeIconHandle);

	if (smallIconHandle)
		::DestroyIcon(smallIconHandle);
}

// =========== KWindowTypes.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/



KHotPluggedDialog::KHotPluggedDialog(){}


void KHotPluggedDialog::OnClose()
{
	::EndDialog(compHWND, 0);
}

void KHotPluggedDialog::OnDestroy(){}

KHotPluggedDialog::~KHotPluggedDialog(){}


KOverlappedWindow::KOverlappedWindow()
{
	compText.AssignStaticText(TXT_WITH_LEN("KOverlapped Window"));
	compDwStyle = WS_OVERLAPPEDWINDOW;
}

KOverlappedWindow::~KOverlappedWindow(){}


KFrame::KFrame()
{
	compText.AssignStaticText(TXT_WITH_LEN("KFrame"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
}

KFrame::~KFrame(){}



KDialog::KDialog()
{
	compText.AssignStaticText(TXT_WITH_LEN("KDialog"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	compDwExStyle = WS_EX_DLGMODALFRAME;
}

KDialog::~KDialog(){}



KToolWindow::KToolWindow()
{
	compText.AssignStaticText(TXT_WITH_LEN("KTool Window"));
	compDwStyle = WS_OVERLAPPED | WS_SYSMENU;
	compDwExStyle = WS_EX_TOOLWINDOW;
}

KToolWindow::~KToolWindow(){}


// =========== KRunnable.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/


KRunnable::KRunnable(){}

KRunnable::~KRunnable(){}

void KRunnable::Run(KThread* thread){}

// =========== KThread.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/


#include <process.h>

unsigned __stdcall RFCThread_Proc(void* lpParameter)
{
	if (lpParameter == nullptr) // for safe!
		return 0;

	KThread* thread = (KThread*)lpParameter;
	thread->Run();

	return 0;
}

bool CreateRFCThread(KThread* thread)
{
	if (thread)
	{
		// create thread in suspended state. so we can set the handle field.
		HANDLE handle = (HANDLE)::_beginthreadex(NULL, 0, RFCThread_Proc, thread, CREATE_SUSPENDED, NULL);
		//HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL);

		if (handle)
		{
			thread->SetHandle(handle);
			::ResumeThread(handle);

			return true;
		}
	}
	return false;
}

KThread::KThread()
{
	handle = 0; 
	runnable = nullptr;
	threadShouldStop = false;
}

void KThread::SetHandle(HANDLE handle)
{
	this->handle = handle;
}

void KThread::SetRunnable(KRunnable* runnable)
{
	this->runnable = runnable;
}

HANDLE KThread::GetHandle()
{
	return handle;
}

KThread::operator HANDLE()const
{
	return handle;
}

bool KThread::ShouldRun()
{
	return !threadShouldStop;
}

void KThread::Run()
{
	if (runnable)
		runnable->Run(this);
}

bool KThread::IsThreadRunning()
{
	if (handle)
	{
		const DWORD result = ::WaitForSingleObject(handle, 0);
		return (result != WAIT_OBJECT_0);
	}

	return false;
}

void KThread::ThreadShouldStop()
{
	threadShouldStop = true;
}

DWORD KThread::WaitUntilThreadFinish(bool pumpMessages)
{
	if (!pumpMessages)
		return ::WaitForSingleObject(handle, INFINITE);
	
	while (true)
	{
		MSG msg;
		while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		// if this thread sent msgs to caller thread before calling following function and after the above while block, those msgs will not be processed until new msg arrived. 
		// if there is no new msgs, there will be a deadlock! that's why we have a timeout! after the timeout, any pending msgs will be processed and continue...
		DWORD dwRet = ::MsgWaitForMultipleObjects(1, &handle, FALSE, 200, QS_ALLINPUT);

		if (dwRet == WAIT_OBJECT_0) // thread finished
		{
			return true;
		}
		else if ((dwRet == (WAIT_OBJECT_0 + 1)) || (dwRet == WAIT_TIMEOUT)) // window message or timeout
		{
			continue;
		}
		else // failure
		{
			break;
		}
	}

	return false;
}

bool KThread::StartThread()
{
	threadShouldStop = false;

	if (handle) // close old handle
	{
		::CloseHandle(handle);
		handle = 0;
	}

	return ::CreateRFCThread(this);
}

void KThread::uSleep(int waitTime)
{
	__int64 time1 = 0, time2 = 0, freq = 0;

	::QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	::QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		::QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while ((time2 - time1) < ((waitTime * freq) / 1000000));
}

KThread::~KThread()
{
	if (handle)
		::CloseHandle(handle);
}

// =========== KPerformanceCounter.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KPerformanceCounter::KPerformanceCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);

	this->pcFreq = double(li.QuadPart) / 1000.0;
}

void KPerformanceCounter::StartCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	this->counterStart = li.QuadPart;
}

double KPerformanceCounter::EndCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return double(li.QuadPart - this->counterStart) / this->pcFreq;
}

KPerformanceCounter::~KPerformanceCounter()
{

}


// =========== KRegistry.cpp ===========

/*
	Copyright (C) 2013-2022 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/


KRegistry::KRegistry()
{

}

bool KRegistry::CreateKey(HKEY hKeyRoot, const KString& subKey)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	::RegCloseKey(hkey);
	return true;
}

bool KRegistry::DeleteKey(HKEY hKeyRoot, const KString& subKey)
{
	return ::RegDeleteKeyW(hKeyRoot, subKey) == ERROR_SUCCESS ? true : false;
}

bool KRegistry::ReadString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			if (requiredBytes == 0) // value might be empty
			{
				*result = KString();
			}
			else{
				void* buffer = ::malloc(requiredBytes + sizeof(wchar_t)); // + for strings which doesn't have ending null
				::ZeroMemory(buffer, requiredBytes + sizeof(wchar_t)); // zero the ending null

				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)buffer, &requiredBytes);
				*result = KString((wchar_t*)buffer, KString::FREE_TEXT_WHEN_DONE);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::WriteString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const int bCount = (value.GetLength() + 1) * sizeof(wchar_t); // +1 for ending null
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_SZ, (LPBYTE)(const wchar_t*)value, bCount);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::ReadDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		const LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, &dwType, (LPBYTE)result, &dwSize);
		::RegCloseKey(hkey);

		if (ret == ERROR_SUCCESS)
			return true;
	}
	return false;
}

bool KRegistry::WriteDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const DWORD dwSize = sizeof(DWORD);
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_DWORD, (LPBYTE)&value, dwSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::ReadBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void** buffer, DWORD* buffSize)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			*buffSize = requiredBytes;
			if (requiredBytes == 0) // value might be empty
			{
				*buffer = 0;
			}
			else{
				*buffer = ::malloc(requiredBytes);
				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)*buffer, &requiredBytes);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::WriteBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void* buffer, DWORD buffSize)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_BINARY, (LPBYTE)buffer, buffSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

KRegistry::~KRegistry()
{

}