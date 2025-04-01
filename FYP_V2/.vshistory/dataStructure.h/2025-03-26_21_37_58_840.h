#pragma once

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