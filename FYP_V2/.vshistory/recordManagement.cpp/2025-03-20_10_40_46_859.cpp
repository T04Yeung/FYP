#include <iostream>
#include <algorithm>
#include "recordManagement.h"
#include "fileManagement.h"

void recordManagement::addRecord(std::vector<Record>& recordList, Record record) {
	recordList.emplace_back(record);
	sort(recordList.begin(), recordList.end(), [](Record r1, Record r2) {
		return r1.getId() < r2.getId();
	});

	fileFunctions::cr
}
void recordManagement::removeRecord(std::vector<Record>& recordList, Record record) {
	removeRecord(recordList, record.getId());
}
void recordManagement::removeRecord(std::vector<Record>& recordList, int id) {

}

std::vector<Record> recordManagement::filterRecord(std::vector<Record>& recordList, recordFilter filter) {

}