#include "timetable.h"
#include "configuration.h"
#include <iostream>

RoomCharacteristics::RoomCharacteristics() : roomNotOverlapping(false), enoughSeats(false),
	technicalEquipment(false), professorNotOverlapping(false), groupNotOverlapping(false) {}

Timetable::Timetable(uint numberOfCrossoverPoints, uint mutationSize,
	uint crossoverProbability, uint mutationProbability)
	: slots(std::vector<std::vector<std::shared_ptr<CourseClass>>>(DAYS_NUM * DAY_HOURS * Configuration::getInstance().rooms.size())),
	requirementsFlags(std::vector<RoomCharacteristics>(Configuration::getInstance().classes.size())),
	fitness(0), numberOfCrossoverPoints(numberOfCrossoverPoints), mutationSize(mutationSize),
	crossoverProbability(crossoverProbability), mutationProbability(mutationProbability) {}

Timetable::Timetable(std::shared_ptr<Timetable> other, bool setupOnly) {
	if (!setupOnly) {
		slots = other->slots;
		classes = other->classes;
		requirementsFlags= other->requirementsFlags;
		fitness = other->fitness;
	} else {
		slots.resize(DAYS_NUM * DAY_HOURS * Configuration::getInstance().rooms.size());
		requirementsFlags.resize(Configuration::getInstance().classes.size());
	}

	numberOfCrossoverPoints = other->numberOfCrossoverPoints;
	mutationSize = other->mutationSize;
	crossoverProbability = other->crossoverProbability;
	mutationProbability = other->mutationProbability;
}

std::shared_ptr<Timetable> Timetable::crossover(std::shared_ptr<Timetable> parent2) {
	if (rand() % 100 > crossoverProbability) {	// check probability of crossover operation
		return std::make_shared<Timetable>(shared_from_this(), false);	// no crossover, just copy first parent
	}

	std::shared_ptr<Timetable> newTimetable = std::make_shared<Timetable>(shared_from_this(), true); // new chromosome object, copy chromosome setup

	auto size = classes.size(); // number of classes
	std::vector<bool> crossoverPoints(size, false);

	for (auto i = 0; i < numberOfCrossoverPoints; ++i) { // determine crossover point (randomly)
		while (true) {
			auto p = rand() % size;
			if (!crossoverPoints[p]) {
				crossoverPoints[p] = true;
				break;
			}
		}
	}

	auto first = (rand() % 2 == 0);

	auto it1 = classes.begin();
	auto it2 = parent2->classes.begin();
	auto adj = 0;
	for (auto i = 0; i < size; ++i, ++it1, ++it2) { // make new code by combining parent codes
		if (first) {
			newTimetable->classes.insert(std::pair<std::shared_ptr<CourseClass>, uint>((*it1).first, (*it1).second)); // insert class from first parent into new chromosome's calss table
			for (auto j =  0; j < (*it1).first->durationInHours; ++j) {
				newTimetable->slots[(*it1).second + j].push_back((*it1).first);
			}
		} else {
			newTimetable->classes.insert(std::pair<std::shared_ptr<CourseClass>, uint>((*it2).first, (*it2).second));
			for (auto j = 0; j < (*it2).first->durationInHours; ++j) {
				newTimetable->slots[(*it2).second + j].push_back((*it2).first);
			}
		}

		if (crossoverPoints[i]) { // crossover point
			first = !first; // change soruce chromosome
		}
		// ++it1; ++it2;
	}
	newTimetable->calculateFitness();
	return newTimetable; // return smart pointer to offspring
}

void Timetable::mutation() {
	if (std::rand() % 100 > mutationProbability) {
		return;
	}

	auto numberOfClasses = classes.size();
	auto size = slots.size();

	for (auto i = mutationSize; i > 0; --i) { // move selected number of classes at random position
		// select random class for movement
		auto pos = std::rand() % numberOfClasses;
		auto it = std::next(classes.begin(), pos);
		std::shared_ptr<CourseClass> courseClass = it->first;

		// determine position of class randomly
		auto numberOfRooms = Configuration::getInstance().rooms.size();
		auto day = rand() % DAYS_NUM;
		auto room = rand() % numberOfRooms;
		auto dur = courseClass->durationInHours;
		auto time = rand() % (DAY_HOURS + 1 - dur);
		auto pos2 = day * numberOfRooms * DAY_HOURS + room * DAY_HOURS + time;

		// move all slots
		for (auto i = dur - 1; i >= 0; --i) {
			auto current = slots[pos + i];
			for (auto j = 0; j < current.size(); ++j) {
				if (current[j] == courseClass) {
					current.erase(current.begin() + j);
					break;
				}
			}

			// move class hour to new time-space slot
			slots.at(pos2 + i).push_back(courseClass);
		}
		it->second = pos2;
	}
	calculateFitness();
}

