#include "MergeSort.cpp"
#include <iostream>

using namespace std;

// alias declaration
using IntLinkedList = LinkedList<int>;        // since C++ 11

// or typedef LinkedList<int> IntLinkedList;

template<typename T>
void test(T testCase) {
    cout << "Test Case " << endl;

    IntLinkedList ll(testCase);
    ll.print();

    ll.mergeSort(nullptr);

    cout << "   ->   ";

    ll.print();

    cout << endl
         << endl;
}

int main() {
    vector<vector<int>> testcases = {
            {},
            {-1},
            {2,  1},
            {-1, -3, 10, 9},
            {10, 2,  40, -10, -3, -5, 3, 100},
    };
    for (auto t : testcases) {
        test(t);
    }
    return 0;
}