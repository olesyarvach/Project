#pragma once
#include <vector>
#include <memory>
#include "timetable.h"

struct Evolution {
	Evolution(uint numberOfChromosomes, uint replaceByGeneration, uint trackBest,
		std::shared_ptr<Timetable> prototype, uint generationsLimit);

	void start();

	inline std::shared_ptr<Timetable> getBestChromosome() {
		return population[bestChromosomes[0]];
	}

	void addToBest(uint chromosomeIndex);
  
  std::shared_ptr<Timetable> prototype;
	std::vector<std::shared_ptr<Timetable>> population;
	std::vector<bool> bestFlags;
	std::vector<ushort> bestChromosomes;
	uint replaceByGeneration;
	uint currentGeneration;
	uint currentBestSize;
	uint generationsLimit;
};
