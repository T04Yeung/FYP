#pragma once
#include <vector>
#include "record.h"

class recordManagement {
public:
	static int getLatestRecordId();
	static int getLatestRecordId(std::string directory);
	static void removeRecord(std::vector<Record>& recordList, Record record);
	static void removeRecord(std::vector<Record>& recordList, int id);
};