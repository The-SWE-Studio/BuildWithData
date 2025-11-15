#pragma once

/*
 * This is a generic, templated Node class.
 * It can hold data of *any* type (T).
 * This allows us to use it for our Stack and Queue.
 */
template <typename T>
struct Node {
    T data;
    Node<T>* next;

    // Constructor to initialize the node
    Node(T val) : data(val), next(nullptr) {}
};