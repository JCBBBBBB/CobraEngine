
#include "Engine.h"

#include "Level/Level.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

#include <Windows.h>
#include <cassert>
#include <cstdint>
#include <utility>

Engine* Engine::m_instance = nullptr;

Engine::Engine()
{
	assert(m_instance == nullptr);
	m_instance = this;

	// 입력 시스템 생성
	m_input = std::make_unique<Input>();

	// 렌더링 시스템 생성
	m_renderer = std::make_unique<Renderer>(m_setting.width, m_setting.height);
}

Engine::~Engine()
{
	Shutdown();
	if (m_instance)
	{
		m_instance = nullptr;
	}
}

// 게임 루프 실행
void Engine::Run()
{
	// 타이머가 1초에 세는 횟수
	LARGE_INTEGER frequency = {};
	QueryPerformanceFrequency(&frequency);

	// 현재 타이머의 누적 값
	LARGE_INTEGER counter = {};
	QueryPerformanceCounter(&counter); // 현재 타이머의 누적 값

	std::int64_t previousTime = counter.QuadPart;

	// 목표 프레임 간격 -> 1 / 60 FPS
	const float oneFrameTime = 1.0f / m_setting.frameRate;

	while (!m_isQuit)
	{
		// 현재 키 상태 갱신
		ProcessInput();

		// 현재 시점 확인
		QueryPerformanceCounter(&counter);
		const std::int64_t currentTime = counter.QuadPart;

		// 이전 프레임 이후 경과 시간을 초 단위로 계산
		const float deltaTime = static_cast<float>(currentTime - previousTime) / static_cast<float>(frequency.QuadPart);
	
		// 목표 간격이 지나지 않았으면 시간을 다시 확인
		if (deltaTime < oneFrameTime)
		{
			continue;
		}

		// 레벨 자체 초기화
		OnInitialized();

		// 새로 등록된 Actor를 초기화
		BeginPlay();

		// 게임 상태 변경
		Tick(deltaTime);

		// 변경된 상태로 화면 구성과 표시
		Draw();

		// 현재 프레임의 이벤트 처리가 끝난 뒤 레벨 전환
		if (m_nextLevel)
		{
			m_mainLevel = std::move(m_nextLevel);
		}

		// 순회가 끝났으므로 예약된 Actor 추가/제거를 반영
		if (m_mainLevel)
		{
			m_mainLevel->ProcessAddAndDestroyActors();
		}

		// 게임의 입력 판단이 끝난 뒤 이전 상태를 저장한다
		SavePreviousInputStates();

		// 이번 프레임의 기준 시점을 다음 계산에 사용한다
		previousTime = currentTime;

	}

	Shutdown();


}

// 게임 루프 종료 요청
void Engine::Quit()
{
	// 즉시 객체를 삭제하지 않고 반복문 종료 요청
	m_isQuit = true;
}

// 현재 생성된 Engine에 접근
Engine& Engine::Get()
{
	assert(m_instance);
	return *m_instance;
}


void Engine::ProcessInput()
{
	assert(m_input);
	m_input->ProcessInput();
}

void Engine::OnInitialized()
{
	if (!m_mainLevel)
	{
		return;
	}

	if (m_mainLevel->HasInitialized())
	{
		return;
	}

	m_mainLevel->OnInitialized();
}

void Engine::BeginPlay()
{
	if (!m_mainLevel)
	{
		return;
	}

	m_mainLevel->BeginPlay();
}

void Engine::Tick(float deltaTime)
{
	if (!m_mainLevel)
	{
		return;
	}

	m_mainLevel->Tick(deltaTime);
}

void Engine::Draw()
{
	if (!m_mainLevel)
	{
		return;
	}

	// Actor들이 Renderer에 그리기를 요청한다
	m_mainLevel->Draw();

	// 모인 요청으로 화면을 완성하고 표시한다
	m_renderer->Draw();
}

void Engine::SavePreviousInputStates()
{
	assert(m_input);
	m_input->SavePreviousStates();
}

void Engine::Shutdown()
{
	// 게임 객체들을 먼저 정리한다
	m_nextLevel.reset();
	m_mainLevel.reset();

	// 그 다음 화면과 입력시스템을 정리한다
	m_renderer.reset();
	m_input.reset();
}