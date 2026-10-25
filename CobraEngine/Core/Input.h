#pragma once
#include "Core.h"


class COBRA_API Input
{
	// 호출 시점은 엔진이 관리하도록
	// 엔진이 private 함수 접근할 수 있게 함
	friend class Engine;

public:
	Input();
	~Input();

	bool GetKey(int keyCode) const; // 지금 눌려 있나?
	bool GetKeyDown(int keyCode) const; // 안 눌렸다가 눌렸나?
	bool GetKeyUp(int keyCode) const; // 눌렸다가 놓았나?

	static Input& Get(); // Input& : 기존 객체를 그대로 사용할 수 있게 연결해주는 것(새 객체나 복사본이 아님)

private:
	void ProcessInput();
	void SavePreviousStates(); // 다음 프레임을 위한 이전 상태 저장

	Input(const Input&) = delete; // 새 객체 복사해서 만들기 막음, Input b = a
	Input& operator=(const Input&) = delete; // 이미 있는 객체에 복사하기 막음, a = b
private:

	struct KeyState
	{
		bool isPressed = false; // 현재 누름
		bool wasPressed = false; // 과거에 누름
	};

private:
	KeyState m_keyState[256];
	static Input* m_instance; // Input 객체의 주소를 저장할 보관함
};