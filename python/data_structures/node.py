# data_structures/node.py
"""
Defines the basic Node class.

This is the fundamental building block for many linked data structures
like Linked Lists, Queues, Stacks, and Trees. It holds two pieces
of information:
1. `data`: The value it stores.
2. `next`: A "pointer" (reference) to the next Node in the sequence.
"""

class Node:
    """A single node for use in linked data structures."""
    def __init__(self, data=None):
        self.data = data
        self.next = None  # Reference to the next node

    def __repr__(self):
        return f"Node({self.data})"