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

	void Clear() const; // 다음 화면 그리기 전에 버퍼 비우는 함수
	void Draw(const CHAR_INFO* data) const; // 전달받은 화면 크기의 문자 배열을 이 버퍼에 기록

	HANDLE GetHandle() const { return m_screenBuffer; }

private:
	HANDLE m_screenBuffer = INVALID_HANDLE_VALUE; // 윈도우가 돌려준 버퍼 핸들
	Vector2 m_screenSize; // 사용할 화면의 가로/세로 크기
};
