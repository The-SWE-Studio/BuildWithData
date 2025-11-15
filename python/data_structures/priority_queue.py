# data_structures/priority_queue.py
"""
Custom implementation of a Priority Queue using a Min-Heap.

A Min-Heap is a complete binary tree where every parent node's
value is less than or equal to its children's values. This
makes it very efficient (O(log n)) to find and extract the
minimum element.

We store items as tuples: (priority, data).

Real-world analogy: An emergency room. Patients are not treated
FIFO, but based on the severity of their condition (priority).
"""

class PriorityQueue:
    """
    A Priority Queue implemented as a Min-Heap.
    The heap is stored in a standard Python list.
    """
    
    def __init__(self):
        # The list (acting as an array) to store the heap.
        # We add a 0 at the start to make parent/child index
        # calculations simpler (1-based indexing).
        self.heap = [0]
        self._size = 0

    def _perc_up(self, i):
        """
        Helper method to "percolate" a new item up the heap
        to maintain the heap property.
        Complexity: O(log n)
        """
        # While the node at i is smaller than its parent
        while i // 2 > 0:
            if self.heap[i][0] < self.heap[i // 2][0]:
                # Swap the node with its parent
                self.heap[i], self.heap[i // 2] = self.heap[i // 2], self.heap[i]
            else:
                break
            i = i // 2 # Move up to the parent's index

    def insert(self, item, priority):
        """
        Add a new item to the heap.
        `item` is the data, `priority` is its priority number (lower is higher).
        Complexity: O(log n)
        """
        # We store (priority, item) tuples to compare priorities
        self.heap.append((priority, item))
        self._size += 1
        self._perc_up(self._size)
        print(f"[P-Queue]: Inserted '{item.title}' with priority {priority}")

    def _perc_down(self, i):
        """
        Helper method to "percolate" an item down the heap
        after the root is removed.
        Complexity: O(log n)
        """
        while (i * 2) <= self._size:
            mc = self._min_child(i)
            if self.heap[i][0] > self.heap[mc][0]:
                # Swap the node with its smallest child
                self.heap[i], self.heap[mc] = self.heap[mc], self.heap[i]
            else:
                break
            i = mc # Move down to the child's index

    def _min_child(self, i):
        """
        Helper method to find the index of the smallest child of node i.
        Complexity: O(1)
        """
        # If there is no right child, return the left child
        if (i * 2 + 1) > self._size:
            return i * 2
        else:
            # Return the index of the smaller of the two children
            if self.heap[i * 2][0] < self.heap[i * 2 + 1][0]:
                return i * 2
            else:
                return i * 2 + 1

    def extract_min(self):
        """
        Remove and return the item with the highest priority (lowest number).
        Returns (priority, item) tuple.
        Complexity: O(log n)
        """
        if self.is_empty():
            return None
            
        # The min item is always at the root (index 1)
        min_val = self.heap[1]
        
        # Move the last item in the heap to the root
        self.heap[1] = self.heap[self._size]
        self.heap.pop() # Remove the last item
        self._size -= 1
        
        # Percolate the new root down to its correct position
        if not self.is_empty():
            self._perc_down(1)
            
        print(f"[P-Queue]: Extracted '{min_val[1].title}' (Priority {min_val[0]})")
        return min_val

    def peek_min(self):
        """Return the highest priority item without removing it."""
        return self.heap[1] if not self.is_empty() else None

    def is_empty(self):
        """Check if the priority queue is empty. Complexity: O(1)"""
        return self._size == 0

    def __len__(self):
        return self._size