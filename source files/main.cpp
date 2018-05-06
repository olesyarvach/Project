#include <iostream>
#include "evolution.h"
#include "configuration.h"
#include "timetable.h"
#include <algorithm>
#include <vector>

int main() {
	std::cout << "Алгоритм не справился с поставленной задачей" << std::endl;
	Configuration::getInstance().parseFile("app.properties");
	auto timetablePrototype = std::make_shared<Timetable>(2, 2, 80, 3);

	Evolution evolution(100, 8, 5, timetablePrototype, 100);
	evolution.start();

	auto result = evolution.getBestChromosome();
	if (result->fitness >= 1) {
		// ok
	}
  else {
		std::cout << "Алгоритм не справился с поставленной задачей" << std::endl;
	}
	return 0;
}
