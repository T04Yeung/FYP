﻿#include "pch.h"
#include "HomePage.xaml.h"
#include "SharedData.h"
#include "recordManagement.h"
#include "record.h"
#include "fileManagement.h"
#include "graphManagement.h"
#include "CPU_Algorithm.h"
#include "GPU_Algorithm.cuh"
#include "nlohmann/json.hpp"
#include <random>
#include <algorithm>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Input.Interop.h>
#include <winrt/Windows.UI.h>
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

winrt::Windows::Foundation::Rect GetVisibleArea(const ScrollViewer& scrollViewer) {
    const float zoom = scrollViewer.ZoomFactor();
    winrt::Windows::Foundation::Rect visibleArea = {
        (float) scrollViewer.HorizontalOffset() / zoom,
        (float) scrollViewer.VerticalOffset() / zoom,
        (float) scrollViewer.ViewportWidth() / zoom,
        (float) scrollViewer.ViewportHeight() / zoom
    };
    return visibleArea;
}

std::vector<winrt::Windows::Foundation::Point> generateNodePositions() {
    std::vector<winrt::Windows::Foundation::Point> nodePositions;
    int numOfVertices = graph.getNumOfVertices();
    nodePositions.resize(numOfVertices);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(100.0, 4900.0);
    for (int i = 0; i < numOfVertices; ++i) {
        nodePositions[i] = { distribution(gen), distribution(gen) };
    }

    const double k = 0.75 * sqrt(5000.0 * 5000.0 / numOfVertices);
    const int iterations = 50;
    double currentTemp = 5000.0;
    double cooling = 100.0;

    int** adjMatrix = graph.getAdjacencyMatrix();

    for (int i = 0; i < iterations; i++) {
        std::vector<winrt::Windows::Foundation::Point> disp(numOfVertices, { 0, 0 });

        for (int j = 0; j < numOfVertices; j++) {
            for (int k = 0; k < numOfVertices; k++) {
                if (j != k) {
                    double dx = nodePositions[k].X - nodePositions[j].X;
                    double dy = nodePositions[k].Y - nodePositions[j].Y;
                    double dist = max(1.0, std::hypot(dx, dy));
                    double rep = (k * k) / dist;
                    disp[i].X -= (dx / dist) * rep;
                    disp[i].Y -= (dy / dist) * rep;
                }

                if (adjMatrix[j][k] != 0) {
                    double dx = nodePositions[k].X - nodePositions[j].X;
                    double dy = nodePositions[k].Y - nodePositions[j].Y;
                    double dist = std::hypot(dx, dy);
                    double attr = (dist * dist) / k;
                    disp[i].X += (dx / dist) * attr;
                    disp[i].Y += (dy / dist) * attr;
                    disp[j].X -= (dx / dist) * attr;
                    disp[j].Y -= (dy / dist) * attr;
                }
            }
        }

        for (int j = 0; j < numOfVertices; j++) {
            double dispLength = max(1.0, std::hypot(disp[j].X, disp[j].Y));
            nodePositions[j].X += (disp[j].X / dispLength) * min(dispLength, currentTemp);
            nodePositions[j].Y += (disp[j].Y / dispLength) * min(dispLength, currentTemp);

            nodePositions[j].X = std::clamp(disp[j].X, 100.0f, 4900.0f);
            nodePositions[j].Y = std::clamp(disp[j].Y, 100.0f, 4900.0f);
        }

        currentTemp -= cooling;
    }

    return nodePositions;
}

