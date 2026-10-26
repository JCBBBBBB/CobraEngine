#include "Vector2.h"
#include <cassert>

Vector2 Vector2::Zero(0, 0);
Vector2 Vector2::One(1,1);
Vector2 Vector2::Right(1,0);
Vector2 Vector2::Left(-1,0);
Vector2 Vector2::Up(0,-1);
Vector2 Vector2::Down(0,1);


Vector2::Vector2() : m_x(0), m_y(0)
{

}

Vector2::Vector2(int x, int y)
{
	m_x = x;
	m_y = y;
}

Vector2::~Vector2()
{
}

Vector2 Vector2::operator+(const Vector2& other) const
{
	return Vector2(this->m_x + other.m_x, this->m_y + other.m_y);
}

Vector2 Vector2::operator-(const Vector2& other) const
{
	/*Vector2 previous(3, 4);
	Vector2 next(5, 6);

	Vector2 position = next - previous*/

	return Vector2(this->m_x - other.m_x, this->m_y - other.m_y);
}

Vector2 Vector2::operator*(const Vector2& other) const
{
	return Vector2(this->m_x * other.m_x, this->m_y * other.m_y);
}

Vector2 Vector2::operator*(int value) const
{
	return Vector2(this->m_x * value, this->m_y * value);
}

Vector2 Vector2::operator/(const Vector2& other) const
{
	assert((other.m_x != 0) && (other.m_y != 0)); // 나누는 수가 0이면 안됨

	return Vector2(this->m_x / other.m_x, this->m_y / other.m_y);
}

bool Vector2::operator==(const Vector2& other) const
{
	return (this->m_x == other.m_x) && (this->m_y == other.m_y); // x와 y좌표 둘다 같아야 -> 같은 Vector2
}

bool Vector2::operator!=(const Vector2& other) const
{
	return (this->m_x != other.m_x) || (this->m_y != other.m_y); // x와 y좌표 중 하나라도 다르면 -> 다른 Vector2
}

Vector2& Vector2::operator=(const Vector2& other) // 대입
{
	this->m_x = other.m_x;
	this->m_y = other.m_y;

	return *this;
}
