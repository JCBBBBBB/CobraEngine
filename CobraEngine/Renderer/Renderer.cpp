#include "Renderer.h"
#include <cassert>

Renderer* Renderer::m_instance = nullptr;

Renderer::Frame::Frame(int area)
{
	charInfoArray = std::make_unique<CHAR_INFO[]>(area);
	sortingOrderArray = std::make_unique<int[]>(area);
}

void Renderer::Frame::Clear(const Vector2& screenSize)
{
	for (int y = 0; y < screenSize.GetY(); y++)
	{
		for (int x = 0; x < screenSize.GetX(); x++)
		{
			charInfoArray[y * screenSize.GetX() + x].Char.AsciiChar = ' ';
			charInfoArray[y * screenSize.GetX() + x].Attributes = 0;
			sortingOrderArray[y * screenSize.GetX() + x] = -1;
		}
	}
}


Renderer::Renderer(int width, int height) : m_screenSize(width, height)
{
	assert(m_instance == nullptr);
	m_instance = this;

	m_screenBuffers[0] = std::make_unique<ScreenBuffer>(width, height);
	m_screenBuffers[1] = std::make_unique<ScreenBuffer>(width, height);
	
	m_screenBuffers[0]->Clear();
	m_screenBuffers[1]->Clear();

	BOOL result = SetConsoleActiveScreenBuffer(m_screenBuffers[0]->GetHandle()); // 0번 버퍼를 화면에 표시
	assert(result == TRUE);

	m_frame = std::make_unique<Frame>(m_screenSize.GetX() * m_screenSize.GetY());
	m_frame->Clear(m_screenSize);
}

Renderer::~Renderer()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL result = SetConsoleActiveScreenBuffer(handle);
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

void Renderer::Submit(const std::string& image, const Vector2& position, Color color, int sortingOrder)
{
	RenderCommand command;

	command.image = image;
	command.position = position;
	command.color = color;
	command.sortingOrder = sortingOrder;

	m_renderCommands.push_back(command);
}

void Renderer::Draw()
{
	m_frame->Clear(m_screenSize);
	DrawRenderCommands();
	Present();
}

void Renderer::DrawRenderCommands()
{
	for (auto element : m_renderCommands) 
	{
		if (element.position.GetY() < 0 || element.position.GetY() >= m_screenSize.GetY())
		{
			continue;
		}

		for (int i = 0; i < element.image.size(); i++) //글자
		{
			if (element.position.GetX() + i < 0 || element.position.GetX() + i >= m_screenSize.GetX())
			{
				continue;
			}

			if (m_frame->sortingOrderArray[element.position.GetY() * m_screenSize.GetX() + element.position.GetX() + i] > element.sortingOrder)
			{
				continue;
			}

			m_frame->charInfoArray[element.position.GetY() * m_screenSize.GetX() + element.position.GetX() + i].Char.AsciiChar = element.image[i];
			m_frame->charInfoArray[element.position.GetY() * m_screenSize.GetX() + element.position.GetX() + i].Attributes = static_cast<WORD>(element.color);
			m_frame->sortingOrderArray[element.position.GetY() * m_screenSize.GetX() + element.position.GetX() + i] = element.sortingOrder;
		}
	}

	m_screenBuffers[m_backBufferIndex]->Draw(m_frame->charInfoArray.get());
	m_renderCommands.clear();
}

void Renderer::Present()
{
	BOOL result = SetConsoleActiveScreenBuffer(m_screenBuffers[m_backBufferIndex]->GetHandle());
	assert(result == TRUE);

	m_backBufferIndex = 1 - m_backBufferIndex;
}
