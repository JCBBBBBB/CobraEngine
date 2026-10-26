#pragma once
#include "Core/Core.h"

class COBRA_API Vector2 // 게임 프로젝트에서 사용할 클래스이므로 DLL 공개 매크로 적용
{
public:
	Vector2();
	Vector2(int x, int y);
	~Vector2();

	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator*(const Vector2& other) const;
	Vector2 operator*(int value) const;
	Vector2 operator/(const Vector2& other) const;

	bool operator==(const Vector2& other) const;
	bool operator!=(const Vector2& other) const;
	Vector2& operator=(const Vector2& other); // 새로운 결과 객체를 만들어 반환하지 않고, 이미 존재하는 객체 자체를 참조로 돌려줌

	int GetX() const { return m_x; } // const 붙인 이유 : 함수 안에서 m_x를 변경 못하게
	int GetY() const { return m_y; } // const 붙인 이유 : 함수 안에서 m_y를 변경 못하게


	//static인 이유는 Right, Up 같은 값이 개별 객체의 상태가 아니라, 모든 코드가 함께 사용할 공통 기준값이기 때문
	static Vector2 Zero;
	static Vector2 One;
	static Vector2 Right;
	static Vector2 Left;
	static Vector2 Up;
	static Vector2 Down;

private:
	int m_x;
	int m_y;
};

