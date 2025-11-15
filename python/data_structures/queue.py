# data_structures/queue.py
"""
Custom implementation of a Queue (FIFO - First-In, First-Out).

This implementation uses a Linked List (built from Nodes) and keeps
track of both the `head` (front) and `tail` (back) for efficient
O(1) enqueue and dequeue operations.

Real-world analogy: A checkout line at a grocery store. The first
person in line (head) is served first, and new people (tail)
join at the end.
"""
from .node import Node

class Queue:
    """A FIFO Queue implementation using a linked list."""
    
    def __init__(self):
        self.head = None  # Front of the queue
        self.tail = None  # Back of the queue
        self._size = 0

    def enqueue(self, data):
        """
        Add an item to the back (tail) of the queue.
        Complexity: O(1)
        """
        new_node = Node(data)
        if self.is_empty():
            # If queue is empty, new node is both head and tail
            self.head = new_node
            self.tail = new_node
        else:
            # Link the current tail to the new node
            self.tail.next = new_node
            # Update the tail to be the new node
            self.tail = new_node
        self._size += 1
        print(f"[Queue]: Enqueued {data.title}")

    def dequeue(self):
        """
        Remove and return the item from the front (head) of the queue.
        Returns None if the queue is empty.
        Complexity: O(1)
        """
        if self.is_empty():
            return None
            
        # Get data from the head node
        data = self.head.data
        
        # Move the head pointer to the next node
        self.head = self.head.next
        
        # If the queue is now empty, reset the tail as well
        if self.head is None:
            self.tail = None
            
        self._size -= 1
        print(f"[Queue]: Dequeued {data.title}")
        return data

    def peek(self):
        """
        Return the item at the front (head) without removing it.
        Complexity: O(1)
        """
        return self.head.data if self.head else None

    def is_empty(self):
        """Check if the queue is empty. Complexity: O(1)"""
        return self.head is None

    def __len__(self):
        """Return the size of the queue."""
        return self._size

    def __repr__(self):
        items = []
        current = self.head
        while current:
            items.append(str(current.data.title))
            current = current.next
        return f"Queue(head -> {' -> '.join(items)} <- tail)"