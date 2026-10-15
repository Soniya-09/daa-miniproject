// LP-III : Design & Analysis of Algorithms (410241) - Mini Project
// Topic 2: Merge Sort & Multithreaded Merge Sort
// Version 1 : Normal (single-threaded) Merge Sort
// Version 2 : Multithreaded Merge Sort (std::thread, depth-limited)

#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

long long comparisons = 0;     // counted only by the normal merge sort

// ---------- Merge two sorted halves a[l..m] and a[m+1..r] ----------
void merge(vector<int>& a, int l, int m, int r, bool count) {
    vector<int> left(a.begin() + l, a.begin() + m + 1);
    vector<int> right(a.begin() + m + 1, a.begin() + r + 1);

    size_t i = 0, j = 0;
    int k = l;
    while (i < left.size() && j < right.size()) {
        if (count) comparisons++;
        if (left[i] <= right[j]) a[k++] = left[i++];
        else                     a[k++] = right[j++];
    }
    while (i < left.size())  a[k++] = left[i++];    // copy what is left
    while (j < right.size()) a[k++] = right[j++];
}

// ---------- 1. Normal Merge Sort ----------
void mergeSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(a, l, m);
    mergeSort(a, m + 1, r);
    merge(a, l, m, r, true);
}
