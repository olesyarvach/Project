#pragma once
#include <string>
#include <memory>
#include <vector>

typedef unsigned short ushort;
typedef unsigned int uint;

struct Classroom {
	Classroom(std::string name, bool isComputerAvailable, uint numberOfSeats);

	std::string name;
	static ushort nextRoomId;
	bool isComputerAvailable;
	ushort id;
	uint numberOfSeats;
};

struct Course {
	Course(ushort id, std::string name);

	ushort id;
	std::string name;
};

struct CourseClass;

struct Professor {
	Professor(ushort id, std::string name);

	ushort id;
	std::string name;
	std::vector<std::shared_ptr<CourseClass>> courseClasses;
};

struct StudentsGroup {
	StudentsGroup(ushort id, std::string name, uint sizeOfGroup);

	ushort id;
	std::string name;
	std::vector<std::shared_ptr<CourseClass>> courseClasses;
	uint sizeOfGroup;
};

struct CourseClass {
	inline bool professorOverlaps(std::shared_ptr<CourseClass> other) {
		return professor->id == other->professor->id;
	}

	static std::shared_ptr<CourseClass> create(std::shared_ptr<Course> course,
		std::shared_ptr<Professor> professor,
		std::vector<std::shared_ptr<StudentsGroup>> groups,
		unsigned short durationInHours,
		bool isComputerRequired = false);
    
    bool studentsGroupOverlap(std::shared_ptr<CourseClass> other);

	std::shared_ptr<Course> course;
	std::shared_ptr<Professor> professor;
	std::vector<std::shared_ptr<StudentsGroup>> groups;
	ushort seatsRequired;
	bool isComputerRequired;
	ushort durationInHours;
};
