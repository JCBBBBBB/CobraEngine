#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Actor/Actor.h"
#include "Core/Input.h"

#include <Windows.h>
#include <memory>
#include <string>

// 입력에 따라 이동하고 표식을 생성하는 테스트용 Player.
class Player : public Actor
{
	TYPE_DECLARATIONS(Player, Actor)

public:
	Player()
		: Actor("@", Vector2(3, 3), Color::Yellow)
	{
		// 바닥과 표식보다 위에 표시한다.
		m_sortingOrder = 1;
	}

	void Tick(float deltaTime) override
	{
		Input& input = Input::Get();

		// 종료 요청.
		if (input.GetKeyDown(VK_ESCAPE))
		{
			QuitGame();
			return;
		}

		// 방향키를 새로 누를 때마다 한 칸 이동한다.
		Vector2 nextPosition = GetPosition();

		if (input.GetKeyDown(VK_LEFT))
		{
			nextPosition = nextPosition + Vector2::Left;
		}

		if (input.GetKeyDown(VK_RIGHT))
		{
			nextPosition = nextPosition + Vector2::Right;
		}

		if (input.GetKeyDown(VK_UP))
		{
			nextPosition = nextPosition + Vector2::Up;
		}

		if (input.GetKeyDown(VK_DOWN))
		{
			nextPosition = nextPosition + Vector2::Down;
		}

		// 위 두 줄은 안내 문구 영역으로 남긴다.
		// 화면 안의 좌표일 때만 위치를 변경한다.
		if (nextPosition.GetX() >= 0 &&
			nextPosition.GetX() < Engine::Get().GetWidth() &&
			nextPosition.GetY() >= 2 &&
			nextPosition.GetY() < Engine::Get().GetHeight())
		{
			SetPosition(nextPosition);
		}

		// 표식이 없으면 현재 위치에 하나 생성한다.
		if (input.GetKeyDown(VK_SPACE) && m_marker.expired())
		{
			std::shared_ptr<Level> level = GetOwner();

			if (level)
			{
				m_marker = level->SpawnActor<Actor>(
					"*",
					GetPosition(),
					Color::Cyan
				);
			}
		}

		// 현재 표식을 제거 예약한다.
		if (input.GetKeyDown('X'))
		{
			std::shared_ptr<Actor> marker = m_marker.lock();

			if (marker)
			{
				marker->Destroy();
				m_marker.reset();
			}
		}
	}

private:
	// 표식의 실제 소유자는 Level이다.
	// Player는 표식을 찾아갈 약한 참조만 보관한다.
	std::weak_ptr<Actor> m_marker;
};

// 테스트 장면을 구성하는 Level.
class GameLevel : public Level
{
public:
	void OnInitialized() override
	{
		// 레벨 초기화 완료 상태를 기록한다.
		Level::OnInitialized();

		SpawnActor<Player>();

		SpawnActor<Actor>(
			"CobraEngine Integration Test",
			Vector2(1, 0),
			Color::White
		);

		SpawnActor<Actor>(
			"Arrows: Move | Space: Mark | X: Remove | ESC: Quit",
			Vector2(1, 1),
			Color::White
		);

		// Player와 겹치는 바닥.
		// 기본 우선순위는 0이므로 Player를 가리지 않는다.
		SpawnActor<Actor>(
			std::string(Engine::Get().GetWidth(), '.'),
			Vector2(0, 3),
			Color::Green
		);
	}
};

int main()
{
	// Input과 Renderer를 생성한다.
	Engine engine;

	// 첫 번째로 실행할 레벨을 요청한다.
	engine.AddNewLevel<GameLevel>();

	// 입력·업데이트·그리기를 반복한다.
	engine.Run();

	return 0;
}