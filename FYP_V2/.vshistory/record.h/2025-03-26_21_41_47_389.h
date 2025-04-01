#pragma once
#include <iostream>
#include <ctime>
#include <vector>
#include "nlohmann/json.hpp"
#include "dataStructure.h"

class Record {
private:
	int id;
	time_t executionDate;
	int deviceIdx;
	std::string deviceName;
	int algorithmIdx;
	std::string algorithmName;
	double timeSpend;
	std::vector<Node> nodeList;
	nlohmann::json details;
public:
	Record(int id, time_t executionDate, int deviceIdx, std::string deviceName, int algorithmIdx, std::string algorithmName, double timeSpend, std::vector<Node> nodeList,nlohmann::json details) {
		this->id = id;
		this->executionDate = executionDate;
		this->deviceIdx = deviceIdx;
		this->deviceName = deviceName;
		this->algorithmIdx = algorithmIdx;
		this->algorithmName = algorithmName;
		this->timeSpend = timeSpend;
		this->nodeList = nodeList;
		this->details = details;
	}

	int getId() {
		return id;
	}

	time_t getExecutionDate() {
		return executionDate;
	}

	std::string getExecutionDateStr() {
		char date[20];
		struct tm timeinfo;
		localtime_s(&timeinfo, &executionDate); 
		strftime(date, sizeof(date), "%Y-%m-%d", &timeinfo);
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

	std::vector<Node> getNodeList() {
		return nodeList;
	}

	nlohmann::json getDetails() {
		return details;
	}
};