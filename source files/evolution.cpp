#include "evolution.h"
#include "ctime"

Evolution::Evolution(uint numberOfChromosomes, uint replaceByGeneration, uint trackBest,
	std::shared_ptr<Timetable> prototype, uint generationsLimit) : replaceByGeneration(replaceByGeneration),
	currentBestSize(0),
	prototype(prototype),
	generationsLimit(generationsLimit),
	currentGeneration(0) {
	if (numberOfChromosomes < 2) { // there should be at least 2 chromosomes in population
		numberOfChromosomes = 2;
	}

	if (trackBest < 1) { // and algorithm should track at least on of best chromosomes
		trackBest = 1;
	}
  if (replaceByGeneration < 1) {
		replaceByGeneration = 1;
	} else if (replaceByGeneration > numberOfChromosomes - trackBest) {
		replaceByGeneration = numberOfChromosomes - trackBest;
	}

	population = std::vector<std::shared_ptr<Timetable>>(numberOfChromosomes, nullptr); // reserve space for population
	bestFlags = std::vector<bool>(numberOfChromosomes, false);

	bestChromosomes.resize(trackBest); // reserve space for best chromosome group
}
void Evolution::start() {
	std::srand(std::time(nullptr));

	for (auto i = 0; i < bestFlags.size(); ++i) {
		bestFlags[i] = false;
	}
	currentBestSize = 0;
	for (auto i = 0; i < population.size(); ++i) {
		population[i] = prototype->makeNewFromPrototype();
		addToBest(i);
	}

	for (currentGeneration = 0; currentGeneration < generationsLimit; ++currentGeneration) {
		auto bestChromosome = getBestChromosome();
		if (bestChromosome->fitness >= 1) {
			break;
		}
    std::vector<std::shared_ptr<Timetable>> offspring(replaceByGeneration);
		for (auto i = 0; i < offspring.size(); ++i) {
			auto firstParent = population[rand() % population.size()];
			auto secondParent = population[rand() % population.size()];

			offspring[i] = firstParent->crossover(secondParent);
			offspring[i]->mutation();
		}
		for (auto i = 0; i < replaceByGeneration; ++i) {
			int ci; // index of chromosome
			do {
				ci = rand() % population.size(); // select chromosome for replacement randomly
			} while (bestFlags[ci]); // protect best chromosomes from replacement

			population[ci] = offspring[i];

			addToBest(ci); // try to add new chromosomes in best chromosome group
		}
	}
  }

void Evolution::addToBest(uint chromosomeIndex) {
	// don't add if new chromosome hasn't fitness big enough for best chromosome group
	// or it is already in the group?
	if ((currentBestSize == bestChromosomes.size() &&
		population[bestChromosomes[currentBestSize - 1]]->fitness >=
		population[chromosomeIndex]->fitness) || bestFlags[chromosomeIndex]) {
		return;
	}

// find place for new chromosome
	auto i = currentBestSize;
	for (; i > 0; --i) {
		// group is not full?
		if (i < bestChromosomes.size()) {
			// position of new chromosomes is found?
			if (population[bestChromosomes[i - 1]]->fitness >
				population[chromosomeIndex]->fitness) {
				break;
			}
      // move chromosomes to make room for new
			bestChromosomes[i] = bestChromosomes[i - 1];
		}
		else
			// group is full remove worst chromosomes in the group
			bestFlags[bestChromosomes[i - 1]] = false;
	}

// store chromosome in best chromosome group
	bestChromosomes[i] = chromosomeIndex;
	bestFlags[chromosomeIndex] = true;

	// increase current size if it has not reached the limit yet
	if (currentBestSize < bestChromosomes.size()) {
		currentBestSize++;
	}
}
