#include "pch.h"
#include "HomePage.xaml.h"
#include "recordManagement.h"
#include "record.h"
#include "fileManagement.h"
#include "graphManagement.h"
#include "CPU_Algorithm.h"
#include "GPU_Algorithm.cuh"
#include "nlohmann/json.hpp"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <fstream>
#include <chrono>
#include <vector>
#include <ctime>
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

Graph graph;
std::vector<Node> currentNodes;
std::vector<Edge> currentEdges;

const std::vector<std::string> deviceNameList = {
    "CPU",
    "GPU",
    "CPU and GPU"
};

const std::vector<std::string> algorithmNameList = {
    "Parallel Dijkstra's algorithm",
    "2D Partitioning BFS"
};

namespace winrt::FYP_V2::implementation
{
    int32_t HomePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void HomePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}

winrt::fire_and_forget winrt::FYP_V2::implementation::HomePage::ShowMessage(hstring message) {
    Controls::ContentDialog dialog;
	dialog.XamlRoot(this->XamlRoot());
    dialog.Title(winrt::box_value(hstring(L"Message")));
    dialog.Content(winrt::box_value(message));
    dialog.PrimaryButtonText(hstring(L"OK"));

    co_await dialog.ShowAsync();
}

void winrt::FYP_V2::implementation::HomePage::ImportVertices(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    std::string filename = fileFunctions::OpenFileDialog();

    if (filename.empty()) {
		return;
    }

    if (fileFunctions::isCSVFile(filename)) {
        std::ifstream nodeFile(filename);

        currentNodes = fileFunctions::readNodeFile(nodeFile);

        nodeFile.close();
    }
    else {
		ShowMessage(L"Invalid file type. Please select a CSV file.");
    }
}

void winrt::FYP_V2::implementation::HomePage::ImportEdges(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    if (currentNodes.size() == 0) {
		ShowMessage(L"No nodes imported. Please import nodes first.");
        return;
    }

    std::string filename = fileFunctions::OpenFileDialog();

    if (filename.empty()) {
        return;
    }

    if (fileFunctions::isCSVFile(filename)) {
        std::ifstream edgeFile(filename);

        currentEdges = fileFunctions::readEdgeFile(edgeFile, currentNodes);

        edgeFile.close();

		graph = graphManagement::createGraph(currentNodes, currentEdges);

		visualizeGraph();
    }
    else {
		ShowMessage(L"Invalid file type. Please select a CSV file.");
    }
}

