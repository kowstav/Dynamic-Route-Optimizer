#include "../include/segment_tree.h"
#include <algorithm> // For std::min/max if used for other operations

// Constructor: builds the segment tree from an initial array of values.
SegmentTree::SegmentTree(const std::vector<double>& arr) {
    // Store the size of the original array.
    n = arr.size();
    // The size of the segment tree array is typically 4*n for worst-case.
    tree.resize(4 * n);
    // If the input array is not empty, build the tree.
    if (n > 0) {
        // Call the recursive build function.
        build(arr, 1, 0, n - 1);
    }
}

// Builds the segment tree.
void SegmentTree::build(const std::vector<double>& arr, int node, int start, int end) {
    // If the current segment is a leaf node.
    if (start == end) {
        // Assign the array value to the tree node.
        tree[node] = arr[start];
    } else {
        // Calculate the midpoint of the current segment.
        int mid = (start + end) / 2;
        // Recursively build the left child.
        build(arr, 2 * node, start, mid);
        // Recursively build the right child.
        build(arr, 2 * node + 1, mid + 1, end);
        // The value of the current node is the sum of its children (for sum query).
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }
}

// Updates the value at a specific index in the original array and propagates the change up the tree.
void SegmentTree::update(int node, int start, int end, int idx, double val) {
    // If the current segment is a leaf node and it's the target index.
    if (start == end) {
        // Update the tree node with the new value.
        tree[node] = val;
        // (Implicitly, arr[idx] = val; if an underlying array were also maintained here)
    } else {
        // Calculate the midpoint of the current segment.
        int mid = (start + end) / 2;
        // If the target index is in the left child's range.
        if (start <= idx && idx <= mid) {
            // Recursively update the left child.
            update(2 * node, start, mid, idx, val);
        } else {
            // Recursively update the right child.
            update(2 * node + 1, mid + 1, end, idx, val);
        }
        // Recompute the value of the current node based on its children.
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }
}

// Queries the sum (or other operation) over a given range [l, r].
double SegmentTree::query(int node, int start, int end, int l, int r) {
    // If the current segment is outside the query range.
    if (r < start || end < l) {
        // Return 0 (identity for sum).
        return 0;
    }
    // If the current segment is completely within the query range.
    if (l <= start && end <= r) {
        // Return the value of the current tree node.
        return tree[node];
    }
    // If the current segment partially overlaps with the query range.
    int mid = (start + end) / 2;
    // Query the left child.
    double p1 = query(2 * node, start, mid, l, r);
    // Query the right child.
    double p2 = query(2 * node + 1, mid + 1, end, l, r);
    // Return the sum of results from children.
    return p1 + p2;
}

// Public interface for update.
void SegmentTree::updateValue(int idx, double val) {
    // Check for valid index.
    if (idx < 0 || idx >= n) return;
    // Call the recursive update function.
    update(1, 0, n - 1, idx, val);
}

// Public interface for query.
double SegmentTree::queryRange(int l, int r) {
    // Check for valid range.
    if (l < 0 || r >= n || l > r) return 0; // Or throw error
    // Call the recursive query function.
    return query(1, 0, n - 1, l, r);
}