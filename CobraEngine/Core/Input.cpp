#include "Input.h"
#include <Windows.h>
#include <cassert>

Input* Input::m_instance = nullptr;


Input::Input()
{
	assert(m_instance == nullptr);
	m_instance = this; // 현재 객체 주소 저장, this는 이 객체의 주소
}

Input::~Input()
{
	m_instance = nullptr;
}

bool Input::GetKey(int keyCode) const
{
	if (keyCode < 0 || keyCode >= 256)
	{
		return false;
	}

	return m_keyState[keyCode].isPressed;
}

bool Input::GetKeyDown(int keyCode) const
{
	if (keyCode < 0 || keyCode >= 256)
	{
		return false;
	}

	return m_keyState[keyCode].isPressed && !m_keyState[keyCode].wasPressed;
}

bool Input::GetKeyUp(int keyCode) const
{
	if (keyCode < 0 || keyCode >= 256)
	{
		return false;
	}

	return m_keyState[keyCode].wasPressed && !m_keyState[keyCode].isPressed;
}

Input& Input::Get()
{
	assert(m_instance);

	return *m_instance; // m_instance가 가리키는 객체 자체 반환
}

void Input::ProcessInput()
{
	for (int i = 0; i < 256; i++)
	{
		if (GetAsyncKeyState(i) & 0x8000) // 0x8000은 현재 눌렸는지 확인하는 것
		{
			m_keyState[i].isPressed = true;
		}
		else
		{
			m_keyState[i].isPressed = false;
		}
	}
}

void Input::SavePreviousStates()
{
	for (int i = 0; i < 256; i++)
	{
		m_keyState[i].wasPressed = m_keyState[i].isPressed;

	}
}
