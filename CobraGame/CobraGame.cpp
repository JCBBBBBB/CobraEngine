// CobraGame.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "Actor/Actor.h"
#include "Renderer//Renderer.h"

#include <cassert>
#include <memory>
#include <iostream>

class TestPlayer : public Actor
{
	TYPE_DECLARATIONS(TestPlayer, Actor)

public:
	TestPlayer() : Actor("@", Vector2(3, 2), Color::Blue)
	{
		m_sortingOrder = 1;
	}

	void BeginPlay() override
	{
		// 부모가 초기화 완료 여부를 기록하도록 호출
		super::BeginPlay();
	}

	void Tick(float deltaTime) override
	{

		SetPosition(GetPosition() + Vector2::Right);
	}
};


int main()
{
	

	return 0;
}