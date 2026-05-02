#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <cstdlib>

template <typename T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;
    // Helper method to expand the capacity linearly when the array is full
    // Adding only one element may be not time effective
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
    DynamicArray() : size(0), capacity(2) {
        data = new T[capacity];
    }

    // Prevent shallow copies
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

    // Overload [] operator for fast access
    T& operator[](int index) {
        if (index < 0 || index >= size) {
            std::cerr << "Fatal Error: Index " << index << " out of bounds in DynamicArray! Terminating program.\n";
            std::exit(EXIT_FAILURE);
        }
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