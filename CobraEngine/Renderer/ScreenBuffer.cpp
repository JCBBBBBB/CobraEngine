#include "ScreenBuffer.h"
#include <cassert>

ScreenBuffer::ScreenBuffer(int width, int height) : m_screenSize(width, height)
{

	// 이 설정으로 문자 화면 공간을 하나 만들어줘 -> 핸들 저장
	m_screenBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	assert(m_screenBuffer != INVALID_HANDLE_VALUE); // INVALID_HANDLE_VALUE면 실패

	// 화면 창 크기 설정
	SMALL_RECT rect = {};
	rect.Top = 0;
	rect.Left = 0;
	rect.Right = static_cast<short>(m_screenSize.GetX() - 1);
	rect.Bottom = static_cast<short>(m_screenSize.GetY() - 1);
	BOOL result = SetConsoleWindowInfo(m_screenBuffer, TRUE, &rect);
	assert(result == TRUE);

	// 화면 버퍼 크기 설정 및 예외 처리
	COORD coord = {};
	coord.X = static_cast<short>(m_screenSize.GetX());
	coord.Y = static_cast<short>(m_screenSize.GetY());
	result = SetConsoleScreenBufferSize(m_screenBuffer, coord);
	assert(result == TRUE);

	// 커서 끄기
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(m_screenBuffer, &info);
	info.bVisible = FALSE;
	SetConsoleCursorInfo(m_screenBuffer, &info);
}

ScreenBuffer::~ScreenBuffer()
{
	// 값이 있다면
	if (m_screenBuffer != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_screenBuffer);
	}
}

// 콘솔을 지우는 함수
// 공백 문자를 화면 크기 전체에 한번에 설정
void ScreenBuffer::Clear() const
{
	// 글자를 채울 시작 위치
	COORD coord = {};

	// 실제로 쓴 문자 개수
	DWORD writtenCount = 0;

	// 이 버퍼의 시작 위치부터, 전체 칸에 공백을 써줘. 실제 몇칸 썼는지도 알려줘
	BOOL result = FillConsoleOutputCharacterA(m_screenBuffer, ' ', m_screenSize.GetX() * m_screenSize.GetY(), coord, &writtenCount);
	assert(result == TRUE);
}

// 화면 크기의 문자/색상 데이터를 이 객체의 콘솔 버퍼에 기록한다
void ScreenBuffer::Draw(const CHAR_INFO* data) const
{
	// 입력 배열을 가로/세로 몇 칸으로 해석할지 지정
	COORD bufferSize = {};
	bufferSize.X = static_cast<SHORT>(m_screenSize.GetX());
	bufferSize.Y = static_cast<SHORT>(m_screenSize.GetY());
	
	// 입력 배열에서 읽기 시작할 좌표 : (0,0)
	COORD bufferPosition = {};
	bufferPosition.X = 0;
	bufferPosition.Y = 0;
	
	// 문자를 기록할 범위
	SMALL_RECT writeRegion = {};
	writeRegion.Top = 0;
	writeRegion.Left = 0;
	writeRegion.Bottom = static_cast<SHORT>(m_screenSize.GetY() - 1);
	writeRegion.Right = static_cast<SHORT>(m_screenSize.GetX() - 1);


	// 입력 배열의 문자와 색상을 대상 버퍼의 지정 영역에 복사한다
	BOOL result = WriteConsoleOutputA(m_screenBuffer, data, bufferSize, bufferPosition, &writeRegion);
	assert(result == TRUE);
}
