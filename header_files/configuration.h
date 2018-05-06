#pragma once

#include "payloads.h"
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>
#include <memory>
#include <cstddef>
using namespace std;


struct Configuration {
	static Configuration & getInstance();

	void parseFile(const char * fileName);

	shared_ptr<StudentsGroup> getGroupById(ushort id);

	shared_ptr<Classroom> getClassroomById(ushort id);

	shared_ptr<Professor> getProfessorById(ushort id);

	shared_ptr<Course> getCourseById(ushort id);

	shared_ptr<Professor> parseProfessor(ifstream & in);

	shared_ptr<StudentsGroup> parseGroup(ifstream & in);
  
  shared_ptr<Course> parseCourse(ifstream & in);

	shared_ptr<Classroom> parseClassroom(ifstream & in);

	shared_ptr<CourseClass> parseClass(ifstream & in);

	bool getConfigBlockLine(ifstream & in, std::string & key, string & value);
	
	unordered_map<ushort, shared_ptr<Professor>> professors;	// id-professor
	unordered_map<ushort, shared_ptr<StudentsGroup>> groups;	// id-group
	unordered_map<ushort, shared_ptr<Course>> courses;		// id-course
	unordered_map<ushort, shared_ptr<Classroom>> rooms;		// id-classroom
	vector<shared_ptr<CourseClass>> classes;					// classes
};

