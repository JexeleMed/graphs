#ifndef LIST_HPP
#define LIST_HPP

#include <iostream>
#include <cstdlib>

template <typename T>
class SinglyLinkedList {
private:
    struct Node {
        T data;
        Node* next;

        Node(T value) : data(value), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

public:
    class Iterator {
    private:
        Node* current;
    public:
        explicit Iterator(Node* node) : current(node) {}

        T& operator*() {
            return current->data;
        }

        Iterator& operator++() {
            if (current != nullptr) {
                current = current->next;
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    // Read-only iterator: lets range-for work on const lists
    // (without it, const methods like display() fail to compile when instantiated)
    class ConstIterator {
    private:
        const Node* current;
    public:
        explicit ConstIterator(const Node* node) : current(node) {}

        const T& operator*() const {
            return current->data;
        }

        ConstIterator& operator++() {
            if (current != nullptr) {
                current = current->next;
            }
            return *this;
        }

        bool operator!=(const ConstIterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() {
        return Iterator(head);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    // Const overloads — picked automatically when the list itself is const
    ConstIterator begin() const {
        return ConstIterator(head);
    }

    ConstIterator end() const {
        return ConstIterator(nullptr);
    }

    SinglyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    ~SinglyLinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    void append(T value) {
        Node* newNode = new Node(value);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    void removeLast() {
        if (head == nullptr) return;

        if (head == tail) {
            delete head;
            head = tail = nullptr;
        } else {
            Node* current = head;
            while (current->next != tail) {
                current = current->next;
            }
            delete tail;
            tail = current;
            tail->next = nullptr;
        }
        size--;
    }

    T& operator[](int index) {
        if (index < 0 || index >= size) {
            std::cerr << "Fatal Error: Index " << index << " out of bounds in SinglyLinkedList! Terminating program.\n";
            std::exit(EXIT_FAILURE);
        }

        Node* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }

    [[nodiscard]] int getSize() const {
        return size;
    }

    void display() const {
        for (auto& element : *this) {
            std::cout << element << " ";
        }
        std::cout << "\n";
    }
};

#endif