#pragma once
#include "record.h"
#include <vector>
#include <fstream>
#include <windows.h>
#include <shobjidl.h>

class ExcelResult {
private:
	Record record;


public:
	virtual ~ExcelResult() {};
	virtual void exportResult();

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

class DijkstraExcel : ExcelResult {
private: 
	Record record;
public:
	void exportResult() override {
		
	}
};