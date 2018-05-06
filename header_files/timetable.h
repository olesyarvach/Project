#pragma once

#include "payloads.h"
#include <unordered_map>
#include <vector>
#include <cstddef>

#define DAYS_NUM	6	// number of days in week
#define DAY_HOURS	10	// working hours per day

struct RoomCharacteristics {
	RoomCharacteristics();

	bool roomNotOverlapping;
	bool enoughSeats;
	bool technicalEquipment;
	bool professorNotOverlapping;
	bool groupNotOverlapping;
};

struct Timetable : std::enable_shared_from_this<Timetable> {
	Timetable(std::shared_ptr<Timetable> other, bool setupOnly);

	std::shared_ptr<Timetable> crossover(std::shared_ptr<Timetable> parent2);

	void mutation();

	void calculateFitness();

	Timetable(uint numberOfCrossoverPoints, uint mutationSize,
		uint crossoverProbability, uint mutationProbability);

	std::shared_ptr<Timetable> makeNewFromPrototype();

	std::vector<RoomCharacteristics> requirementsFlags;
	float fitness;
	uint numberOfCrossoverPoints;
	uint mutationSize;
	uint crossoverProbability;
	uint mutationProbability;
	std::vector<std::vector<std::shared_ptr<CourseClass>>> slots; // time-space slots, one entry represents one lesson in one classroom
	std::unordered_map<std::shared_ptr<CourseClass>, uint> classes; // classs-position in slots
};

