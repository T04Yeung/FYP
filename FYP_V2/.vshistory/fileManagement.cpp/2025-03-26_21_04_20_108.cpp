#include "pch.h"
#include "fileManagement.h"
#include "record.h"

#include <windows.h>
#include <commdlg.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <unordered_set>

const std::string dataRootDirectoryStr = fileFunctions::getDataRootDirectoryStr();
const std::vector<std::string> subDirectoryList = fileFunctions::getSubDirectoryList();

std::string fileFunctions::OpenFileDialog() {
    OPENFILENAME ofn;
    char szFile[260];       

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = (LPWSTR) szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files(*.*)\0*.*\0CSV Files(*.CSV)\0*.CSV\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        std::wstring wideStr(ofn.lpstrFile);
        return std::string(wideStr.begin(), wideStr.end());
    }
    return "";
}

bool fileFunctions::isCSVFile(std::string fileName) {
    if (fileName.substr(fileName.find_last_of(".") + 1) != "csv") {
        return false;
    }

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: File does not exist or cannot be opened: " << fileName << std::endl;
        return false;
    }

    file.close();

    return true;
}

std::vector<std::string> fileFunctions::readCSVFileLine(std::string line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> row;

    while (getline(ss, item, ',')) {
        row.push_back(item);
    }

    return row;
}

std::vector<Node> fileFunctions::readNodeFile(std::ifstream& nodeFile) {
    std::vector<Node> nodeList;
    if (!nodeFile.is_open()) {
        std::cerr << "Error: Unable to open node file." << std::endl;
		return nodeList;
    }

    std::string line;

    int idCount = 0;

    while (getline(nodeFile, line)) {
        Node node;
		node.id = idCount++;
        node.name = readCSVFileLine(line).front();
        nodeList.push_back(node);
    }

    return nodeList;
}

std::unordered_set<Edge> fileFunctions::readEdgeFile(std::ifstream& edgeFile, std::vector<Node> nodeList) {
    std::unordered_set<Edge> edgeList;
    if (!edgeFile.is_open()) {
        std::cerr << "Error: Unable to open edge file." << std::endl;
        return edgeList;
    }
    std::string line;

    std::unordered_map<std::string, int> nodeNameToId;
    for (const auto& node : nodeList) {
        nodeNameToId[node.name] = node.id;
    }

    int idCount = 0;

    while (getline(edgeFile, line)) {
        std::vector<std::string> row = readCSVFileLine(line);
        Edge edge;

        if (row.size() != 3) continue;

        auto fromIter = nodeNameToId.find(row[0]);
        auto toIter = nodeNameToId.find(row[1]);

        if (fromIter != nodeNameToId.end() && toIter != nodeNameToId.end()) {
            edge.from = fromIter->second;
            edge.to = toIter->second;
            edge.weight = std::stoi(row[2]);

            if (edge.from != -1 && edge.to != -1 && edge.weight != -1) {
                edge.id = idCount++;
                edgeList.insert(edge);
            }
        }
    }

    return edgeList;
}

void fileFunctions::checkSystemDirectoryExistence() {
    std::filesystem::path directory = dataRootDirectoryStr;

    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }

    nlohmann::json configJson;
    configJson["recordId"] = 0;

    std::filesystem::path configFile = dataRootDirectoryStr + "\\config.json";

    if (!std::filesystem::exists(configFile)) {
        std::ofstream jsonFile(configFile);

        if (jsonFile.is_open()) {
            jsonFile << configJson.dump(4);
        }
    }

    for (std::string subDirectory : subDirectoryList) {
        std::filesystem::path tempDirectory = dataRootDirectoryStr + subDirectory;

        if (!std::filesystem::exists(tempDirectory)) {
            std::filesystem::create_directories(tempDirectory);
        }
    }
}

bool fileFunctions::createRecordFile(Record record) {
    nlohmann::json recordJson;
    recordJson["id"] = record.getId();
    recordJson["executionDate"] = record.getExecutionDate();
    recordJson["executionDateStr"] = record.getExecutionDateStr();
    recordJson["deviceIdx"] = record.getDeviceIdx();
    recordJson["deviceName"] = record.getDeviceName();
    recordJson["algorithmIdx"] = record.getAlgorithmIdx();
    recordJson["algorithmName"] = record.getAlgorithmName();
    recordJson["timeSpend"] = record.getTimeSpend();
    recordJson["nodeList"] = record.getNodeList();
    recordJson["detail"] = record.getDetails();

    try {
        std::stringstream ss;
        ss << std::setw(6) << std::setfill('0') << record.getId();
        std::string idStr = ss.str();

        std::filesystem::path filePath = dataRootDirectoryStr + subDirectoryList[0] + "\\" + idStr + ".json";
        std::ofstream jsonFile(filePath);

        if (jsonFile.is_open()) {
            jsonFile << recordJson.dump(4);
        }

        jsonFile.close();
    }
    catch (const std::exception& e) {
        return false;
    }

    return true;
}

bool fileFunctions::removeRecordFile(Record record) {
    return removeRecordFile(record.getId());
}

bool fileFunctions::removeRecordFile(int id) {
    std::string directoryPath = dataRootDirectoryStr + subDirectoryList[0];

    for (const auto& record : std::filesystem::directory_iterator(directoryPath)) {
        std::string recordFileStr = record.path().filename().string();

        std::stringstream ss;
        ss << std::setw(6) << std::setfill('0') << id;
        std::string idStr = ss.str();

        if (recordFileStr == idStr + ".json") {
            std::filesystem::remove(record.path());
            return true;
        }
    }

    return false;
}

std::vector<Record> fileFunctions::getAllRecords() {
    std::vector<Record> records;
    std::string directoryPath = dataRootDirectoryStr + subDirectoryList[0];
    for (const auto& record : std::filesystem::directory_iterator(directoryPath)) {
        if (record.is_regular_file() && record.path().extension() == ".json") {
            std::ifstream file(record.path());
            if (!file.is_open()) {
                continue;
            }

            nlohmann::json recordJson;
            file >> recordJson;

            int id = recordJson["id"].get<int>();
            time_t executionDate = recordJson["executionDate"].get<time_t>();
            int deviceIdx = recordJson["deviceIdx"].get<int>();
            std::string deviceName = recordJson["deviceName"].get<std::string>();
            int algorithmIdx = recordJson["algorithmIdx"].get<int>();
            std::string algorithmName = recordJson["algorithmName"].get<std::string>();
            double timeSpend = recordJson["timeSpend"].get<double>();
            nlohmann::json details = recordJson["details"];

            records.emplace_back(
                id, executionDate, deviceIdx, deviceName,
                algorithmIdx, algorithmName, timeSpend, details
            );
        }
    }

    return records;
}