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

// ---------- 2. Multithreaded Merge Sort ----------
// depth = how many more levels may create a new thread.
// depth 3 -> up to 2^3 = 8 threads work at the same time.
const int THRESHOLD = 1000;    // small parts are sorted without a new thread

void parallelMergeSort(vector<int>& a, int l, int r, int depth) {
    if (l >= r) return;
    int m = l + (r - l) / 2;

    if (depth > 0 && (r - l + 1) > THRESHOLD) {
        // left half in a new thread, right half in this thread
        thread t(parallelMergeSort, ref(a), l, m, depth - 1);
        parallelMergeSort(a, m + 1, r, depth - 1);
        t.join();                               // wait for the left half
    } else {
        parallelMergeSort(a, l, m, 0);          // no more threads: normal recursion
        parallelMergeSort(a, m + 1, r, 0);
    }
    merge(a, l, m, r, false);
}

// ---------- Input generators ----------
vector<int> randomArray(int n) {
    mt19937 gen(42);                            // fixed seed so results can be repeated
    uniform_int_distribution<int> dist(1, 1000000);
    vector<int> a(n);
    for (int& x : a) x = dist(gen);
    return a;
}

vector<int> sortedArray(int n) {                // best case: fewest comparisons
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = i + 1;
    return a;
}

vector<int> reverseArray(int n) {
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = n - i;
    return a;
}

// Worst case: every merge must compare until both halves are empty.
// Build it by "un-merging" a sorted array: even positions go left, odd go right.
void makeWorst(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    vector<int> left, right;
    for (int i = l; i <= r; i++) {
        if ((i - l) % 2 == 0) left.push_back(a[i]);
        else                  right.push_back(a[i]);
    }
    int k = l;
    for (int x : left)  a[k++] = x;
    for (int x : right) a[k++] = x;
    makeWorst(a, l, m);
    makeWorst(a, m + 1, r);
}

vector<int> worstArray(int n) {
    vector<int> a = sortedArray(n);
    makeWorst(a, 0, n - 1);
    return a;
}
