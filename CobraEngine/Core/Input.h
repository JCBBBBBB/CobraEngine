#pragma once
#include "Core.h"


class COBRA_API Input
{
public:
	bool GetKey(int keyCode) const; // 지금 눌려 있나?
	bool GetKeyDown(int keyCode) const; // 안 눌렸다가 눌렸나?
	bool GetKeyUp(int keyCode) const; // 눌렸다가 놓았나?


private:

	struct KeyState
	{
		bool isPressed = false; // 현재 누름
		bool wasPressed = false; // 과거에 누름
	};

	KeyState m_keyState[256];
};