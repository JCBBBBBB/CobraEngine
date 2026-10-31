#include "Actor/Actor.h"

#include <Windows.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

using ActorList = std::vector<std::shared_ptr<Actor>>;
using RemoveFunction = void (*)(ActorList&);

// 수정 전: 제거할 때마다 뒤 원소들을 당긴다.
// 두 비교 함수의 인라이닝 조건을 맞춘다.
__declspec(noinline)
void RemoveBefore(ActorList& actors)
{
	for (auto it = actors.begin(); it != actors.end();)
	{
		if ((*it)->HasExpired())
		{
			it = actors.erase(it);
			continue;
		}

		++it;
	}
}

// 수정 후: 생존 원소를 모으고 뒤 구간을 한 번에 제거한다.
__declspec(noinline)
void RemoveAfter(ActorList& actors)
{
	const auto newEnd = std::remove_if(
		actors.begin(),
		actors.end(),
		[](const std::shared_ptr<Actor>& actor)
		{
			return actor->HasExpired();
		}
	);

	actors.erase(newEnd, actors.end());
}

// 목록 준비 → 제거 시간 측정 → 결과 검증.
double MeasureOnce(
	int actorCount,
	int removalPercent,
	RemoveFunction removeFunction,
	LONGLONG frequency)
{
	ActorList actors;
	actors.reserve(actorCount);

	// 검증용 주소만 저장한다.
	// shared_ptr가 아니므로 Actor의 소멸을 막지 않는다.
	std::vector<const Actor*> expectedSurvivors;
	expectedSurvivors.reserve(actorCount);

	// 준비 과정은 측정 시간에 포함하지 않는다.
	for (int i = 0; i < actorCount; ++i)
	{
		auto actor = std::make_shared<Actor>();

		const bool shouldRemove =
			removalPercent == 100 ||
			(removalPercent == 50 && i % 2 == 1);

		if (shouldRemove)
		{
			actor->Destroy();
		}
		else
		{
			expectedSurvivors.push_back(actor.get());
		}

		actors.push_back(std::move(actor));
	}

	LARGE_INTEGER start = {};
	LARGE_INTEGER end = {};

	QueryPerformanceCounter(&start);

	// 이 호출만 측정한다.
	removeFunction(actors);

	QueryPerformanceCounter(&end);

	// 검증 과정도 측정 시간에 포함하지 않는다.
	if (actors.size() != expectedSurvivors.size())
	{
		throw std::runtime_error("Survivor count mismatch.");
	}

	for (std::size_t i = 0; i < actors.size(); ++i)
	{
		if (actors[i].get() != expectedSurvivors[i])
		{
			throw std::runtime_error("Survivor order mismatch.");
		}
	}

	return static_cast<double>(end.QuadPart - start.QuadPart)
		* 1000.0
		/ static_cast<double>(frequency);
}

// 측정값을 정렬한 후 가운데 값을 구한다.
double Median(std::vector<double> values)
{
	std::sort(values.begin(), values.end());

	const std::size_t middle = values.size() / 2;

	if (values.size() % 2 == 0)
	{
		return (values[middle - 1] + values[middle]) / 2.0;
	}

	return values[middle];
}

struct Sample
{
	int actorCount;
	int removalPercent;
	int trial;

	double beforeMs;
	double afterMs;
};

int main()
{
	try
	{
		LARGE_INTEGER frequency = {};

		if (!QueryPerformanceFrequency(&frequency) ||
			frequency.QuadPart <= 0)
		{
			throw std::runtime_error("Timer initialization failed.");
		}

		constexpr int warmupCount = 5;
		constexpr int repeatCount = 30;

		const int actorCounts[] = { 1000, 10000 };
		const int removalPercents[] = { 0, 50, 100 };

		std::vector<Sample> samples;
		samples.reserve(2 * 3 * repeatCount);

		std::cout << std::fixed << std::setprecision(6);

		for (int actorCount : actorCounts)
		{
			for (int removalPercent : removalPercents)
			{
				// 초기 실행의 영향을 줄이기 위한 준비 실행.
				for (int i = 0; i < warmupCount; ++i)
				{
					MeasureOnce(
						actorCount, removalPercent,
						RemoveBefore, frequency.QuadPart
					);

					MeasureOnce(
						actorCount, removalPercent,
						RemoveAfter, frequency.QuadPart
					);
				}

				std::vector<double> beforeTimes;
				std::vector<double> afterTimes;

				beforeTimes.reserve(repeatCount);
				afterTimes.reserve(repeatCount);

				for (int trial = 0; trial < repeatCount; ++trial)
				{
					double beforeMs = 0.0;
					double afterMs = 0.0;

					// 항상 같은 방식이 먼저 실행되지 않도록 번갈아 측정.
					if (trial % 2 == 0)
					{
						beforeMs = MeasureOnce(
							actorCount, removalPercent,
							RemoveBefore, frequency.QuadPart
						);

						afterMs = MeasureOnce(
							actorCount, removalPercent,
							RemoveAfter, frequency.QuadPart
						);
					}
					else
					{
						afterMs = MeasureOnce(
							actorCount, removalPercent,
							RemoveAfter, frequency.QuadPart
						);

						beforeMs = MeasureOnce(
							actorCount, removalPercent,
							RemoveBefore, frequency.QuadPart
						);
					}

					beforeTimes.push_back(beforeMs);
					afterTimes.push_back(afterMs);

					samples.push_back({
						actorCount,
						removalPercent,
						trial + 1,
						beforeMs,
						afterMs
						});
				}

				std::cout
					<< "Actors=" << actorCount
					<< ", Remove=" << removalPercent << "%"
					<< ", Before=" << Median(beforeTimes) << " ms"
					<< ", After=" << Median(afterTimes) << " ms"
					<< '\n';
			}
		}

		// 모든 측정이 끝난 뒤 파일에 저장한다.
		std::ofstream file("ActorRemovalBenchmark.csv");

		if (!file)
		{
			throw std::runtime_error("Cannot create CSV file.");
		}

		file << "actor_count,removal_percent,trial,before_ms,after_ms\n";
		file << std::fixed << std::setprecision(6);

		for (const Sample& sample : samples)
		{
			file
				<< sample.actorCount << ','
				<< sample.removalPercent << ','
				<< sample.trial << ','
				<< sample.beforeMs << ','
				<< sample.afterMs << '\n';
		}

		std::cout
			<< "\nAll count/order checks passed.\n"
			<< "Saved: ActorRemovalBenchmark.csv\n";
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		return 1;
	}

	return 0;
}