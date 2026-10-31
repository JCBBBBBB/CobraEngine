#pragma once

#include "Core/Core.h"

#include <memory>
#include <type_traits>

class Level;
class Input;
class Renderer;

class COBRA_API Engine
{
	struct EngineSetting
	{
		float frameRate = 60.0f;
		int width = 60;
		int height = 25;
	};

public:
	Engine();
	virtual ~Engine();

	// 게임 루프 실행
	void Run();

	// 게임 루프 종료 요청
	void Quit();

	// 현재 생성된 Engine에 접근
	static Engine& Get();

	// Level 또는 Level의 자식 클래스만 등록 가능
	template<typename T, typename = std::enable_if_t<std::is_base_of<Level, T>::value>>
	void AddNewLevel()
	{
		m_nextLevel = std::make_shared<T>();
	}

	int GetWidth() const { return m_setting.width; }
	int GetHeight() const { return m_setting.height; }

protected:
	void ProcessInput();
	void OnInitialized();
	void BeginPlay();
	void Tick(float deltaTime);
	void Draw();
	void SavePreviousInputStates();
	void Shutdown();

protected:
	EngineSetting m_setting;

	bool m_isQuit = false;
	static Engine* m_instance;

	std::shared_ptr<Level> m_mainLevel; // 현재 실행중인 레벨
	std::shared_ptr<Level> m_nextLevel; // 프레임 처리가 끝나면 전환할 레벨

	// Engine이 수명을 관리하는 시스템
	std::unique_ptr<Input> m_input;
	std::unique_ptr<Renderer> m_renderer;
};