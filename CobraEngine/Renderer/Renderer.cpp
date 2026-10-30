#include "Renderer.h"
#include <cassert>

Renderer* Renderer::m_instance = nullptr;

// 화면 전체 칸 수만큼 문자/색상 배열과 우선순위 배열 생성
Renderer::Frame::Frame(int area)
{
	charInfoArray = std::make_unique<CHAR_INFO[]>(area); // 각 칸의 최종 문자와 색상을 저장할 배열
	sortingOrderArray = std::make_unique<int[]>(area); // 각 칸에 기록된 요청의 그리기 우선순위를 저장할 배열
}

// 이전 화면 데이터 지우고, 새 화면 구성
// Frame 내부 배열 초기화
void Renderer::Frame::Clear(const Vector2& screenSize)
{
	for (int y = 0; y < screenSize.GetY(); y++)
	{
		for (int x = 0; x < screenSize.GetX(); x++)
		{
			// 화면 좌표 (x,y)를 일차원 배열 번호로 변환한다
			// 배열 번호 = y * 화면 너비 + x
			int index = y * screenSize.GetX() + x;

			charInfoArray[index].Char.AsciiChar = ' '; // 이전 문자를 공백으로 변경
			charInfoArray[index].Attributes = 0; // 이전 색상 속성 초기화
			sortingOrderArray[index] = -1; // 기본 요청 순서 0보다 낮은 값으로 초기화
		}
	}
}

// 화면 크기 저장하고, 렌더링에 필요한 자원 준비
Renderer::Renderer(int width, int height) : m_screenSize(width, height)
{
	assert(m_instance == nullptr);
	m_instance = this;

	// 화면 표시와 다음 화면 작성을 번갈아 담당할 버퍼 두 개를 생성
	m_screenBuffers[0] = std::make_unique<ScreenBuffer>(width, height);
	m_screenBuffers[1] = std::make_unique<ScreenBuffer>(width, height);
	
	// 두 Windows 버퍼의 문자를 공백으로 채운다
	m_screenBuffers[0]->Clear();
	m_screenBuffers[1]->Clear();

	// 처음에는 0번 버퍼를 화면에 표시
	BOOL result = SetConsoleActiveScreenBuffer(m_screenBuffers[0]->GetHandle());
	assert(result == TRUE);

	// 최종 화면을 구성할 Frame과 내부 배열을 생성
	m_frame = std::make_unique<Frame>(m_screenSize.GetX() * m_screenSize.GetY());
	m_frame->Clear(m_screenSize); // Frame을 공백과 초기 우선순위로 준비
}

Renderer::~Renderer()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // 표준 출력 핸들로 저장
	BOOL result = SetConsoleActiveScreenBuffer(handle); // 기존 콘솔 버퍼를 다시 표시
	assert(result == TRUE);

	if (m_instance)
	{
		m_instance = nullptr;
	}
}

Renderer& Renderer::Get()
{
	assert(m_instance);
	return *m_instance;
}

// 그리기 요청 하나를 만들어 이번 프레임의 요청 목록에 추가
// 이 함수에서는 Frame이나 Windows 버퍼에 기록하지 않는다
void Renderer::Submit(const std::string& image, const Vector2& position, Color color, int sortingOrder)
{
	RenderCommand command;

	// 전달받은 정보를 요청 객체에 저장
	command.image = image;
	command.position = position;
	command.color = color;
	command.sortingOrder = sortingOrder;

	// 요청을 vector에 복사해서 보관
	m_renderCommands.push_back(command);
}

// 한 프레임의 화면 초기화, 구성, 표시를 순서대로 실행
void Renderer::Draw()
{
	// 이전 문자와 우선 순위 제거
	// 아직 처리하지 않은 요청 목록 유지
	m_frame->Clear(m_screenSize);

	// 요청으로 Frame을 구성하고 백버퍼에 기록
	DrawRenderCommands();

	// 완성된 백버퍼를 표시하고 다른 버퍼로 변경
	Present();
}

// 요청 목록을 읽어 Frame을 구성한 뒤 백버퍼에 기록
void Renderer::DrawRenderCommands()
{
	// 이번 프레임의 요청을 순서대로 처리
	// 각 요청을 element에 복사
	for (const RenderCommand& command : m_renderCommands) 
	{
		// 문자열 전체가 화면 밖의 줄에 있으면 건너뛴다
		if (command.position.GetY() < 0 || command.position.GetY() >= m_screenSize.GetY())
		{
			continue;
		}

		// 요청 문자열의 각 글자를 순서대로 처리
		for (int i = 0; i < static_cast<int>(command.image.size()); i++) //글자
		{
			// 화면 밖의 글자만 건너뛰어 부분적으로 보이는 문자열도 처리
			if (command.position.GetX() + i < 0 || command.position.GetX() + i >= m_screenSize.GetX())
			{
				continue;
			}

			const int frameIndex = command.position.GetY() * m_screenSize.GetX() + command.position.GetX() + i;

			// 같은 칸에 더 높은 우선순위의 문자가 기록되어 있으면 건너뛴다
			// 우선순위가 같으면 나중 요청이 덮어쓴다
			if (m_frame->sortingOrderArray[frameIndex] > command.sortingOrder)
			{
				continue;
			}

			// 해당 화면 칸에 이번 글자를 기록한다
			m_frame->charInfoArray[frameIndex].Char.AsciiChar = command.image[i];
			
			// 해당 칸의 색상을 Windows 속성 자료형에 맞춰 기록한다
			m_frame->charInfoArray[frameIndex].Attributes = static_cast<WORD>(command.color);
			
			// 우선순위 저장
			m_frame->sortingOrderArray[frameIndex] = command.sortingOrder;
		}
	}
	// 모든 요청으로 완성한 문자/색상 배열을 백버퍼에 기록
	m_screenBuffers[m_backBufferIndex]->Draw(m_frame->charInfoArray.get());
	
	// 처리한 요청 제거하고 다음 프레임 요청 받을 준비
	// Frame과 Windows 버퍼의 내용은 유지
	m_renderCommands.clear();
}

// 완성된 백버퍼를 화면에 표시하고 다음 작성 대상 버퍼를 선택
void Renderer::Present()
{
	// 화면 데이터를 기록한 백버퍼 표시
	BOOL result = SetConsoleActiveScreenBuffer(m_screenBuffers[m_backBufferIndex]->GetHandle());
	assert(result == TRUE);

	// 다음 화면은 다른 버퍼에 작성
	m_backBufferIndex = 1 - m_backBufferIndex;
}
