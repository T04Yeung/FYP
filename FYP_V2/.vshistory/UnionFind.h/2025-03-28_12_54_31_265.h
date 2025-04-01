#pragma once

#include <vector>
#include <algorithm>
#include <omp.h>

class UnionFind {
private:
    std::vector<int> parent;
    std::vector<int> rank;
    std::vector<omp_lock_t> locks;

public:
    UnionFind(int size) : parent(size), rank(size, 0), locks(size) {
        for (int i = 0; i < size; i++) {
            parent[i] = i;
            omp_init_lock(&locks[i]);
        }
    }

    ~UnionFind() {
        for (auto& lock : locks) omp_destroy_lock(&lock);
    }

    int find(int element) {
        while (parent[element] != element) {
            omp_set_lock(&locks[element]);
            int p = parent[element];
            omp_unset_lock(&locks[element]);
            parent[element] = parent[p];
            element = p;
        }
        return element;
    }

    bool unite(int setA, int setB) {
        int rootA = find(setA);
        int rootB = find(setB);
        if (rootA == rootB) return false;

        omp_set_lock(&locks[rootA]);
        omp_set_lock(&locks[rootB]);

        if (rank[rootA] > rank[rootB]) {
            parent[rootB] = rootA;
        }
        else {
            parent[rootA] = rootB;
            if (rank[rootA] == rank[rootB]) rank[rootB]++;
        }

        omp_unset_lock(&locks[rootB]);
        omp_unset_lock(&locks[rootA]);
        return true;
    }
};