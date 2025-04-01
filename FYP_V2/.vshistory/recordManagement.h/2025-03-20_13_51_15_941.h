#pragma once
#include <vector>
#include "record.h"

struct recordFilter {
	int deviceIdx = -1;
	int algorithmIdx = -1;
	time_t dateFrom = NULL;
	time_t dateTo = NULL;
};

class recordManagement {
	int getLatestRecordId();
	void addRecord(std::vector<Record>& recordList, Record record);
	void removeRecord(std::vector<Record>& recordList, Record record);
	void removeRecord(std::vector<Record>& recordList, int id);

	std::vector<Record> filterRecord(std::vector<Record>& recordList, recordFilter filter);
};