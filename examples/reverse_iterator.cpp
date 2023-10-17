#include <iostream>

class Iterator {
    int *keys;
    int index;
public:
    explicit Iterator(int *k, int i) : keys(k), index(i) {}

    int operator*() const {
        return keys[index];
    }

    Iterator &operator++() {
        ++index;
        return *this;
    }

    Iterator &operator--() {
        --index;
        return *this;
    }

    bool operator!=(const Iterator &other) const {
        return index != other.index;
    }
};

class Iterable {
    int keys[5] = {1, 2, 3, 4, 5};
    int capacity = 5;
public:
    Iterator begin() {
        return Iterator(keys, 0);
    }

    Iterator end() {
        return Iterator(keys, capacity);
    }

    Iterator rbegin() {
        return Iterator(keys, capacity - 1);
    }

    Iterator rend() {
        return Iterator(keys, -1);
    }
};

int main() {
    Iterable iterable;

    std::cout << "Forward iteration:" << std::endl;
    for (int it: iterable) {
        std::cout << it << ' ';
    }
    std::cout << std::endl;

    std::cout << "Reverse iteration:" << std::endl;
    for (auto it = iterable.rbegin(); it != iterable.rend(); --it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;

    return 0;
}
