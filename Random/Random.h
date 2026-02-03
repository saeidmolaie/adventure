#pragma once

#include <vector>
#include <cstdint>
#include <chrono>

class Random
{

private:
	static constexpr int MaxIntValue = 2147483647;
	static constexpr int SeedConstant = 161803398;

	int currentIndex = 0;
	int secondaryIndex = 21;
	int seedArray[56];

public:
	Random() : Random(GenerateSeedFromTime())
	{
	}

	explicit Random(int seed)
	{
		int absSeed =
			(seed == INT32_MIN) ? INT32_MAX : std::abs(seed);

		int mk = 1;
		int mj = SeedConstant - absSeed;

		seedArray[55] = mj;

		for (int i = 1; i < 55; i++)
		{
			int index = (21 * i) % 55;

			seedArray[index] = mk;
			mk = mj - mk;

			if (mk < 0)
				mk += MaxIntValue;

			mj = seedArray[index];
		}

		for (int pass = 0; pass < 4; pass++)
		{
			for (int i = 1; i < 56; i++)
			{
				int offsetIndex = 1 + (i + 30) % 55;

				seedArray[i] -= seedArray[offsetIndex];

				if (seedArray[i] < 0)
					seedArray[i] += MaxIntValue;
			}
		}
	}

public:
	int Next()
	{
		if (++currentIndex >= 56)
			currentIndex = 1;

		if (++secondaryIndex >= 56)
			secondaryIndex = 1;

		int newRandomValue =
			seedArray[currentIndex] - seedArray[secondaryIndex];

		if (newRandomValue < 0)
			newRandomValue += MaxIntValue;

		seedArray[currentIndex] = newRandomValue;
		return newRandomValue;
	}

private:
	static int GenerateSeedFromTime()
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto duration = now.time_since_epoch();

		return static_cast<int>(
			std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
	}
};