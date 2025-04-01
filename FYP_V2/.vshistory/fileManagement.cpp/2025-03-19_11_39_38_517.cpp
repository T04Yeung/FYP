#include "pch.h"
#include "fileManagement.h"

#include <windows.h>
#include <commdlg.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

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
        return std::string(ofn.lpstrFile);
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

std::vector<Edge> fileFunctions::readEdgeFile(std::ifstream& edgeFile, std::vector<Node> nodeList) {
    std::vector<Edge> edgeList;
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

        auto fromIter = nodeNameToId.find(row[0]);
        auto toIter = nodeNameToId.find(row[1]);

        if (fromIter != nodeNameToId.end() && toIter != nodeNameToId.end()) {
            edge.from = fromIter->second;
            edge.to = toIter->second;
            edge.weight = std::stoi(row[2]);

            if (edge.from != -1 && edge.to != -1 && edge.weight != -1) {
                edge.id = idCount++;
                edgeList.push_back(edge);
            }
        }
    }

    return edgeList;
}