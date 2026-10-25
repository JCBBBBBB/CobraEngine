#pragma once

#pragma warning(disable: 4251) //Visual Studio의 C++ 컴파일러에 C4251 경고를 표시하지 말라고 지시합니다.

#define DLLEXPORT __declspec(dllexport) // dllexport : DLL 내부에 있는 기능을 외부에서도 사용할 수 있게 공개(클래스, 함수) -> 엔진
#define DLLIMPORT __declspec(dllimport) // dllimport : DLL 내부에 있는 기능을 사용할거야

#if defined(COBRA_ENGINE_BUILD_DLL) // 프로젝트 설정에 COBRA_ENGINE_BUILD_DLL이 정의되어 있는지 확인(컴파일 전 전처리 단계에서 결정)
#define COBRA_API DLLEXPORT // 엔진 만드는 쪽이라면 공개 표시 선택
#else
#define COBRA_API DLLIMPORT // 엔진 사용하는 쪽이라면 가져오기 표시 선택 -> 게임 프로젝트는 COBRA_ENGINE_BUILD_DLL 정의하지 않음
#endif