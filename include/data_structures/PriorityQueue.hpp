#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include "data_structures/Array.hpp"
#include <stdexcept>

template <typename T>
class PriorityQueue {
private:
    DynamicArray<T> heap;

    // Helper functions to navigate
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    void swap(int i, int j) {
        T temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    // Restoring heap up
    void heapifyUp(int index) {
        while (index > 0 && heap[parent(index)] > heap[index]) {
            swap(parent(index), index);
            index = parent(index);
        }
    }

    // Restoring heap down
    void heapifyDown(int index) {
        int minIndex = index;
        int left = leftChild(index);
        int right = rightChild(index);
        int size = heap.getSize();

        if (left < size && heap[minIndex] > heap[left]) {
            minIndex = left;
        }
        if (right < size && heap[minIndex] > heap[right]) {
            minIndex = right;
        }
        if (index != minIndex) {
            swap(index, minIndex);
            heapifyDown(minIndex);
        }
    }

public:
    PriorityQueue(int initialCapacity = 100) : heap(initialCapacity) {}

    void push(T value) {
        heap.append(value);
        heapifyUp(heap.getSize() - 1);
    }

    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("PriorityQueue is empty!");
        }

        T result = heap[0];

        // Replace root with last element and fix down
        heap[0] = heap[heap.getSize() - 1];
        heap.removeLast();

        if (!isEmpty()) {
            heapifyDown(0);
        }

        return result;
    }

    bool isEmpty() const {
        return heap.getSize() == 0;
    }
};

#endif