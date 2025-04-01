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
	nlohmann::json details;
public:
	Record(int id, time_t executionDate, int deviceIdx, std::string deviceName, int algorithmIdx, std::string algorithmName, double timeSpend, nlohmann::json details) {
		this->id = id;
		this->executionDate = executionDate;
		this->deviceIdx = deviceIdx;
		this->deviceName = deviceName;
		this->algorithmIdx = algorithmIdx;
		this->algorithmName = algorithmName;
		this->timeSpend = timeSpend;
		this->details = details;
	}

	int getId() {
		return id;
	}

	time_t getExecutionDate() {
		return executionDate;
	}

	std::string getExecutionDateStr() {
		char date[11];
		struct tm* timeinfo{};
		localtime_s(timeinfo , &executionDate);
		strftime(date, sizeof(date), "%Y-%m-%d", timeinfo);
		return date;
	}

	int getDeviceIdx() {
		return deviceIdx;
	}

	std::string getDeviceName() {
		return deviceName;
	}

	int getAlgorithmIdx() {
		return algorithmIdx;
	}

	std::string getAlgorithmName() {
		return algorithmName;
	}

	double getTimeSpend() {
		return timeSpend;
	}

	nlohmann::json getDetails() {
		return details;
	}
};