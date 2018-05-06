#include "payloads.h"

ushort Classroom::nextRoomId = 0;

Classroom::Classroom(std::string name, bool isComputerAvailable, uint numberOfSeats)
	: name(name), isComputerAvailable(isComputerAvailable), numberOfSeats(numberOfSeats), id(++nextRoomId) {}

Course::Course(ushort id, std::string name) : id(id), name(name) {}

Professor::Professor(ushort id, std::string name) : id(id), name(name) {}

StudentsGroup::StudentsGroup(ushort id, std::string name, uint sizeOfGroup)
	: id(id), name(name), sizeOfGroup(sizeOfGroup) {}

std::shared_ptr<CourseClass> CourseClass::create(std::shared_ptr<Course> course, std::shared_ptr<Professor> professor, std::vector<std::shared_ptr<StudentsGroup>> groups, unsigned short durationInHours, bool isComputerRequired) {
	auto newClass = std::make_shared<CourseClass>();
	newClass->professor = professor;
	newClass->course = course;
	newClass->groups = groups;
	newClass->durationInHours = durationInHours;
	newClass->isComputerRequired = isComputerRequired;
	newClass->seatsRequired = 0;

	professor->courseClasses.push_back(newClass);
	for (auto e : groups) {
		e->courseClasses.push_back(newClass);
		newClass->seatsRequired += e->sizeOfGroup;
	}

	return newClass;
}

bool CourseClass::studentsGroupOverlap(std::shared_ptr<CourseClass> other) {
	for (auto e : groups) {
		for (auto e1 : other->groups) {
			if (e == e1) {
				return true;
			}
		}
	}
	return false;
}
