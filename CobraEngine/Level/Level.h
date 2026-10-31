#pragma once

#include "Core/Core.h"
#include "Actor/Actor.h"

#include <memory>
#include <vector>
#include <type_traits>
#include <utility>

// Actor들을 소유하고 프레임 이벤트를 전달하는 장면의 기반 클래스
class COBRA_API Level : public std::enable_shared_from_this<Level>
{
	// Actor 추가/제거/처리 시점은 Engine이 관리
	friend class Engine;

public:
	Level();
	virtual ~Level();

	// 레벨 자체를 초기화하는 이벤트
	virtual void OnInitialized();

	// 아직 초기화하지 않은 Actor에 초기화 이벤트 전달
	virtual void BeginPlay();

	// 활성 Actor에 업데이트 이벤트 전달
	virtual void Tick(float deltaTime);

	// 활성 Actor에 그리기 이벤트 전달
	virtual void Draw();

	// Actor 또는 Actor를 상속한 클래스만 생성가능
	// is_base_if (T가 Actor이거나 Actor 자식인지 확인)
	// Args -> 여러개의 인자받아 해당 클래스의 생성자로 전달
	template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of<Actor, T>::value>> 
	std::shared_ptr<T> SpawnActor(Args&&... args)
	{
		// 전달받은 인자를 T의 생성자로 전달한다
		std::shared_ptr<T> newActor = std::make_shared<T>(std::forward<Args>(args)...);

		// 생성된 Actor가 자신을 관리하는 Level을 기억하도록 연결한다
		newActor->SetOwner(shared_from_this()); // shared_from_this -> 현재 Level의 기존 소유권을 공유하는 shared_ptr 얻는다

		// 현재 순회 중일 수 있으므로 관리 목록에 바로 추가하지 않는다
		m_addRequestedActors.push_back(newActor);

		return newActor;
	}
	
	bool HasInitialized() const { return m_hasInitialized; }

protected:
	// 예약된 제거와 추가를 관리 목록에 반영
	void ProcessAddAndDestroyActors(); // void UpdateActorList로 나중에 변경


protected:
	bool m_hasInitialized = false;
	std::vector<std::shared_ptr<Actor>> m_actors; // 현재 레벨에 등록된 Actor 목록
	std::vector<std::shared_ptr<Actor>> m_addRequestedActors; // 안전한 처리 시점에 등록할 Actor 목록
};