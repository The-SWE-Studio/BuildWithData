# data_structures/stack.py
"""
Custom implementation of a Stack (LIFO - Last-In, First-Out).

This implementation uses a Linked List (built from Nodes) as its
underlying structure.

Real-world analogy: A stack of plates. You add (push) a new plate
to the top, and you remove (pop) a plate from the top.
"""
from .node import Node

class Stack:
    """A LIFO Stack implementation using a linked list."""
    
    def __init__(self):
        self.top = None  # Points to the top Node of the stack
        self._size = 0

    def push(self, data):
        """
        Add an item to the top of the stack.
        Complexity: O(1)
        """
        new_node = Node(data)
        new_node.next = self.top
        self.top = new_node
        self._size += 1
        print(f"[Stack]: Pushed {data}")

    def pop(self):
        """
        Remove and return the item from the top of the stack.
        Returns None if the stack is empty.
        Complexity: O(1)
        """
        if self.is_empty():
            return None
        
        data = self.top.data
        self.top = self.top.next  # Move top pointer down
        self._size -= 1
        print(f"[Stack]: Popped {data}")
        return data

    def peek(self):
        """
        Return the item at the top of the stack without removing it.
        Complexity: O(1)
        """
        return self.top.data if self.top else None

    def is_empty(self):
        """Check if the stack is empty. Complexity: O(1)"""
        return self.top is None

    def __len__(self):
        """Return the size of the stack."""
        return self._size

    def __repr__(self):
        items = []
        current = self.top
        while current:
            items.append(str(current.data))
            current = current.next
        return f"Stack(top -> {' -> '.join(items)})"