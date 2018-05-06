#include "configuration.h"
#include "utilities.h"
#include <iostream>
Configuration & Configuration::getInstance() {
	static Configuration configuration;
	return configuration;
}

void Configuration::parseFile(const char * fileName) {
	professors.clear();
	groups.clear();
	courses.clear();
	rooms.clear();
	classes.clear();

	Classroom::nextRoomId = 0;
  
  std::ifstream in(fileName);
	std::string line;
	while (in.is_open() && !in.eof()) {
		std::getline(in, line);
		trim(line);

		if (line.compare("#prof") == 0) {
			auto parsedProfessor = parseProfessor(in);
			if (parsedProfessor != nullptr) {
				professors.insert(std::pair<ushort, std::shared_ptr<Professor>>(parsedProfessor->id, parsedProfessor));
			}
		}
		else if (line.compare("#group") == 0) {
			auto parsedGroup = parseGroup(in);
			if (parsedGroup != nullptr) {
				groups.insert(std::pair<ushort, std::shared_ptr<StudentsGroup>>(parsedGroup->id, parsedGroup));
			}
      }
		else if (line.compare("#course") == 0) {
			auto parsedCourse = parseCourse(in);
			if (parsedCourse != nullptr) {
				courses.insert(std::pair<ushort, std::shared_ptr<Course>>(parsedCourse->id, parsedCourse));
			}
		}
		else if (line.compare("#room") == 0) {
			auto parsedClassroom = parseClassroom(in);
			if (parsedClassroom != nullptr) {
				rooms.insert(std::pair<ushort, std::shared_ptr<Classroom>>(parsedClassroom->id, parsedClassroom));
			}
		}
    else if (line.compare("#class") == 0) {
			auto parsedClass = parseClass(in);
			if (parsedClass != nullptr) {
				classes.push_back(parsedClass);
			}
		}
	}
	// std::cout << professors.size() << ' ' << classes.size() << ' ' << rooms.size();
	in.close();
}

bool Configuration::getConfigBlockLine(std::ifstream & in, std::string & key, std::string & value) {
	std::string line;

	while (!in.eof()) {
		std::getline(in, line); // read line from config file
		trim(line);

		if (line.compare("#end") == 0) { // end of object's data 
			return false;
		}

		auto const positionOfEqualSign = line.find('=');
		if (positionOfEqualSign != std::string::npos) {
			key = line.substr(0, positionOfEqualSign);
			trim(key);

			value = line.substr(positionOfEqualSign + 1, line.length());
			trim(value);
      return true;
		}
	}

	return false;
}
std::shared_ptr<Classroom> Configuration::getClassroomById(ushort id) {
	auto const it = rooms.find(id);
	if (it == rooms.end()) {
		return nullptr;
	}
	return it->second;
}

std::shared_ptr<StudentsGroup> Configuration::getGroupById(ushort id) {
	auto const it = groups.find(id);
	if (it == groups.end()) {
		return nullptr;
	}
	return it->second;
}
std::shared_ptr<Professor> Configuration::getProfessorById(ushort id) {
	auto const it = professors.find(id);
	if (it == professors.end()) {
		return nullptr;
	}
	return it->second;
}

std::shared_ptr<Course> Configuration::getCourseById(ushort id) {
	auto const it = courses.find(id);
	if (it == courses.end()) {
		return nullptr;
	}
	return it->second;
}
std::shared_ptr<Professor> Configuration::parseProfessor(std::ifstream & in) {
	auto id = 0;
	std::string name;

	while (!in.eof()) {
		std::string key, value;

		if (!getConfigBlockLine(in, key, value)) { // get key-value pair
			break;
		}

		if (key.compare("id") == 0) {
			id = atoi(value.c_str());
		}
		else if (key.compare("name") == 0) {
			name = value;
      }
	}

	if (id == 0) {
		return nullptr;
	}
	return std::make_shared<Professor>(id, name);
}

std::shared_ptr<StudentsGroup> Configuration::parseGroup(std::ifstream & in) {
	auto id = 0, number = 0;
	std::string name;

	while (!in.eof()) {
		std::string key, value;
    if (!getConfigBlockLine(in, key, value)) { // get key-value pair
			break;
		}

		if (key.compare("id") == 0) {
			id = atoi(value.c_str());
		} else if (key.compare("name") == 0) {
			name = value;
		} else if (key.compare("f") == 0) {
			number = atoi(value.c_str());
		}
	}

	if (id == 0) {
		return nullptr;
	}
  return std::make_shared<StudentsGroup>(id, name, number);
}

std::shared_ptr<Course> Configuration::parseCourse(std::ifstream & in) {
	auto id = 0;
	std::string name;

	while (!in.eof()) {
		std::string key, value;

		if (!getConfigBlockLine(in, key, value)) { // get key-value pair
			break;
		}
    if (key.compare("id") == 0) {
			id = atoi(value.c_str());
		}
		else if (key.compare("name") == 0) {
			name = value;
		}
	}

	if (id == 0) {
		return nullptr;
	}
	return std::make_shared<Course>(id, name);
}
std::shared_ptr<Classroom> Configuration::parseClassroom(std::ifstream & in) {
	auto numberOfSeats = 0;
	auto isComputerAvailable = false;
	std::string name;

	while (!in.eof()) {
		std::string key, value;

		if (!getConfigBlockLine(in, key, value)) { // get key-value pair
			break;
		}

		if (key.compare("name") == 0) {
			name = value;
		}
		else if (key.compare("comp") == 0) {
			isComputerAvailable = (value.compare("true") == 0);
		}
    else if (key.compare("size") == 0) {
			numberOfSeats = atoi(value.c_str());
		}
	}

	if (numberOfSeats == 0) {
		return nullptr;
	}
	return std::make_shared<Classroom>(name, isComputerAvailable, numberOfSeats);
}
std::shared_ptr<CourseClass> Configuration::parseClass(std::ifstream & in) {
	auto professorId = 0, courseId = 0, duration = 1;
	auto isComputerRequired = false;

	std::vector<std::shared_ptr<StudentsGroup>> groups;

	while (!in.eof()) {
		std::string key, value;

		if (!getConfigBlockLine(in, key, value)) { // get key-value pair
			break;
		}

if (key.compare("professor") == 0) {
			professorId = atoi(value.c_str());
		}
		else if (key.compare("course") == 0) {
			courseId = atoi(value.c_str());
		}
		else if (key.compare("comp") == 0) {
			isComputerRequired = value.compare("true") == 0;
		}
		else if (key.compare("duration") == 0) {
			duration = atoi(value.c_str());
		}
    else if (key.compare("group") == 0) {
			auto group = getGroupById(atoi(value.c_str()));
			if (group) {
				groups.push_back(group);
			}
		}
	}

	auto professor = getProfessorById(professorId);
	auto course = getCourseById(courseId);

	if (course == nullptr || professor == nullptr) {
		return nullptr;
	}
return CourseClass::create(course, professor, groups, duration, isComputerRequired);
}
