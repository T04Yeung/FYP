#pragma once
#include "nlohmann/json.hpp"
#include <cuda.h>
#include <cuda_runtime.h>

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
	__host__ __device__ Edge() {
		id = -1;
		from = to = weight = -1;
	}

	__host__ __device__ Edge(int id, int from, int to, int weight) {
		this->id = id;
		this->from = from;
		this->to = to;
		this->weight = weight;
	}

	__host__ __device__ Edge(const Edge& other) : id(other.id), from(other.from), to(other.to), weight(other.weight) {}

	__host__ __device__ bool operator==(const Edge& other) const {
		return id == other.id && from == other.from && to == other.to && weight == other.weight;
	}

	__host__ __device__ bool operator<(const Edge& other) const {
		return weight < other.weight;
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