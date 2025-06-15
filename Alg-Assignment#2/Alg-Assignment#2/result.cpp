#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <algorithm>
#include <stack>

using namespace std;

// Bubble Sort Implementation
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Merge Sort Implementation (Iterative)
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int>& arr) {
    int n = arr.size();
    for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
        for (int left = 0; left < n - 1; left += 2 * curr_size) {
            int mid = min(left + curr_size - 1, n - 1);
            int right = min(left + 2 * curr_size - 1, n - 1);
            merge(arr, left, mid, right);
        }
    }
}

// Quick Sort Implementation (Iterative)
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<int>& arr) {
    int n = arr.size();
    stack<pair<int, int>> stk;
    stk.push({ 0, n - 1 });

    while (!stk.empty()) {
        int low = stk.top().first;
        int high = stk.top().second;
        stk.pop();

        if (low < high) {
            if (high - low <= 100) { // Use insertion sort for small arrays
                for (int i = low + 1; i <= high; i++) {
                    int key = arr[i];
                    int j = i - 1;
                    while (j >= low && arr[j] > key) {
                        arr[j + 1] = arr[j];
                        j--;
                    }
                    arr[j + 1] = key;
                }
            }
            else {
                int pi = partition(arr, low, high);
                stk.push({ low, pi - 1 });
                stk.push({ pi + 1, high });
            }
        }
    }
}

// Function to generate test data
vector<int> generateData(int size, string type) {
    vector<int> data(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000000);

    if (type == "random") {
        for (int i = 0; i < size; i++) {
            data[i] = dis(gen);
        }
    }
    else if (type == "sorted") {
        for (int i = 0; i < size; i++) {
            data[i] = i;
        }
    }
    else if (type == "partially_sorted") {
        for (int i = 0; i < size; i++) {
            data[i] = i;
        }
        for (int i = 0; i < size / 10; i++) {
            int idx1 = dis(gen) % size;
            int idx2 = dis(gen) % size;
            swap(data[idx1], data[idx2]);
        }
    }
    else if (type == "reverse_sorted") {
        for (int i = 0; i < size; i++) {
            data[i] = size - i;
        }
    }
    return data;
}

// Function to measure execution time for Bubble Sort
double measureTimeBubbleSort(vector<int> arr) {
    auto start = chrono::high_resolution_clock::now();
    bubbleSort(arr);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return duration.count();
}

// Function to measure execution time for Merge Sort
double measureTimeMergeSort(vector<int> arr) {
    auto start = chrono::high_resolution_clock::now();
    mergeSort(arr);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return duration.count();
}

// Function to measure execution time for Quick Sort
double measureTimeQuickSort(vector<int> arr) {
    auto start = chrono::high_resolution_clock::now();
    quickSort(arr);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return duration.count();
}

int main() {
    vector<int> sizes = { 100, 500, 1000 };
    vector<string> types = { "random", "sorted", "partially_sorted", "reverse_sorted" };
    ofstream outFile("results.txt");

    if (!outFile.is_open()) {
        cerr << "Error: Could not open results.txt for writing.\n";
        return 1;
    }

    outFile << "Size,Type,BubbleSortTime,MergeSortTime,QuickSortTime\n";

    for (int size : sizes) {
        for (const string& type : types) {
            cout << "Processing size: " << size << ", type: " << type << "\n";
            // Run each algorithm 5 times (reduced to minimize memory usage)
            double bubbleTime = 0, mergeTime = 0, quickTime = 0;
            int runs = 5;

            for (int i = 0; i < runs; i++) {
                cout << "Run " << i + 1 << " for size " << size << ", type " << type << "\n";
                vector<int> data;
                try {
                    data = generateData(size, type);
                }
                catch (const std::bad_alloc& e) {
                    cerr << "Memory allocation failed for size " << size << ", type " << type << "\n";
                    return 1;
                }
                vector<int> dataCopy = data;

                cout << "Bubble Sort run " << i + 1 << "\n";
                bubbleTime += measureTimeBubbleSort(data);
                data = dataCopy; // Reset data
                cout << "Merge Sort run " << i + 1 << "\n";
                mergeTime += measureTimeMergeSort(data);
                data = dataCopy; // Reset data
                cout << "Quick Sort run " << i + 1 << "\n";
                quickTime += measureTimeQuickSort(data);
            }

            bubbleTime /= runs;
            mergeTime /= runs;
            quickTime /= runs;

            outFile << size << "," << type << "," << bubbleTime << "," << mergeTime << "," << quickTime << "\n";
            outFile.flush();
        }
    }

    outFile.close();
    cout << "Results saved to results.txt\n";
    return 0;
}