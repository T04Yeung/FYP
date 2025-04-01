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
	Record(int id, std::time_t executionDate, int deviceIdx, const std::string& deviceName,
		int algorithmIdx, const std::string& algorithmName, double timeSpend,
		 const nlohmann::json& details)
		: id(id),
		executionDate(executionDate),
		deviceIdx(deviceIdx),
		deviceName(deviceName),
		algorithmIdx(algorithmIdx),
		algorithmName(algorithmName),
		timeSpend(timeSpend),
		details(details)
	{
	}
	Record(int id, std::time_t executionDate, int deviceIdx, const std::string& deviceName,
		int algorithmIdx, const std::string& algorithmName, double timeSpend,
		const std::vector<Node>& nodeList, const nlohmann::json& details)
		: id(id),
		executionDate(executionDate),
		deviceIdx(deviceIdx),
		deviceName(deviceName),
		algorithmIdx(algorithmIdx),
		algorithmName(algorithmName),
		timeSpend(timeSpend),
		nodeList(nodeList),
		details(details)
	{
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