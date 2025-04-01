#pragma once

class Graph {
private:
	int numOfVertices;
	int** adjacencyMatrix;

public:
	Graph() {
		numOfVertices = 0;
		adjacencyMatrix = nullptr;
	}
	Graph(int numOfVertices) {
		this->numOfVertices = numOfVertices;
		adjacencyMatrix = new int*[numOfVertices];
		for (int i = 0; i < numOfVertices; i++) {
			adjacencyMatrix[i] = new int[numOfVertices];
		}

		for (int i = 0; i < numOfVertices; i++) {
			for (int j = 0; j < numOfVertices; j++) {
				adjacencyMatrix[i][j] = 0;
			}
		}
	}

	Graph(const Graph& other) : numOfVertices(other.numOfVertices) {
		adjacencyMatrix = new int* [numOfVertices];
		for (int i = 0; i < numOfVertices; i++) {
			adjacencyMatrix[i] = new int[numOfVertices];
			std::copy(other.adjacencyMatrix[i], other.adjacencyMatrix[i] + numOfVertices, adjacencyMatrix[i]);
		}
	}

	Graph& operator=(const Graph& other) {
		if (this == &other) return *this;

		for (int i = 0; i < numOfVertices; i++) {
			delete[] adjacencyMatrix[i];
		}
		delete[] adjacencyMatrix;

		numOfVertices = other.numOfVertices;
		adjacencyMatrix = new int* [numOfVertices];
		for (int i = 0; i < numOfVertices; i++) {
			adjacencyMatrix[i] = new int[numOfVertices];
			std::copy(other.adjacencyMatrix[i], other.adjacencyMatrix[i] + numOfVertices, adjacencyMatrix[i]);
		}

		return *this;
	}

	bool operator==(const Graph& other) const {
		if (numOfVertices != other.numOfVertices) return false;
		for (int i = 0; i < numOfVertices; i++) {
			for (int j = 0; j < numOfVertices; j++) {
				if (adjacencyMatrix[i][j] != other.adjacencyMatrix[i][j]) return false;
			}
		}
		return true;
	}

	~Graph() {
		for (int i = 0; i < numOfVertices; i++)
			delete[] adjacencyMatrix[i];
		delete[] adjacencyMatrix;
	}

	void addEdge(int vertex1, int vertex2, int weight) {
		adjacencyMatrix[vertex1][vertex2] = weight;
	}

	int getNumOfVertices() const { return numOfVertices; }
	void setNumOfVertices(int numOfVertices) { this->numOfVertices = numOfVertices; }

	int** getAdjacencyMatrix() const {
		return adjacencyMatrix;
	}
	void setAdjacencyMatrix(int** adjacencyMatrix) {
		this->adjacencyMatrix = adjacencyMatrix;
	}
};