#include "pch.h"
#include "HomePage.xaml.h"
#include "SharedData.h"
#include "recordManagement.h"
#include "record.h"
#include "fileManagement.h"
#include "graphManagement.h"
#include "CPU_Algorithm.h"
#include "GPU_Algorithm.cuh"
#include "nlohmann/json.hpp"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Input.Interop.h>
#include <winrt/Windows.UI.h>
#include <random>
#include <algorithm>
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
using namespace Microsoft::UI::Xaml::Shapes;
using namespace Microsoft::UI::Xaml::Media;
using namespace Windows::UI;

#define graph SharedData::getInstance().graph
#define currentNodes SharedData::getInstance().currentNodes
#define currentEdges SharedData::getInstance().currentEdges
#define nodePositions SharedData::getInstance().nodePositions

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
    if (!currentNodes.empty()) {
		bool isConfirmed = false;

        Controls::ContentDialog dialog;
        dialog.XamlRoot(this->XamlRoot());
        dialog.Title(winrt::box_value(hstring(L"Confirmation")));
        dialog.Content(winrt::box_value(hstring(L"There are already a imported node list. You will clear all graph data if you import a new one.\nAre you sure?")));
        dialog.PrimaryButtonText(hstring(L"OK"));
		dialog.CloseButtonText(hstring(L"Cancel"));

		dialog.PrimaryButtonClick([&isConfirmed](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const&) {
			isConfirmed = true;
			}
        );

        dialog.ShowAsync();
        if (!isConfirmed) {
            return;
        }

		currentNodes.clear();
        currentEdges.clear();
		nodePositions.clear();
		GraphCanvas().Children().Clear();
		GraphBtnPanel().Visibility(Visibility::Collapsed);
    }

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

		SharedData::getInstance().addSnapshot();

		graph = SharedData::getInstance().getGraph(SharedData::getInstance().currentDisplayGraphId);

		visualizeGraph();
        if (SharedData::getInstance().totalSnapshots > 1) {
            GraphBtnPanel().Visibility(Visibility::Visible);
        }
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

void getNodesPosition(int** adjMatrix, const int& numOfVertices) {
    const int canvasWidth = 8000;
    const int canvasHeight = 5500;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disW(100.0, canvasWidth - 100.0);
    std::uniform_real_distribution<> disH(100.0, canvasHeight - 100.0);
    for (int i = 0; i < numOfVertices; i++) {
        nodePositions.emplace_back(disW(gen), disH(gen));
    }

    const int iterations = 150;
    const double rep = 2500.0;
    const double attr = 0.05;
    const double length = 300.0;
    const double damping = 0.75;

    for (int iter = 0; iter < iterations; iter++) {
        std::vector<std::pair<double, double>> forces(numOfVertices, { 0.0, 0.0 });

        for (int i = 0; i < numOfVertices; i++) {
            for (int j = 0; j < numOfVertices; j++) {
                if (i == j) continue;

                double dx = nodePositions[j].first - nodePositions[i].first;
                double dy = nodePositions[j].second - nodePositions[i].second;
                double distance = max(std::hypot(dx, dy), 1.0);

                double force = rep / (distance * distance);
                forces[i].first -= force * dx / distance;
                forces[i].second -= force * dy / distance;
            }
        }

        for (int i = 0; i < numOfVertices; i++) {
            for (int j = i + 1; j < numOfVertices; ++j) {
                if (adjMatrix[i][j] == 0) continue;

                double dx = nodePositions[j].first - nodePositions[i].first;
                double dy = nodePositions[j].second - nodePositions[i].second;
                double distance = max(std::hypot(dx, dy), 1.0);

                double force = attr * (distance - length);
                forces[i].first += force * dx / distance;
                forces[i].second += force * dy / distance;
                forces[j].first -= force * dx / distance;
                forces[j].second -= force * dy / distance;
            }
        }

        for (int i = 0; i < numOfVertices; i++) {
            nodePositions[i].first += forces[i].first * damping;
            nodePositions[i].second += forces[i].second * damping;

            nodePositions[i].first = std::clamp(nodePositions[i].first, 50.0, canvasWidth - 50.0);
            nodePositions[i].second = std::clamp(nodePositions[i].second, 50.0, canvasHeight - 50.0);
        }
    }
}

