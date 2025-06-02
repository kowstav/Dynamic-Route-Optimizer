#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H

#include <vector>
#include <functional>

class SegmentTree {
public:
    std::vector<double> tree;
    int n;

    // Constructor: builds the segment tree from an initial array of values.
    SegmentTree(const std::vector<double>& arr);
    void build(const std::vector<double>& arr, int node, int start, int end);
    void update(int node, int start, int end, int idx, double val);
    double query(int node, int start, int end, int l, int r);
    void updateValue(int idx, double val);
    double queryRange(int l, int r);
};

#endif