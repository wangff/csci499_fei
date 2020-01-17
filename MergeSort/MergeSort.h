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

public:
    LinkedList() : root(nullptr) {};

    LinkedList(std::vector<T>);

    ~LinkedList();

    Node *mergeSort(Node *);

    void print();
};