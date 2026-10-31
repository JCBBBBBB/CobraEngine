#include "Actor.h"
#include "Renderer/Renderer.h"

// 위치/문자/색상 저장
Actor::Actor(const std::string& image, const Vector2& position, Color color)
	: m_image(image), m_position(position), m_color(color), m_width(static_cast<int>(image.length()))
{

}

Actor::~Actor()
{

}

void Actor::BeginPlay()
{
	m_hasBeganPlay = true; // 초기화 이벤트를 처리했다는 사실을 기록한다
}

void Actor::Tick(float deltaTime)
{
	// 구체적인 업데이트 동작은 자식 클래스에서 구현한다
}

// Actor가 저장한 정보를 Renderer에 전달
void Actor::Draw()
{
	if (!IsActive())
	{
		return;
	}

	// Actor가 저장한 정보를 Renderer에 전달
	Renderer::Get().Submit(m_image, m_position, m_color, m_sortingOrder);
}

// 바로 Destroy하지 않는 이유
// -> 순회 중인 목록을 즉시 변경하면 반복 처리 꼬일 수 있음
void Actor::Destroy()
{
	// 객체 삭제가 아니라 제거 요청을 표시한다
	m_hasExpired = true;
}

std::shared_ptr<Level> Actor::GetOwner()
{
	// Level이 살아있음 -> shared_ptr<Level>
	// Level이 소멸했거나 미등록 -> 빈 shared_ptr
	return m_owner.lock(); // weak_ptr은 연결대상이 이미 소멸했을 수도 있어서, 바로 객체처럼 사용할 수 없음
}

// 자신을 관리하는 Level을 등록한다
void Actor::SetOwner(std::weak_ptr<Level> newOwner)
{
	m_owner = newOwner;
}

void Actor::SetPosition(const Vector2& newPosition)
{
	if (m_position == newPosition)
	{
		return;
	}

	m_position = newPosition;
}