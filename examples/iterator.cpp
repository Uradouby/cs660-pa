#include <iostream>
#include <vector>

class Iterable {
    using iterator = std::vector<int>::const_iterator;
    std::vector<int> values = {1, 2, 3, 4, 5};
public:

    iterator begin() const {
        return values.begin();
    }

    iterator end() const {
        return values.end();
    }
};

class CustomIterator {
    size_t index;
    size_t size;
    const std::vector<int> &values;
    const std::vector<bool> &skip;
public:
    CustomIterator(size_t i, size_t s, const std::vector<int> &values, const std::vector<bool> &skip)
            : index(i), size(s), values(values), skip(skip) {
        while (index < size && skip[index]) {
            index++;
        }
    }

    bool operator!=(const CustomIterator &other) const {
        return index != other.index;
    }

    CustomIterator &operator++() {
        do {
            index++;
        } while (index < size && skip[index]);
        return *this;
    }

    const int &operator*() const {
        return values[index];
    }
};

class CustomIterable {
    using iterator = CustomIterator;
    std::vector<bool> skip = {false, true, false, false, true};
    std::vector<int> values = {1, 2, 3, 4, 5};
public:
    iterator begin() const {
        return CustomIterator{0, values.size(), values, skip};
    }

    iterator end() const {
        return CustomIterator{values.size(), values.size(), values, skip};
    }
};

template<typename T>
void iterate(const T &items) {
    for (const auto &it: items) {
        std::cout << it << std::endl;
    }
    std::cout << std::endl;

    auto it = items.begin();
    while (it != items.end()) {
        const int &x = *it;
        std::cout << x << std::endl;
        ++it;
    }
    std::cout << std::endl;
}

int main() {
    Iterable iterable;
    iterate(iterable);
    CustomIterable customIterable;
    iterate(customIterable);
    return 0;
}
