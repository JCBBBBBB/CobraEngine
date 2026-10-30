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

	virtual std::size_t GetType() const = 0;
	virtual bool Is(std::size_t id) const;
	

	template<typename T>
	bool IsTypeOf() const { return Is(T::TypeId()); }

	template<typename T, typename U>
	std::shared_ptr<T> Cast(const std::shared_ptr<U>& object)
	{
		if (object == nullptr)
		{
			return nullptr;
		}

		if (object->Is(T::TypeId()))
		{
			return std::static_pointer_cast<T>(object)
		}
		else
		{
			return nullptr;
		}
	}
};