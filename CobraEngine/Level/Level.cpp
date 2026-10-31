#include "Level.h"

Level::Level()
{

}

Level::~Level()
{

}

void Level::OnInitialized()
{
	m_hasInitialized = true;
}


void Level::BeginPlay()
{
	for (const std::shared_ptr<Actor>& actor : m_actors)
	{
		// 이미 초기화한 Actor는 건너뛴다
		if (actor->HasBeganPlay())
		{
			continue;
		}

		actor->BeginPlay();
	}
}


void Level::Tick(float deltaTime)
{
	for (const std::shared_ptr<Actor>& actor : m_actors)
	{
		// 비활성화되었거나 제거 예약된 Actor는 건너뛴다
		if (!actor->IsActive())
		{
			continue;
		}

		actor->Tick(deltaTime);
	}
}

void Level::Draw()
{
	for (const std::shared_ptr<Actor>& actor : m_actors)
	{
		// 비활성화되었거나 제거 예약된 Actor는 건너뛴다
		if (!actor->IsActive())
		{
			continue;
		}

		actor->Draw();
	}
}





void Level::ProcessAddAndDestroyActors()
{
	// 제거 예약된 Actor를 관리 목록에서 뺀다
	//for (auto iter = m_actors.begin(); iter != m_actors.end();)
	//{
	//	if ((*iter)->HasExpired())
	//	{
	//		// erase가 반환한 다음 원소 위치부터 계속 검사한다
	//		iter = m_actors.erase(iter);
	//		continue; // 바로 그 위치부터 검사 가능
	//	}

	//	++iter;
	//}
	const auto newEnd = std::remove_if(
		m_actors.begin(),
		m_actors.end(),
		[](const std::shared_ptr<Actor>& actor)
		{
			return actor->HasExpired();
		}
	);

	m_actors.erase(newEnd, m_actors.end());

	// 추가 요청된 Actor를 관리 목록에 등록한다
	for (const std::shared_ptr<Actor>& actor : m_addRequestedActors)
	{
		m_actors.push_back(actor);
	}

	// 등록을 마친 요청 목록을 비운다
	m_addRequestedActors.clear();
}