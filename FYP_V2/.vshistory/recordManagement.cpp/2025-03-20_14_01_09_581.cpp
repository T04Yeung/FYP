#include <iostream>
#include <algorithm>
#include <fstream>
#include "nlohmann/json.hpp"
#include "recordManagement.h"
#include "fileManagement.h"

int recordManagement::getLatestRecordId() {
	std::string filePath = fileFunctions::dataRootDirectoryStr + "\\config.json";

	std::ifstream configIfs(filePath);
	nlohmann::json configJson = nlohmann::json::parse(configIfs);
	configIfs.close();

	int latestId = configJson.at("recordId") + 1;
	configJson["recordId"] = latestId;

	std::ofstream configOfs(filePath);
	configOfs << configJson.dump(4);
	configOfs.close();
}

void recordManagement::addRecord(std::vector<Record>& recordList, Record record) {
	recordList.emplace_back(record);
	sort(recordList.begin(), recordList.end(), [](Record r1, Record r2) {
		return r1.getId() < r2.getId();
	});

	fileFunctions::createRecordFile(record);
}
void recordManagement::removeRecord(std::vector<Record>& recordList, Record record) {
	removeRecord(recordList, record.getId());
}
void recordManagement::removeRecord(std::vector<Record>& recordList, int id) {
	for (size_t i = 0; i < recordList.size(); i++) {
		if (recordList[i].getId() == id) {
			recordList.erase(recordList.begin() + i);
		}
	}
}

std::vector<Record> recordManagement::filterRecord(std::vector<Record>& recordList, recordFilter filter) {

}