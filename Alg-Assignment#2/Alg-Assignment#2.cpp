#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <fstream>

using namespace std;

// Bubble Sort
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
}

// Iterative Merge Sort
void merge(vector<int>& arr, int l, int m, int r) {
    vector<int> left(arr.begin() + l, arr.begin() + m + 1);
    vector<int> right(arr.begin() + m + 1, arr.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < left.size() && j < right.size())
        arr[k++] = (left[i] <= right[j]) ? left[i++] : right[j++];
    while (i < left.size()) arr[k++] = left[i++];
    while (j < right.size()) arr[k++] = right[j++];
}

void mergeSort(vector<int>& arr) {
    int n = arr.size();
    for (int curr_size = 1; curr_size < n; curr_size *= 2)
        for (int left = 0; left < n - 1; left += 2 * curr_size) {
            int mid = min(left + curr_size - 1, n - 1);
            int right = min(left + 2 * curr_size - 1, n - 1);
            merge(arr, left, mid, right);
        }
}

// Iterative Quick Sort
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high], i = low - 1;
    for (int j = low; j < high; j++)
        if (arr[j] <= pivot)
            swap(arr[++i], arr[j]);
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<int>& arr) {
    int n = arr.size();
    vector<pair<int, int>> stk;
    stk.push_back({ 0, n - 1 });
    while (!stk.empty()) {
        pair<int, int> range = stk.back(); stk.pop_back();
        int low = range.first, high = range.second;
        if (low < high) {
            if (high - low <= 64) {
                for (int i = low + 1; i <= high; i++) {
                    int key = arr[i], j = i - 1;
                    while (j >= low && arr[j] > key)
                        arr[j + 1] = arr[j--];
                    arr[j + 1] = key;
                }
            }
            else {
                int pi = partition(arr, low, high);
                stk.push_back({ low, pi - 1 });
                stk.push_back({ pi + 1, high });
            }
        }
    }
}

// Generate Test Data using time-based seed
vector<int> generateData(int size, const string& type) {
    vector<int> data(size);
    srand(static_cast<unsigned>(time(0)));
    if (type == "random") {
        for (int& d : data) d = rand() % 1000;
    }
    else if (type == "sorted") {
        for (int i = 0; i < size; i++) data[i] = i;
    }
    else if (type == "partially_sorted") {
        for (int i = 0; i < size; i++) data[i] = i;
        for (int i = 0; i < size / 10; i++)
            swap(data[rand() % size], data[rand() % size]);
    }
    else if (type == "reverse_sorted") {
        for (int i = 0; i < size; i++) data[i] = size - i;
    }
    return data;
}

// Measure Time
double measureTime(void (*sortFunc)(vector<int>&), vector<int> arr) {
    auto data = arr;
    auto start = chrono::high_resolution_clock::now();
    sortFunc(data);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(end - start).count();
}

int main() {
    vector<string> types = { "random", "sorted", "partially_sorted", "reverse_sorted" };
    ofstream outFile("results_extended.txt");
    if (!outFile.is_open()) {
        cerr << "Error writing to file.\n";
        return 1;
    }

    outFile << "Size,Type,BubbleSortTime,MergeSortTime,QuickSortTime,Status\n";

    int size = 100;
    while (true) {
        bool success = true;
        cout << "Testing size: " << size << endl;
        for (const auto& type : types) {
            double bubbleTime = 0, mergeTime = 0, quickTime = 0;
            for (int i = 0; i < 5; ++i) {
                cout << "Processing type: " << type << ", run: " << (i + 1) << endl;
                try {
                    vector<int> testAlloc(size); // «· Õﬁﬁ „‰ «· Œ’Ì’
                    auto data = generateData(size, type);
                    bubbleTime += measureTime(bubbleSort, data);
                    data = generateData(size, type);
                    mergeTime += measureTime(mergeSort, data);
                    data = generateData(size, type);
                    quickTime += measureTime(quickSort, data);
                }
                catch (const bad_alloc& e) {
                    cout << "Memory allocation failed at size " << size << ", type " << type << endl;
                    success = false;
                    break;
                }
                catch (const exception& e) {
                    cout << "Error at size " << size << ", type " << type << ": " << e.what() << endl;
                    success = false;
                    break;
                }
            }
            if (!success) break;
            bubbleTime /= 5;
            mergeTime /= 5;
            quickTime /= 5;
            outFile << size << "," << type << "," << bubbleTime << "," << mergeTime << "," << quickTime << ",Success\n";
        }
        if (!success) break;
        size += 10;
        if (size > 5000) break;
    }

    outFile.close();
    cout << "Extended results saved to results_extended.txt\n";

    return 0;
}