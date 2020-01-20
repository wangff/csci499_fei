#ifndef MERGESORTDECL_H
#define MERGESORTDECL_H

#include <vector>

// Declaration
template<typename T>
class LinkedList {
    struct Node {
        Node *next;
        T val;

        Node() : next(nullptr) {};

        Node(const T &val) : val(val), next(nullptr) {};
    };

    Node *root;

    void add(T const &);

    Node *middlePoint(Node *);

    Node *merge(Node *, Node *);

    Node *mergeSort(Node *);

public:
    LinkedList() : root(nullptr) {};

    LinkedList(std::vector<T>);

    ~LinkedList();

    void print();

    void sort();
};

#endif