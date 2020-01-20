#include "mergesort.h"
#include <iostream>

using namespace std;

// Constructor
template<typename T>
LinkedList<T>::LinkedList(vector<T> vec) {
    root = nullptr;
    for (auto v: vec) {
        add(v);
    }
}

// Destructor
template<typename T>
LinkedList<T>::~LinkedList() {
    while (root) {
        auto next = root->next;
        delete root;
        root = next;
    }
}

template<typename T>
void LinkedList<T>::add(T const &val) {
    // if list is empty
    if (!root) {
        root = new Node(val);
        return;
    }
    // find the last node
    Node *cur = root;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = new Node(val);
}

template<typename T>
typename LinkedList<T>::Node *LinkedList<T>::middlePoint(typename LinkedList<T>::Node *node) {
    Node *slow;
    Node *fast;
    slow = node;
    fast = node->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

/*
    Merge two linked list, and return head of merged list.
*/
template<typename T>
typename LinkedList<T>::Node *LinkedList<T>::merge(typename LinkedList<T>::Node *lp, typename LinkedList<T>::Node *rp) {
    if (!lp)
        return rp;
    if (!rp)
        return lp;

    // final merged node
    Node *dummy = new Node();
    auto cur = dummy;
    while (lp || rp) {
        if (!rp || (lp && (lp->val < rp->val))) {
            cur->next = lp;
            lp = lp->next;
        } else {
            cur->next = rp;
            rp = rp->next;
        }
        cur = cur->next;
    }
    cur->next = nullptr;
    return dummy->next;
}

/*
    Merge Sort.  Specify whitch node is the head of list.
*/
template<typename T>
typename LinkedList<T>::Node *LinkedList<T>::mergeSort(typename LinkedList<T>::Node *node) {
    // start from root.
    if (!node) {
        node = root;
    }
    if (!node || !(node->next)) {
        return node;
    }

    // split the current linkedlinst
    Node *left;
    Node *right;

    Node *middle = middlePoint(node);
    left = node;
    right = middle->next;
    middle->next = nullptr;

    left = mergeSort(left);
    right = mergeSort(right);
    Node *res = merge(left, right);

    // first node of list may change.
    root = res;
    return res;
}

template<typename T>
void LinkedList<T>::sort()
{
    mergeSort(nullptr);
}

template<typename T>
void LinkedList<T>::print() {
    // empty list.
    if (!root) {
        return;
    }

    Node *cur = root;
    while (cur->next) {
        cout << cur->val << ',';
        cur = cur->next;
    }
    cout << cur->val;
}

template LinkedList<int>::LinkedList(vector<int> vec);
template LinkedList<int>::~LinkedList();
template void LinkedList<int>::sort();
template void LinkedList<int>::print();
