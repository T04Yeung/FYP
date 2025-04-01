#pragma once
#include "record.h"
#include <vector>
#include <fstream>
#include <windows.h>
#include <shobjidl.h>
#include <ctime>
#include <nlohmann/json.hpp>

class ExcelResult {
private:
	Record record;
public:
	virtual ~ExcelResult() {};
	virtual void exportResult();

    Record getRecord() {
        return record;
    }

    void setRecord(Record r) {
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
};

class DijkstraExcel : public ExcelResult {
public:
	void exportResult() override {
        std::string prefix = OpenFolderDialog();
        time_t now = std::time(0);

        char date[20];
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(date, sizeof(date), "%Y-%m-%d", &timeinfo);

        std::string fullPath = prefix + "\\" + date + "_Result.csv";
        std::ofstream outputFile(fullPath);

        Record record = getRecord();
        nlohmann::json details = record.getDetails();

        outputFile << "Execution Date:," + record.getExecutionDateStr() + "\n" ;
        outputFile << "Device:, " + record.getDeviceName() + ", ,Algorithm:," + record.getAlgorithmName() + "\n";

        std::string cpuTimeSpend = details.at("CPU Time Spend");
        std::string gpuTimeSpend = details.at("GPU Time Spend");

        outputFile << "CPU Time Spend:," + cpuTimeSpend + "\n";
        outputFile << "GPU Time Spend:," + gpuTimeSpend + "\n";

        outputFile.close();
	}
};