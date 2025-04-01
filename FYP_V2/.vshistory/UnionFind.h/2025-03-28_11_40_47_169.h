#pragma once

#include <vector>
#include <algorithm>
#include <omp.h>

class UnionFind {
private:
	std::vector<int> elements;
	std::vector<omp_lock_t> locks;
public:
	UnionFind(int size) {
		elements.resize(size);
		for (int i = 0; i < size; i++) {
			elements[i] = -1;
			omp_init_lock(&locks[i]);
		}
	}

	~UnionFind() {
		for (auto lock : locks) {
			omp_destroy_lock(&lock);
		}
	}

	int find(int element) {
		if (elements[element] != -1) {
			omp_set_lock(&locks[element]);
			element = find(elements[element]);
			omp_unset_lock(&locks[element]);
		}
		return element;
	}

	void unite(int setA, int setB) {
		omp_set_lock(&locks[setA]);
		omp_set_lock(&locks[setB]);
		elements[setA] = setB;
		omp_unset_lock(&locks[setB]);
		omp_unset_lock(&locks[setA]);
	}
};