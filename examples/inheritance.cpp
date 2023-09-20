#include <iostream>
#include <memory>

class Animal {
public:
    virtual void speak() const = 0;

    // Every base class should have a virtual destructor.
    ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "Dog::speak" << std::endl;
    }

    ~Dog() {
        std::cout << "Dog::~Dog" << std::endl;
    }
};

class Cat : public Animal {
public:
    void speak() const override {
        std::cout << "Cat::speak" << std::endl;
    }

    ~Cat() {
        std::cout << "Cat::~Cat" << std::endl;
    }
};

void try_bark(const Animal &a) {
    if (const auto *d = dynamic_cast<const Dog *>(&a)) {
        d->speak();
    } else {
        std::cout << "Couldn't bark" << std::endl;
    }
}

int main() {
    std::unique_ptr<Cat> b = std::make_unique<Cat>();
    std::unique_ptr<Animal> c = std::make_unique<Cat>();
    Dog d;
    try_bark(*b);
    try_bark(*c);
    try_bark(d);
    // Which object is not destroyed? Why?
    return 0;
}
