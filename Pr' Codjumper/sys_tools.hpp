#pragma once

#include "pch.hpp"

DWORD Sys_MilliSeconds();

inline ivec2 GetCPos()
{
	tagPOINT p;
	GetCursorPos(&p);

	return { p.x, p.y };
}