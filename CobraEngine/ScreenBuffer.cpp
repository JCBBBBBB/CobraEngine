#include "ScreenBuffer.h"
#include <cassert>

ScreenBuffer::ScreenBuffer(int width, int height) : m_screenSize(width, height)
{
	assert(width > 0 && width <= 32767 && height > 0 && height <= 32767);

	// 이 설정으로 문자 화면 공간을 하나 만들어줘 -> 핸들 저장
	m_screenBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	assert(m_screenBuffer != INVALID_HANDLE_VALUE); // INVALID_HANDLE_VALUE면 실패

	// 목표 크기를 Windows 자료형으로 준비
	COORD bufferSize = { static_cast<SHORT>(m_screenSize.GetX()), static_cast<SHORT>(m_screenSize.GetY()) };
	// 버퍼에서 어느 범위를 보여줄지 정함
	SMALL_RECT windowRect = { 0, 0, static_cast<SHORT>(bufferSize.X - 1), static_cast<SHORT>(bufferSize.Y - 1) };

	CONSOLE_SCREEN_BUFFER_INFO currentInfo = {}; // 현재 버퍼 크기 저장
	BOOL result = GetConsoleScreenBufferInfo(m_screenBuffer, &currentInfo); // 이 핸들이 가리키는 버퍼의 현재 크기 currentInfo에 담아줌
	assert(result == TRUE);

	COORD temporarySize = {};

	temporarySize.X = bufferSize.X > currentInfo.dwSize.X ? bufferSize.X : currentInfo.dwSize.X;
	temporarySize.Y = bufferSize.Y > currentInfo.dwSize.Y ? bufferSize.Y : currentInfo.dwSize.Y;

	result = SetConsoleScreenBufferSize(m_screenBuffer, temporarySize); // 문자를 저장하는 공간을 임시 크기로 변경
	assert(result == TRUE);
	result = SetConsoleWindowInfo(m_screenBuffer, TRUE, &windowRect); // 보이는 영역을 목표 범위로 변경
	assert(result == TRUE);
	result = SetConsoleScreenBufferSize(m_screenBuffer, bufferSize); // 남는 저장 공간을 목표 크기로 줄임
	assert(result == TRUE);

	CONSOLE_CURSOR_INFO cursorInfo = {};
	result = GetConsoleCursorInfo(m_screenBuffer, &cursorInfo); // 기존 커서 정보 읽기
	assert(result == TRUE);

	cursorInfo.bVisible = FALSE; // 보이기 여부를 안보이게 처리
	result = SetConsoleCursorInfo(m_screenBuffer, &cursorInfo); // Windows에 적용
	assert(result);
}

ScreenBuffer::~ScreenBuffer()
{
	// 값이 있다면
	if (m_screenBuffer != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_screenBuffer);
	}
}

// 숨겨진 버퍼를 공백으로 채움
void ScreenBuffer::Clear() const
{
	COORD startPosition = {};
	DWORD writtenCount = 0; // 실제로 쓴 문자 개수

	// 이 버퍼의 시작 위치부터, 전체 칸에 공백을 써줘. 실제 몇칸 썼는지도 알려줘
	BOOL result = FillConsoleOutputCharacterA(m_screenBuffer, ' ', m_screenSize.GetX() * m_screenSize.GetY(), startPosition, &writtenCount);
	assert(result == TRUE);
}

// 화면 크기의 문자/색상 데이터를 이 객체의 콘솔 버퍼에 기록한다
void ScreenBuffer::Draw(const CHAR_INFO* data) const
{
	// 입력 배열을 가로/세로 몇 칸으로 해석할지 지정
	COORD bufferSize = { static_cast<SHORT>(m_screenSize.GetX()), static_cast<SHORT>(m_screenSize.GetY()) };
	
	// 입력 배열에서 읽기 시작할 좌표 : (0,0)
	COORD startPosition = {};
	
	// 문자를 기록할 범위
	SMALL_RECT writeRegion = { 0, 0, static_cast<SHORT>(bufferSize.X - 1), static_cast<SHORT>(bufferSize.Y - 1) };

	// 입력 배열의 문자와 색상을 대상 버퍼의 지정 영역에 복사한다
	BOOL result = WriteConsoleOutputA(m_screenBuffer, data, bufferSize, startPosition, &writeRegion);
	assert(result == TRUE);
}
