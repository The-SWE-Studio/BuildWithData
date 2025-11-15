#pragma once
#include "Node.h"
#include <iostream>
#include <stdexcept> // For std::runtime_error

/*
 * Templated Queue (FIFO) implementation.
 * Header-only.
 * Analogy: A line at a checkout counter.
 *
 * NOTE: This REPLACES the C++ example from the previous
 * response, as this templated version is far more useful.
 */
template <typename T>
class Queue {
private:
    Node<T>* head; // Front of the line
    Node<T>* tail; // Back of the line
    int _size;

public:
    Queue() : head(nullptr), tail(nullptr), _size(0) {}

    // Destructor: Cleans up all nodes
    ~Queue() {
        while (!isEmpty()) {
            dequeue(); // Dequeue will delete the node
        }
    }

    // Add an item to the back (tail) of the queue
    // Complexity: O(1)
    void enqueue(T data) {
        Node<T>* newNode = new Node<T>(data);
        if (isEmpty()) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        _size++;
    }

    // Remove and return the item from the front (head)
    // Complexity: O(1)
    T dequeue() {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }

        Node<T>* temp = head;
        T data = head->data;
        head = head->next;

        if (head == nullptr) {
            tail = nullptr; // Queue is now empty
        }

        delete temp; // Free the memory for the node
        _size--;

        return data;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    int size() const {
        return _size;
    }
};