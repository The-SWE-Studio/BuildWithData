#pragma once
#include "Node.h"
#include <iostream>
#include <stdexcept> // For std::runtime_error

/*
 * Templated Stack (LIFO) implementation.
 * It is header-only because it's a template.
 * Analogy: A stack of plates.
 */
template <typename T>
class Stack {
private:
    Node<T>* top;
    int _size;

public:
    Stack() : top(nullptr), _size(0) {}

    // Destructor: Essential to prevent memory leaks!
    // It walks the list and deletes every node.
    ~Stack() {
        while (!isEmpty()) {
            pop(); // Pop will delete the node
        }
    }

    // Push an item onto the top of the stack
    // Complexity: O(1)
    void push(T data) {
        Node<T>* newNode = new Node<T>(data);
        newNode->next = top;
        top = newNode;
        _size++;
    }

    // Remove and return the top item
    // Complexity: O(1)
    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("Stack is empty");
        }
        
        Node<T>* temp = top;
        T data = top->data;
        top = top->next;
        
        delete temp; // Free the memory for the node
        _size--;
        
        return data;
    }

    // Return top item without removing
    // Complexity: O(1)
    T peek() const {
        if (isEmpty()) {
            throw std::runtime_error("Stack is empty");
        }
        return top->data;
    }

    bool isEmpty() const {
        return top == nullptr;
    }

    int size() const {
        return _size;
    }
};