void Timetable::calculateFitness() {
	auto score = 0; // timetable' score

	auto numberOfRooms = Configuration::getInstance().rooms.size();
	auto daySize = DAY_HOURS * numberOfRooms;

	auto classRequirementsCoord = 0;

	// check criterias and calculate scores for each class in schedule
	for (auto & e : classes) {
		// coordinate of slot
		auto p = e.second;
		auto day = p / daySize;
		auto time = p % daySize;
		auto roomId = p / (DAY_HOURS * DAYS_NUM) + ((p % (DAY_HOURS * DAYS_NUM)) != 0);
		if (p == 0) {
			roomId = 1;
		}
		time %= DAY_HOURS;

		auto courseClass = e.first;
		auto dur = courseClass->durationInHours;

		// check for room overlapping of classes
		auto roomOverlapping = false;
		for (auto i = dur - 1; i >= 0; --i) {
			if (slots[p + i].size() > 1) {
				roomOverlapping = true;
				break;
			}
		}

		if (!roomOverlapping) {
			++score;
		}

		auto & currentFlags = requirementsFlags[classRequirementsCoord];
		currentFlags.roomNotOverlapping = !roomOverlapping;
		if (currentFlags.roomNotOverlapping) {
			++score;
		}

		auto room = Configuration::getInstance().getClassroomById(roomId);
		currentFlags.enoughSeats = (room->numberOfSeats >= courseClass->seatsRequired);
		if (currentFlags.enoughSeats) {
			++score;
		}

		currentFlags.technicalEquipment = !courseClass->isComputerRequired || (courseClass->isComputerRequired && room->isComputerAvailable);
		if (currentFlags.technicalEquipment) {
			score++;
		}

		auto professorOverlaps = false, studentsGroupOverlaps = false;

		auto exit = false; // check overlapping of classes for professors and student groups
		for (auto i = numberOfRooms, t = day * daySize + time; i > 0 && !exit; --i, t += DAY_HOURS) { // for each hour of class
			// check for overlapping with other classes at same time
			for (int j = dur - 1; j >= 0 && !exit; --j) {
				auto cl = slots[t + i];
				for (auto e1 : cl) {
					if (courseClass != e1) {
						// professor overlaps?
						if (!professorOverlaps && courseClass->professorOverlaps(e1)) {
							professorOverlaps = true;
						}
						// student group overlaps?
						if (!studentsGroupOverlaps && courseClass->studentsGroupOverlap(e1)) {
							studentsGroupOverlaps = true;
						}

						// both type of overlapping? no need to check more
						if (professorOverlaps && studentsGroupOverlaps) {
							exit = true;
							break;
						}
					}
				}
			}
		}

		// professors have no overlaping classes?
		if (!professorOverlaps) {
			score++;
		}
		currentFlags.professorNotOverlapping = !professorOverlaps;

		// student groups has no overlaping classes?
		if (!studentsGroupOverlaps) {
			score++;
		}
		currentFlags.groupNotOverlapping = !studentsGroupOverlaps;
		
		++classRequirementsCoord;
	}

	fitness = (float)score / (Configuration::getInstance().classes.size() * DAYS_NUM);
}

// Makes new chromosome with same setup but with randomly chosen code
std::shared_ptr<Timetable> Timetable::makeNewFromPrototype() {
	auto size = slots.size(); // number of time-space slots

	std::shared_ptr<Timetable> newChromosome = std::make_shared<Timetable>(shared_from_this(), true);

	int nr = Configuration::getInstance().rooms.size();
	// place classes at random position
	for (auto & courseClass : Configuration::getInstance().classes) {
		int dur = courseClass->durationInHours;
		int day = rand() % DAYS_NUM;
		int room = rand() % nr;
		int time = rand() % (DAY_HOURS + 1 - dur);
		int pos = day * nr * DAY_HOURS + room * DAY_HOURS + time;
		
		// fill time-space slots, for each hour of class
		for (int i = dur - 1; i >= 0; i--) {
			newChromosome->slots.at(pos + i).push_back(courseClass);
		}
		// insert in class table of chromosome
		newChromosome->classes.insert(std::pair<std::shared_ptr<CourseClass>, uint>(courseClass, pos));
	}

	newChromosome->calculateFitness();

	return newChromosome;
}
