#pragma once
#include "Core/Core.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "ScreenBuffer.h"
#include <memory>
#include <string>
#include <vector>

class COBRA_API Renderer
{
public:
	Renderer(int width, int height);
	~Renderer();

	static Renderer& Get();
	void Submit(const std::string& image, const Vector2& position, Color color, int sortingOrder = 0);
	void Draw();

private:
	// 그리기 요청 하나의 정보를 담는 구조체
	struct RenderCommand
	{
		std::string image; // 무엇을 그릴지
		Vector2 position; // 어디에 그릴지
		Color color = Color::White; // 어떤 색으로 그릴지
		int sortingOrder = 0; // 무엇을 더 우선할징ㅇㅇㅇㅇㅇ
	};

	// 화면 각 칸의 최종 문자/색상과 그리기 우선순위를 관리하는 구조체
	struct Frame
	{
		Frame(int area);
		void Clear(const Vector2& screenSize);

		std::unique_ptr<CHAR_INFO[]> charInfoArray;
		std::unique_ptr<int[]> sortingOrderArray;
	};

private:
	void DrawRenderCommands();
	void Present();

private:
	Vector2 m_screenSize; // 화면 사이즈
	int m_backBufferIndex = 1; // 다음 화면을 작성할 버퍼의 배열 인덱스 
	static Renderer* m_instance;

	std::unique_ptr<ScreenBuffer> m_screenBuffers[2]; // 0번 : 지금 보여주는 화면, 1번 : 다음 화면을 그릴 공간
	std::unique_ptr<Frame> m_frame; // 최종 화면 데이터를 구성할 Frame 객체를 소유

	std::vector<RenderCommand> m_renderCommands; // 이번 프레임에 처리할 그리기 요청 목록
};

