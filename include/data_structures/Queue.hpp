#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cassert>

// Fixed-capacity FIFO queue on a circular buffer:
// single allocation, O(1) enqueue/dequeue, indices wrap around modulo capacity.
// Counter "count" disambiguates the classic empty-vs-full ambiguity at front == rear.
template <typename T>
class Queue {
private:
    T* data;
    int capacity;
    int front;  // index of the oldest element
    int count;  // number of stored elements

public:
    explicit Queue(int maxElements) : capacity(maxElements), front(0), count(0) {
        data = new T[capacity];
    }

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    ~Queue() {
        delete[] data;
    }

    void enqueue(T value) {
        assert(count < capacity && "Queue is full!");
        data[(front + count) % capacity] = value;
        count++;
    }

    T dequeue() {
        assert(count > 0 && "Queue is empty!");
        T value = data[front];
        front = (front + 1) % capacity;
        count--;
        return value;
    }

    [[nodiscard]] bool isEmpty() const { return count == 0; }
    [[nodiscard]] int getSize() const { return count; }
};

#endif
