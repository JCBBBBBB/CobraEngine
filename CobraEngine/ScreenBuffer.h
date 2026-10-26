#pragma once
#include <Windows.h>
#include "Math/Vector2.h"

class ScreenBuffer
{
public:
	ScreenBuffer(int width, int height);
	~ScreenBuffer();

	ScreenBuffer& operator=(const ScreenBuffer&) = delete;
	ScreenBuffer(const ScreenBuffer&) = delete;

private:
	HANDLE m_screenBuffer = INVALID_HANDLE_VALUE; // 윈도우가 돌려준 버퍼 핸들
	Vector2 m_screenSize; // 사용할 화면의 가로/세로 크기
};
