#pragma once

#include "Core/Core.h"
#include "Object.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include <string>
#include <memory>

class Level;
// 위치, 모양, 상태와 프레임 이벤트를 제공하는 게임 객체의 기반 클래스
class COBRA_API Actor : public Object
{
	TYPE_DECLARATIONS(Actor, Object);

public:
	Actor(const std::string& image = "", const Vector2& position = Vector2::Zero, Color color = Color::White);
	~Actor() override;

	// 게임에 참여하기 시작할 때 처리할 초기화
	virtual void BeginPlay();

	// 매 프레임의 상태 변경
	virtual void Tick(float deltaTime);

	// 이번 프레임의 그리기 요청 전달
	virtual void Draw();

	// 즉시 삭제하지 않고 제거 대상으로 표시
	void Destroy();

	// 자신을 관리하는 Level을 얻는다
	// Level이 없거나 이미 소멸했다면 nullptr을 반환한다
	std::shared_ptr<Level> GetOwner();

	// 자신을 관리하는 Level을 등록한다
	void SetOwner(std::weak_ptr<Level> newOwner);

	bool HasBeganPlay() const { return m_hasBeganPlay; }
	bool IsActive() const { return m_isActive && !m_hasExpired; }
	bool HasExpired() const { return m_hasExpired; }
	Vector2 GetPosition() const { return m_position; }
	void SetPosition(const Vector2& newPosition);

protected:
	bool m_hasBeganPlay = false; // BeginPlay 처리 여부
	bool m_isActive = true; // 업데이트와 그리기에 참여할지 여부
	bool m_hasExpired = false; // 제거 예약 여부
	std::string m_image; // 화면에 출력할 문자열
	Color m_color = Color::White; // 글자 색상
	int m_width = 0; // 문자열 길이
	int m_sortingOrder = 0; // 겹치는 칸에서의 그리기 우선순위
	Vector2 m_position; // 문자열을 출력할 시작 위치

	std::weak_ptr<Level> m_owner; // 자신을 관리하는 Level에 대한 약한 참조
};