void winrt::FYP_V2::implementation::HomePage::executionBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    int32_t algorithmIdx = algorithmBox().SelectedIndex();

    if (algorithmIdx == -1) {
        ShowMessage(L"Please select an algorithm first!");
        return;
    }
  
    int32_t deviceIdx = deviceGroup().SelectedIndex();
    double timeSpend = 0.0;
    nlohmann::json details;

    if (deviceIdx == 0) {
        auto start = std::chrono::high_resolution_clock::now();
        switch (algorithmIdx) {
            case 0: {
                std::vector<std::vector<int>> result = CPU_Algorithm::CPU_Parallel_Dijkstra(graph);
                details["Result"] = result;
                break;
            }
            case 1: {
                std::vector<int> result = CPU_Algorithm::CPU_2DPartition_BFS(graph, 0);
                details["Result"] = result;
                break;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        timeSpend = duration.count();
        details["CPU Time Spend"] = timeSpend;

        std::string tempStr = std::to_string(duration.count());

    }
    else if (deviceIdx == 1) {
        auto start = std::chrono::high_resolution_clock::now();
        switch (algorithmIdx) {
            case 0: {
                std::vector<std::vector<int>> result = GPU_Algorithm::GPU_Parallel_Dijkstra(graph);
                details["Result"] = result;
                break;
            }
            case 1: {
                std::vector<int> result = GPU_Algorithm::GPU_2DPartition_BFS(graph, 0);
                details["Result"] = result;
                break;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        timeSpend = duration.count();

        details["GPU Time Spend"] = timeSpend;

        std::string tempStr = std::to_string(duration.count());

    }
    else if (deviceIdx == 2) {

        switch (algorithmIdx) {
            case 0: {
                auto start1 = std::chrono::high_resolution_clock::now();

                std::vector<std::vector<int>> result1 = CPU_Algorithm::CPU_Parallel_Dijkstra(graph);
                details["CPU Result"] = result1;

                auto end1 = std::chrono::high_resolution_clock::now();

                auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
                details["CPU Time Spend"] = duration1.count();

                auto start2 = std::chrono::high_resolution_clock::now();

                std::vector<std::vector<int>> result2 = GPU_Algorithm::GPU_Parallel_Dijkstra(graph);
                details["GPU Result"] = result2;

                auto end2 = std::chrono::high_resolution_clock::now();

                auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
                details["GPU Time Spend"] = duration2.count();

                std::string tempStr = std::to_string(duration1.count() + duration2.count());

                timeSpend = duration1.count() + duration2.count();

                break;
            }
            case 1: {
                auto start1 = std::chrono::high_resolution_clock::now();

                std::vector<int> result1 = CPU_Algorithm::CPU_2DPartition_BFS(graph, 0);
                details["CPU Result"] = result1;

                auto end1 = std::chrono::high_resolution_clock::now();

                auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
                details["CPU Time Spend"] = duration1.count();

                auto start2 = std::chrono::high_resolution_clock::now();

                std::vector<int> result2 = GPU_Algorithm::GPU_2DPartition_BFS(graph, 0);
                details["GPU Result"] = result2;

                auto end2 = std::chrono::high_resolution_clock::now();

                auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
                details["GPU Time Spend"] = duration2.count();

                std::string tempStr = std::to_string(duration1.count() + duration2.count());

                timeSpend = duration1.count() + duration2.count();

                break;
            }
        }
    }

    fileFunctions::checkSystemDirectoryExistence();
    Record record = Record(recordManagement::getLatestRecordId(), time(0), deviceIdx, deviceNameList.at(deviceIdx), algorithmIdx, algorithmNameList.at(algorithmIdx), timeSpend, details);
    fileFunctions::createRecordFile(record);
}

void winrt::FYP_V2::implementation::HomePage::visualizeGraph()
{
	GraphCanvas().Children().Clear();

    int** adjMatrix = graph.getAdjacencyMatrix();
	int numOfVertices = graph.getNumOfVertices();

	if (numOfVertices == 0) {
		return;
	}

    const double centerX = 400;
    const double centerY = 300;
    const double radius = 250;

    std::vector<std::pair<double, double>> nodePositions;
    for (int i = 0; i < numOfVertices; ++i) {
        const double angle = 2 * 3.14159265 * i / numOfVertices;
        const double x = centerX + radius * std::cos(angle);
        const double y = centerY + radius * std::sin(angle);
        nodePositions.emplace_back(x, y);
    }

    for (int i = 0; i < numOfVertices; ++i) {
        for (int j = i + 1; j < numOfVertices; ++j) {
            if (adjMatrix[i][j] != 0) {
                const auto& [x1, y1] = nodePositions[i];
                const auto& [x2, y2] = nodePositions[j];
            }
        }
    }

    for (int i = 0; i < numOfVertices; ++i) {
        const auto& [x, y] = nodePositions[i];

        Ellipse ellipse;
        ellipse.Width(40);
        ellipse.Height(40);
        ellipse.Fill(SolidColorBrush(Colors::Blue()));
        Canvas::SetLeft(ellipse, x - 20); // Center the ellipse
        Canvas::SetTop(ellipse, y - 20);
        GraphCanvas().Children().Append(ellipse);

        // Add node label
        TextBlock text;
        text.Text(winrt::to_hstring(i));
        text.Foreground(SolidColorBrush(Colors::White()));
        Canvas::SetLeft(text, x - 10); // Approximate centering
        Canvas::SetTop(text, y - 10);
        GraphCanvas().Children().Append(text);
    }
}
