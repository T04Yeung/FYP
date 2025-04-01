#pragma once
#include "RecordViewModel.h"
#include "fileManagement.h"
#include <vector>
#include <fstream>
#include <windows.h>
#include <shobjidl.h>
#include <ctime>
#include <climits>
#include <nlohmann/json.hpp>

class ExcelResult {
private:
    winrt::FYP_V2::RecordViewModel record;
public:
    ExcelResult() = default;
	virtual void exportResult() = 0;

    winrt::FYP_V2::RecordViewModel getRecord() {
        return record;
    }

    void setRecord(winrt::FYP_V2::RecordViewModel r) {
        record = r;
    }

    std::string OpenFolderDialog() {
        std::string folderPath;

        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr)) {
            IFileOpenDialog* pFileDialog = nullptr;

            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&pFileDialog);
            if (SUCCEEDED(hr)) {
                DWORD dwOptions;
                pFileDialog->GetOptions(&dwOptions);
                pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);

                hr = pFileDialog->Show(NULL);
                if (SUCCEEDED(hr)) {
                    IShellItem* pItem = nullptr;
                    hr = pFileDialog->GetResult(&pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pszFolderPath = nullptr;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                        if (SUCCEEDED(hr)) {
                            char buffer[MAX_PATH];
                            WideCharToMultiByte(CP_UTF8, 0, pszFolderPath, -1, buffer, MAX_PATH, NULL, NULL);
                            folderPath = buffer;
                            CoTaskMemFree(pszFolderPath);
                        }
                        pItem->Release();
                    }
                }
                pFileDialog->Release();
            }
            CoUninitialize();
        }
        return folderPath;
    }

    std::string getFullPath() {
        std::string prefix = OpenFolderDialog();
        time_t now = std::time(0);

        char date[20];
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(date, sizeof(date), "%Y-%m-%d_%H_%M_%S", &timeinfo);

        std::string fullPath = prefix + date + "_Result.csv";

        return fullPath;
    }

    void generateDefaultContent(winrt::FYP_V2::RecordViewModel& record, std::ofstream& outputFile) {
        std::string detailsStr = winrt::to_string(record.Details());
        nlohmann::json details = nlohmann::json::parse(detailsStr);

        outputFile << "Execution Date:," + winrt::to_string(record.ExecutionDate()) + "\n";
        outputFile << "Device:, " + winrt::to_string(record.DeviceName()) + ", ,Algorithm:," + winrt::to_string(record.AlgorithmName()) + "\n";

        double cpuTimeSpend = details.value("CPU Time Spend", 0.0);
        double gpuTimeSpend = details.value("GPU Time Spend", 0.0);

        outputFile << "CPU Time Spend:," + std::to_string(cpuTimeSpend) + "\n";
        outputFile << "GPU Time Spend:," + std::to_string(gpuTimeSpend) + "\n";
    }
};

class DijkstraExcel : public ExcelResult {
public:
    DijkstraExcel() = default;
	void exportResult() override {
        winrt::FYP_V2::RecordViewModel record = getRecord();

        std::string detailsStr = winrt::to_string(record.Details());
        nlohmann::json details = nlohmann::json::parse(detailsStr);

        std::ofstream outputFile(getFullPath());

        generateDefaultContent(record, outputFile);

        outputFile << "Result:\n";

        std::vector<std::vector<int>> result = details.at("Result");

        winrt::Windows::Foundation::Collections::IVector<winrt::hstring> nodeList = record.NodeList();

        outputFile << " ,";

        for (auto& node : nodeList) {
            outputFile << winrt::to_string(node) + ",";
        }
        outputFile << "\n";

        for (size_t i = 0; i < nodeList.Size(); i++) {
            std::string currentNodeName = winrt::to_string(nodeList.GetAt(i));
            outputFile << currentNodeName + ",";
            for (size_t j = 0; j < nodeList.Size(); j++) {
                int tempResult = result[i][j];
                if (tempResult == INT_MAX) {
                    outputFile << "/,";
                }
                else {
                    outputFile << std::to_string(tempResult) + ",";
                }
            }
            outputFile << "\n";
        }

        outputFile.close();
	}
};

class _2DPartitioningBFSExcel : public ExcelResult {
public:
    _2DPartitioningBFSExcel() = default;
    void exportResult() override {
        winrt::FYP_V2::RecordViewModel record = getRecord();

        std::string detailsStr = winrt::to_string(record.Details());
        nlohmann::json details = nlohmann::json::parse(detailsStr);

        std::ofstream outputFile(getFullPath());

        generateDefaultContent(record, outputFile);

        outputFile << "Result:\n";

        std::vector<int> result = details.at("Result");

		int src = details.at("Source");
        winrt::Windows::Foundation::Collections::IVector<winrt::hstring> nodeList = record.NodeList();

        outputFile << "Source Node:,";

        outputFile << src;

        outputFile << "\n";

        for (size_t i = 0; i < result.size(); i++) {
			std::string currentNodeName = winrt::to_string(nodeList.GetAt(i));
			outputFile << currentNodeName + ",";
			int tempResult = result[i];
			if (tempResult == -1) {
				outputFile << "/,";
			}
			else {
				outputFile << std::to_string(tempResult) + ",";
			}
			outputFile << "\n";
        }

        outputFile.close();
    }
};