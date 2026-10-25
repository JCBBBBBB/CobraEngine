#pragma once
#include "Core/Core.h"
#include <Windows.h>

enum class Color : WORD
{
	Red = FOREGROUND_RED,
	Green = FOREGROUND_GREEN,
	Blue = FOREGROUND_BLUE,
	Yellow = Red | Green,
	Cyan = Green | Blue,
	White = Red | Green | Blue,
	BrightWhite = White | FOREGROUND_INTENSITY,
};

