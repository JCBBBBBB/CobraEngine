#include "Input.h"

bool Input::GetKey(int keyCode) const
{
	return m_keyState[keyCode].isPressed;
}

bool Input::GetKeyDown(int keyCode) const
{
	return m_keyState[keyCode].isPressed && !m_keyState[keyCode].wasPressed;
}

bool Input::GetKeyUp(int keyCode) const
{
	return m_keyState[keyCode].wasPressed && !m_keyState[keyCode].isPressed;
}
