
#pragma once

#include "rfc/rfc.h"
#include "CustomMessages.h"

class ChatTextArea : public KTextArea
{
public:
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		// Check if Ctrl+S is pressed
		if ((GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'S')
		{
			::PostMessageW(compParentHWND, RFC_CUSTOM_MESSAGE, CTRL_S_PRESSED_MSG, 0);
			return 0;  // Stop further processing
		}
		else
		{
			return KTextArea::WindowProc(compHWND, WM_KEYDOWN, wParam, lParam);
		}
	}

	LRESULT OnWMChar(WPARAM wParam, LPARAM lParam)
	{
		// Prevent the beep for Ctrl+S by ignoring WM_CHAR when Ctrl is pressed
		if ((GetKeyState(VK_CONTROL) & 0x8000) && wParam == 19) // 19 is the ASCII value of Ctrl+S
		{
			return 0;
		}
		else
		{
			return KTextArea::WindowProc(compHWND, WM_CHAR, wParam, lParam);
		}
	}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_KEYDOWN, OnKeyDown)
		ON_KMSG(WM_CHAR, OnWMChar)
	END_KMSG_HANDLER(KTextArea)
};