#include "../include/union_find.h"

// Constructor: initializes Union-Find for a given set of node IDs.
UnionFind::UnionFind(const std::vector<int>& nodeIds) {
    // For each node ID in the input vector.
    for (int id : nodeIds) {
        // Create a new set for this node.
        makeSet(id);
    }
}

// Makes a new set containing only the element 'v'.
void UnionFind::makeSet(int v) {
    // The parent of 'v' is 'v' itself.
    parent[v] = v;
    // The initial rank of 'v' is 0.
    rank[v] = 0;
}

// Finds the representative (root) of the set containing 'v' with path compression.
int UnionFind::findSet(int v) {
    // If 'v' is not its own parent, it's not the root.
    if (v == parent[v])
        // If 'v' is its own parent, it is the root.
        return v;
    // Recursively find the root and apply path compression.
    return parent[v] = findSet(parent[v]);
}

// Unites the sets containing elements 'a' and 'b' using union by rank.
void UnionFind::uniteSets(int a, int b) {
    // Find the representative of the set containing 'a'.
    a = findSet(a);
    // Find the representative of the set containing 'b'.
    b = findSet(b);
    // If 'a' and 'b' are not already in the same set.
    if (a != b) {
        // If rank of 'a' is less than rank of 'b'.
        if (rank[a] < rank[b])
            // Swap 'a' and 'b'.
            std::swap(a, b);
        // Make 'b' a child of 'a'.
        parent[b] = a;
        // If ranks were equal, increment rank of 'a'.
        if (rank[a] == rank[b])
            // Increment rank of the new root.
            rank[a]++;
    }
}