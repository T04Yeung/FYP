#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include "dataStructure.h"
#include "record.h"

class fileFunctions {
public:
	static std::string getDataRootDirectoryStr() { return std::filesystem::temp_directory_path().string() + "\\GPApplication"; }
	static std::vector<std::string> getSubDirectoryList() { return { "\\record" }; }

	static std::string OpenFileDialog();
	static bool isCSVFile(std::string fileName);
	static std::vector<std::string> readCSVFileLine(std::string line);
	static std::vector<Node> readNodeFile(std::ifstream& nodeFile);
	static std::unordered_set<Edge> readEdgeFile(std::ifstream& edgeFile, std::vector<Node> nodeList);
	static void checkSystemDirectoryExistence();
	static bool createRecordFile(Record record);
	static bool removeRecordFile(Record record);
	static bool removeRecordFile(int id);
	static std::vector<Record> getAllRecords();
};