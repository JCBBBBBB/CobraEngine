#include "Actor.h"
#include "Renderer/Renderer.h"


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

void Actor::Draw()
{
	if (!IsActive())
	{
		return;
	}

	// 실제 출력은 Renderer가 모아서 처리한다
	Renderer::Get().Submit(m_image, m_position, m_color, m_sortingOrder);
}

void Actor::Destroy()
{
	// 객체 삭제가 아니라 제거 요청을 표시한다
	m_hasExpired = true;
}

void Actor::SetPosition(const Vector2& newPosition)
{
	if (m_position == newPosition)
	{
		return;
	}

	m_position = newPosition;
}