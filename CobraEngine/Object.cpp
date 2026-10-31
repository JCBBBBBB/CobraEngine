#include "Object.h"

Object::Object()
{
}

Object::~Object()
{
}

bool Object::Is(std::size_t id) const
{
	// Object 자체에는 타입 ID를 등록하지 않고, 부모로 올라온 검사를 종료한다
	return false;
}
