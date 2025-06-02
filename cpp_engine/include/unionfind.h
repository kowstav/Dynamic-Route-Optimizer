#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>
#include <map>
#include <numeric> // For std::iota

class UnionFind {
public:
    std::map<int, int> parent;
    std::map<int, int> rank;

    // Constructor: initializes Union-Find for a given set of node IDs.
    UnionFind(const std::vector<int>& nodeIds);
    void makeSet(int v);
    int findSet(int v);
    void uniteSets(int a, int b);
};

#endif