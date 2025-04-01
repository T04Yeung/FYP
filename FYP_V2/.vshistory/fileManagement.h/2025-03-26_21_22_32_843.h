#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_set>

struct Node {
	int id;
	std::string name;

public:
	Node() {
		id = -1;
		name = "";
	}

	Node(int id, std::string Name) {
		this->id = id;
		this->name = Name;
	};

	Node(const Node& other) : id(other.id), name(other.name) {}

	bool operator==(const Node& other) const {
		return id == other.id && name == other.name;
	}
};

struct Edge {
	int id;
	int from, to, weight;

public:
	Edge() {
		id = -1;
		from = to = weight = -1;
	}

	Edge(int id, int from, int to, int weight) {
		this->id = id;
		this->from = from;
		this->to = to;
		this->weight = weight;
	}

	Edge(const Edge& other) : id(other.id), from(other.from), to(other.to), weight(other.weight) {}

	bool operator==(const Edge& other) const {
		return id == other.id && from == other.from && to == other.to && weight == other.weight;
	}

	bool operator<(const Edge& other) const {
		return std::tie(id, from, to, weight) < std::tie(other.id, other.from, other.to, other.weight);
	}
};

namespace nlohmann {
	template <>
	struct adl_serializer<Node> {
		static void to_json(json& j, const Node& n) {
			j = json{ {"id", n.id}, {"name", n.name} };
		}
		static void from_json(const json& j, Node& n) {
			j.at("id").get_to(n.id);
			j.at("name").get_to(n.name);
		}
	};
}

namespace std {
	template <>
	struct hash<Edge> {
		std::size_t operator()(const Edge& edge) const {
			return std::hash<int>()(edge.id) ^ std::hash<int>()(edge.from) ^ std::hash<int>()(edge.to) ^ std::hash<int>()(edge.weight);
		}
	};
}

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