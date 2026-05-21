#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <iostream>
#include <cstdlib>
#include <cassert>

template <typename T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;

    void resize() {
        capacity = capacity + (capacity / 2) + 1;
        T* newData = new T[capacity];
        for (int i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    explicit DynamicArray(int initialCapacity = 2) : size(0), capacity(initialCapacity) {
        data = new T[capacity];
    }

    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    ~DynamicArray() {
        delete[] data;
    }

    void append(T value) {
        if (size == capacity) {
            resize();
        }
        data[size] = value;
        size++;
    }

    void removeLast() {
        if (size > 0) {
            size--;
        }
    }

    T& operator[](int index) {
        assert(index >= 0 && index < size && "Fatal Error: Index out of bounds!");
        return data[index];
    }


    const T& operator[](int index) const {
        assert(index >= 0 && index < size && "Fatal Error: Index out of bounds!");
        return data[index];
    }

    [[nodiscard]] int getSize() const {
        return size;
    }

    void display() const {
        for (int i = 0; i < size; i++) {
            std::cout << data[i] << " ";
        }
        std::cout << "\n";
    }
};

#endif