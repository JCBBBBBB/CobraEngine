#include "ScreenBuffer.h"
#include <cassert>

ScreenBuffer::ScreenBuffer(int width, int height) : m_screenSize(width, height)
{
	// 버퍼를 읽고 쓸 권한 요청, 다른 열기 작업에서도 읽기/쓰기 허용, 별도의 보안 속성 지정 생략, 문자 화면 버퍼 요청, 비워둠
	m_screenBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	assert(m_screenBuffer != INVALID_HANDLE_VALUE); // INVALID_HANDLE_VALUE면 실패

	COORD bufferSize = {static_cast<SHORT>(m_screenSize.GetX()), static_cast<SHORT>(m_screenSize.GetY())};
	SMALL_RECT windowRect = { 0, 0, static_cast<SHORT>(bufferSize.X - 1), static_cast<SHORT>(bufferSize.Y - 1) };

}

ScreenBuffer::~ScreenBuffer()
{
	// 값이 있다면
	if (m_screenBuffer != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_screenBuffer);
	}
}
