#pragma once

#include <vector>
#include <algorithm>
#include <omp.h>

class UnionFind {
private:
	std::vector<int> elements;
	omp_lock_t lock;
public:
	UnionFind(int size) {
		elements.resize(size);
		for (int i = 0; i < size; i++) {
			elements[i] = -1;
		}
		omp_init_lock(&lock);
	}

	~UnionFind() {
		omp_destroy_lock(&lock);
	}

	int find(int element) {
		omp_set_lock(&lock);
		if (elements[element] != -1) {
			element = find(elements[element]);
		}
		omp_unset_lock(&lock);
		return element;
	}

	int unite(int setA, int setB) {
		omp_set_lock(&lock);
		elements[setA] = setB;
		omp_unset_lock(&lock);
	}
};