void winrt::FYP_V2::implementation::HomePage::visualizeGraph()
{
	GraphCanvas().Children().Clear();

    int** adjMatrix = graph.getAdjacencyMatrix();
	int numOfVertices = graph.getNumOfVertices();

	if (numOfVertices == 0) {
		return;
	}

    std::vector<winrt::Windows::Foundation::Point> nodePositions = generateNodePositions();

    const float zoomFactor = GraphScrollViewer().ZoomFactor();
    const winrt::Windows::Foundation::Rect visibleArea = GetVisibleArea(GraphScrollViewer());
    const bool displayDetails = zoomFactor > 0.5;
    const bool displayArrows = zoomFactor > 0.8;
    const bool displayLabels = zoomFactor > 1.0;

    std::vector<bool> visibleNodes(numOfVertices, false);
    for (int i = 0; i < numOfVertices; ++i) {
        const double nodeSize = displayDetails ? 40.0 : 8.0;
        visibleNodes[i] =
            (nodePositions[i].X + nodeSize >= visibleArea.X) &&
            (nodePositions[i].X - nodeSize <= visibleArea.X + visibleArea.Width) &&
            (nodePositions[i].Y + nodeSize >= visibleArea.Y) &&
            (nodePositions[i].Y - nodeSize <= visibleArea.Y + visibleArea.Height);
    }

    for (int i = 0; i < numOfVertices; ++i) {
        if (!visibleNodes[i]) continue;

        for (int j = 0; j < numOfVertices; ++j) {
            if (adjMatrix[i][j] != 0 && visibleNodes[j]) {
                const auto& pos1 = nodePositions[i];
                const auto& pos2 = nodePositions[j];

                if (displayArrows) {
                    double dx = pos2.X - pos1.X;
                    double dy = pos2.Y - pos1.Y;
                    double length = std::hypot(dx, dy);
                    if (length < 1e-5) continue;

                    double unitX = dx / length;
                    double unitY = dy / length;
                    double adjustedX2 = pos2.X - unitX * 20;
                    double adjustedY2 = pos2.Y - unitY * 20;

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
                else {
                    Line line;
                    line.X1(pos1.X);
                    line.Y1(pos1.Y);
                    line.X2(pos2.X);
                    line.Y2(pos2.Y);
                    line.Stroke(SolidColorBrush(Colors::Gray()));
                    line.StrokeThickness(1);
                    GraphCanvas().Children().Append(line);
                }
            }

            const auto& pos = nodePositions[i];

            if (displayDetails) {
                Shapes::Ellipse ellipse;
                ellipse.Width(40);
                ellipse.Height(40);
                ellipse.Fill(SolidColorBrush(Colors::Blue()));
                Canvas::SetLeft(ellipse, pos.X - 20);
                Canvas::SetTop(ellipse, pos.Y - 20);
                GraphCanvas().Children().Append(ellipse);

                if (displayLabels) {
                    TextBlock text;
                    text.Text(winrt::to_hstring(currentNodes[i].name));
                    text.Foreground(SolidColorBrush(Colors::White()));
                    Canvas::SetLeft(text, pos.X - 10);
                    Canvas::SetTop(text, pos.Y - 10);
                    GraphCanvas().Children().Append(text);
                }
            }
            else {
                Shapes::Ellipse ellipse;
                ellipse.Width(8);
                ellipse.Height(8);
                ellipse.Fill(SolidColorBrush(Colors::LightBlue()));
                Canvas::SetLeft(ellipse, pos.X - 4);
                Canvas::SetTop(ellipse, pos.Y - 4);
                GraphCanvas().Children().Append(ellipse);
            }
        }
    }
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
    auto scrollViewer = GraphScrollViewer();
    float newZoom = scrollViewer.ZoomFactor() * zoomDelta;
    newZoom = std::clamp(newZoom, 0.1f, 10.0f);

    scrollViewer.ChangeView(
        nullptr,
        nullptr,
        newZoom
    );

    if ((newZoom > 0.5 && scrollViewer.ZoomFactor() <= 0.5) ||
        (newZoom <= 0.5 && scrollViewer.ZoomFactor() > 0.5) ||
        (newZoom > 0.8 && scrollViewer.ZoomFactor() <= 0.8) ||
        (newZoom <= 0.8 && scrollViewer.ZoomFactor() > 0.8)) {
        visualizeGraph();
    }
}

