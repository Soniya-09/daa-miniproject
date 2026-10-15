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

// ---------- Helpers ----------
void printArray(const vector<int>& a) {
    for (int x : a) cout << x << " ";
    cout << "\n";
}

int threadsUsed(int depth) { return 1 << depth; }

// Runs both versions on the same input and prints time, speed-up, comparisons.
void runBoth(const vector<int>& input, int depth, bool show) {
    vector<int> a1, a2;
    int n = input.size();

    // run 5 times and keep the best time (first run is slow due to cache warm-up)
    double sTime = DBL_MAX, pTime = DBL_MAX;
    for (int rep = 0; rep < 5; rep++) {
        a1 = input; a2 = input; comparisons = 0;
        auto t1 = high_resolution_clock::now();
        mergeSort(a1, 0, n - 1);
        auto t2 = high_resolution_clock::now();
        parallelMergeSort(a2, 0, n - 1, depth);
        auto t3 = high_resolution_clock::now();
        sTime = min(sTime, duration<double, milli>(t2 - t1).count());
        pTime = min(pTime, duration<double, milli>(t3 - t2).count());
    }

    if (show) {
        cout << "\nSorted (normal)        : "; printArray(a1);
        cout << "Sorted (multithreaded) : "; printArray(a2);
    }
    cout << fixed << setprecision(4);
    cout << "Normal Merge Sort time        (ms) : " << sTime << "\n";
    cout << "Multithreaded Merge Sort time (ms) : " << pTime
         << "  [up to " << threadsUsed(depth) << " threads]\n";
    cout << "Comparisons (normal)               : " << comparisons << "\n";
    cout << setprecision(2);
    if (pTime > 0) cout << "Speed-up (normal / multithreaded)  : " << sTime / pTime << "\n";
    else           cout << "Speed-up (normal / multithreaded)  : too fast to measure\n";
    if (n <= THRESHOLD)
        cout << "Note: n <= THRESHOLD (" << THRESHOLD
             << "), so no extra thread is created for this small array.\n";
    cout << "Both results same and sorted?      : "
         << ((a1 == a2 && is_sorted(a1.begin(), a1.end())) ? "YES" : "NO") << "\n";
}

void benchmark(int depth) {
    int sizes[] = {1000, 10000, 100000, 1000000, 5000000};
    string names[] = {"Random", "Best (sorted)", "Reverse sorted", "Worst case"};

    cout << "\nCPU cores available : " << thread::hardware_concurrency()
         << " | Threads used : " << threadsUsed(depth) << "\n";
    cout << left << setw(10) << "n" << setw(16) << "Input"
         << setw(14) << "Comparisons" << setw(14) << "Normal(ms)"
         << setw(14) << "Thread(ms)" << "Speed-up\n";
    cout << string(76, '-') << "\n";

    for (int n : sizes) {
        for (int c = 0; c < 4; c++) {
            vector<int> in = (c == 0) ? randomArray(n) : (c == 1) ? sortedArray(n)
                           : (c == 2) ? reverseArray(n) : worstArray(n);
            vector<int> a1 = in, a2 = in;

            // run 3 times and keep the best time to reduce noise
            double sBest = DBL_MAX, pBest = DBL_MAX;
            long long cmp = 0;
            for (int rep = 0; rep < 3; rep++) {
                a1 = in; a2 = in; comparisons = 0;
                auto t1 = high_resolution_clock::now();
                mergeSort(a1, 0, n - 1);
                auto t2 = high_resolution_clock::now();
                parallelMergeSort(a2, 0, n - 1, depth);
                auto t3 = high_resolution_clock::now();
                sBest = min(sBest, duration<double, milli>(t2 - t1).count());
                pBest = min(pBest, duration<double, milli>(t3 - t2).count());
                cmp = comparisons;
            }
            if (a1 != a2) cout << "ERROR: results differ!\n";

            cout << left << setw(10) << n << setw(16) << names[c]
                 << setw(14) << cmp << fixed << setprecision(3)
                 << setw(14) << sBest << setw(14) << pBest
                 << setprecision(2) << sBest / pBest << "\n";
        }
    }

    // Effect of number of threads (n = 1,000,000 random)
    int n = 1000000;
    vector<int> in = randomArray(n);
    cout << "\nEffect of thread count (n = " << n << ", random input)\n";
    cout << left << setw(10) << "Threads" << setw(14) << "Time(ms)" << "Speed-up\n";
    cout << string(32, '-') << "\n";
    double base = 0;
    for (int d = 0; d <= 4; d++) {
        double best = DBL_MAX;
        for (int rep = 0; rep < 3; rep++) {
            vector<int> a = in;
            auto t1 = high_resolution_clock::now();
            parallelMergeSort(a, 0, n - 1, d);
            auto t2 = high_resolution_clock::now();
            best = min(best, duration<double, milli>(t2 - t1).count());
        }
        if (d == 0) base = best;
        cout << left << setw(10) << threadsUsed(d) << fixed << setprecision(3)
             << setw(14) << best << setprecision(2) << base / best << "\n";
    }
}