void winrt::FYP_V2::implementation::HomePage::visualizeGraph()
{
	GraphCanvas().Children().Clear();

    int** adjMatrix = graph.getAdjacencyMatrix();
	int numOfVertices = graph.getNumOfVertices();

	if (numOfVertices == 0) {
		return;
	}

    if (nodePositions.empty()) {
		getNodesPosition(adjMatrix, numOfVertices);
    }

    for (int i = 0; i < numOfVertices; i++) {
        for (int j = i + 1; j < numOfVertices; j++) {
            if (adjMatrix[i][j] != 0) {
                const auto& [x1, y1] = nodePositions[i];
                const auto& [x2, y2] = nodePositions[j];

                double dx = x2 - x1;
                double dy = y2 - y1;
                double length = std::hypot(dx, dy);
                if (length < 1e-5) continue;

                double unitX = dx / length;
                double unitY = dy / length;
                double adjustedX2 = x2 - unitX * 20;
                double adjustedY2 = y2 - unitY * 20;

                Line line;
                line.X1(x1);
                line.Y1(y1);
                line.X2(adjustedX2);
                line.Y2(adjustedY2);
                line.Stroke(Media::SolidColorBrush(Colors::Black()));
                line.StrokeThickness(2);
                GraphCanvas().Children().Append(line);

                TextBlock text;
                text.Text(winrt::to_hstring(adjMatrix[i][j]));
                text.Foreground(SolidColorBrush(Colors::Red()));
                Canvas::SetLeft(text, (adjustedX2 + x1) / 2);
                Canvas::SetTop(text, (adjustedY2 + y1) / 2);
                Canvas::SetZIndex(text, 2);
                GraphCanvas().Children().Append(text);

                Path arrow;
                const double arrowSize = 10.0;

                winrt::Windows::Foundation::Point angle1(adjustedX2, adjustedY2);
                winrt::Windows::Foundation::Point angle2(
                    angle1.X - unitX * arrowSize - unitY * arrowSize,
                    angle1.Y - unitY * arrowSize + unitX * arrowSize
                );
                winrt::Windows::Foundation::Point angle3(
                    angle1.X - unitX * arrowSize + unitY * arrowSize,
                    angle1.Y - unitY * arrowSize - unitX * arrowSize
                );

                PathFigure arrowFigure;
                arrowFigure.StartPoint(angle1);
                LineSegment leftSegment;
				leftSegment.Point(angle2);
                arrowFigure.Segments().Append(leftSegment);

				LineSegment rightSegment;
				rightSegment.Point(angle3);
                arrowFigure.Segments().Append(rightSegment);

                arrowFigure.IsClosed(true);

                PathGeometry arrowGeometry;
                arrowGeometry.Figures().Append(arrowFigure);
                arrow.Data(arrowGeometry);
                arrow.Fill(SolidColorBrush(Colors::Black()));

                Canvas::SetZIndex(arrow, 1);
                GraphCanvas().Children().Append(arrow);
            }
        }
    }

    for (int i = 0; i < numOfVertices; i++) {
        const auto& [x, y] = nodePositions[i];

        Shapes::Ellipse ellipse;
        ellipse.Width(40);
        ellipse.Height(40);
        ellipse.Fill(SolidColorBrush(Colors::Blue()));
        Canvas::SetLeft(ellipse, x - 20);
        Canvas::SetTop(ellipse, y - 20);
        Canvas::SetZIndex(ellipse, 0);
        GraphCanvas().Children().Append(ellipse);

        TextBlock text;
        text.Text(winrt::to_hstring(currentNodes[i].name));
        text.Foreground(SolidColorBrush(Colors::Red()));
        Canvas::SetLeft(text, x - 25);
        Canvas::SetTop(text, y + 20);
        Canvas::SetZIndex(text, 2);
        GraphCanvas().Children().Append(text);
    }

	ZoomGraph(0.1);
}

void winrt::FYP_V2::implementation::HomePage::GraphScrollViewer_PointerWheelChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
{
    auto point = e.GetCurrentPoint(GraphScrollViewer());
    if (point.Properties().IsHorizontalMouseWheel()) return;

    const double delta = point.Properties().MouseWheelDelta();
    ZoomGraph(delta > 0 ? 1.1 : 0.9);
    e.Handled(true);
}

void winrt::FYP_V2::implementation::HomePage::ZoomGraph(double zoomDelta) {
    float zoomFactor = GraphScrollViewer().ZoomFactor();
    zoomFactor *= zoomDelta;
    zoomFactor = std::clamp(zoomFactor, (float) 0.1, (float) 10.0);
    GraphScrollViewer().ChangeView(
        nullptr,
        nullptr,
        zoomFactor
    );
}

void winrt::FYP_V2::implementation::HomePage::PrevBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	
}

void winrt::FYP_V2::implementation::HomePage::RemoveBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}

void winrt::FYP_V2::implementation::HomePage::NextBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}