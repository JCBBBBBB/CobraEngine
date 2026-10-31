#pragma once

#include "Core/Core.h"
#include <cstddef>
#include <memory>

// 추상 클래스 -> Object 객체 자체를 직접 생성 불가
class COBRA_API Object
{
public:
	Object();
	virtual ~Object();

	// 실제 객체의 타입 ID를 반환한다.
	// 구체적인 자식 클래스가 반드시 구현해야 한다
	virtual std::size_t GetType() const = 0;

	// 해당 타입 계열인지 검사한다.
	// 자식에서 일치하지 않은 검사가 여기까지 오면 false로 끝난다
	virtual bool Is(std::size_t id) const;
	
	// 타입 이름을 해당 타입의 ID로 바꿔 검사한다
	template<typename T>
	bool IsTypeOf() const { return Is(T::TypeId()); }

	// 실제 타입을 확인한 뒤, 같은 객체를 가리키는 스마트 포인터로 변환한다
	template<typename T, typename U>
	std::shared_ptr<T> Cast(const std::shared_ptr<U>& object)
	{
		if (object == nullptr)
		{
			return nullptr;
		}

		if (object->Is(T::TypeId()))
		{
			return std::static_pointer_cast<T>(object);
		}
		else
		{
			return nullptr;
		}
	}
};

// 자식 클래스마다 반복되는 타입 관련 코드를 생성한다.
// Type: 현재 클래스 이름.
// ParentType: 직접 상속한 부모 클래스 이름.
#define TYPE_DECLARATIONS(Type, ParentType)                 \
	using super = ParentType;                              \
protected:                                                 \
	static std::size_t TypeIdClass()                        \
	{                                                      \
		static int runtimeTypeId = 0;                       \
		return reinterpret_cast<std::size_t>(&runtimeTypeId);\
	}                                                      \
public:                                                    \
	static std::size_t TypeId()                             \
	{                                                      \
		return Type::TypeIdClass();                         \
	}                                                      \
	std::size_t GetType() const override                    \
	{                                                      \
		return Type::TypeIdClass();                         \
	}                                                      \
	bool Is(std::size_t id) const override                  \
	{                                                      \
		if (id == Type::TypeIdClass())                      \
		{                                                  \
			return true;                                   \
		}                                                  \
		return ParentType::Is(id);                          \
	}