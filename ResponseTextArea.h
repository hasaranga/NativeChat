
// (c) 2024 CrownSoft
// https://www.crownsoft.net

#pragma once

#include "rfc/rfc.h"

class ResponseTextArea : public KTextArea
{
public:
	ResponseTextArea()
	{
		compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP |
			WS_VSCROLL | ES_MULTILINE | ES_READONLY;
	}
};