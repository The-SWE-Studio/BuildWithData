#include "Queue.h"
#include <iostream>

Queue::Queue() : head(nullptr), tail(nullptr), _size(0) {}

// The destructor is crucial to prevent memory leaks!
Queue::~Queue() {
    std::cout << "[Queue]: Cleaning up memory..." << std::endl;
    while (!isEmpty()) {
        Task* task = dequeue();
        delete task; // Delete the Task object
    }
}

void Queue::enqueue(Task* data) {
    Node* newNode = new Node(data); // Allocate memory
    if (isEmpty()) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    _size++;
    std::cout << "[Queue]: Enqueued " << data->title << std::endl;
}

Task* Queue::dequeue() {
    if (isEmpty()) {
        return nullptr;
    }

    Node* temp = head;
    Task* taskData = head->data;
    head = head->next;

    if (head == nullptr) {
        tail = nullptr;
    }

    delete temp; // Delete the Node wrapper
    // We DO NOT delete taskData, as the caller now "owns" it
    _size--;
    
    std::cout << "[Queue]: Dequeued " << taskData->title << std::endl;
    return taskData;
}

bool Queue::isEmpty() {
    return head == nullptr;
}

int Queue::size() {
    return _size;
}