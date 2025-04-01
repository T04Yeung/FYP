#pragma once
#include <iostream>
#include <ctime>
#include "nlohmann/json.hpp"

class Record {
private:
	int id;
	time_t executionDate;
	int deviceIdx;
	std::string deviceName;
	int algorithmIdx;
	std::string algorithmName;
	double timeSpend;
};