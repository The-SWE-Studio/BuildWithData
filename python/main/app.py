# main/app.py
"""
Main Application Orchestrator for "BuildWithData".

This script demonstrates the complete flow:
1.  **In-Memory (Input):** New tasks are created and added to a
    custom `Queue`. This simulates an input buffer.
2.  **Persistence:** A "processor" dequeues tasks one-by-one
    and saves them to the MySQL `Database`.
3.  **In-Memory (Processing):** We fetch pending tasks from the
    `Database` and load them into a custom `PriorityQueue`.
4.  **In-Memory (Logic):** We process tasks from the `PriorityQueue`,
    executing the highest-priority tasks first.
5.  **In-Memory (State):** We use a `Stack` to manage an "Undo"
    operation, showing how in-memory structures manage application state.
"""

import time
from db.database import DatabaseConnector
from models.task import Task
from data_structures.queue import Queue
from data_structures.priority_queue import PriorityQueue
from data_structures.stack import Stack
from config import DB_CONFIG

def separator(title):
    print("\n" + "="*25 + f" {title} " + "="*25)


class TaskManager:
    def __init__(self, db_connector):
        self.db = db_connector
        # 1. In-memory buffer for new tasks
        self.new_task_queue = Queue()
        # 2. In-memory scheduler for pending tasks
        self.task_scheduler = PriorityQueue()
        # 3. In-memory state manager for operations
        self.undo_stack = Stack()
        
        # This (action, data) tuple format is a simple "Command" pattern
        # e.g., ('update_status', {'task_id': 1, 'old_status': 'pending'})
        print("TaskManager initialized with Queue, PriorityQueue, and Stack.")

    def submit_new_task(self, title, desc, priority, user_id=1):
        """
        Step 1: A user submits a new task.
        It goes into the IN-MEMORY QUEUE first (fast operation).
        """
        print(f"\nUser submitted new task: '{title}'")
        task = Task(title=title, description=desc, priority=priority, assignee_id=user_id)
        self.new_task_queue.enqueue(task)

    def process_new_task_queue(self):
        """
        Step 2: A "background processor" saves tasks from the
        queue to the PERSISTENT DATABASE.
        """
        separator("Processing New Task Queue")
        while not self.new_task_queue.is_empty():
            task_to_save = self.new_task_queue.dequeue()
            print(f"Processor: Saving '{task_to_save.title}' to database...")
            # This is the moment the data moves from memory to disk
            self.db.create_task(task_to_save)
            time.sleep(0.5) # Simulate processing time
        print("Task queue empty. All new tasks persisted.")

    def load_tasks_into_scheduler(self):
        """
        Step 3: Load pending tasks from the DATABASE into the
        IN-MEMORY PRIORITY QUEUE to prepare for processing.
        """
        separator("Loading Pending Tasks into Scheduler")
        print("Fetching 'pending' tasks from database...")
        pending_tasks = self.db.get_pending_tasks_by_priority()
        if not pending_tasks:
            print("No pending tasks found.")
            return
            
        print(f"Found {len(pending_tasks)} pending tasks. Loading into PriorityQueue...")
        for task in pending_tasks:
            # Our Priority Queue stores (priority, task) tuples
            self.task_scheduler.insert(task, task.priority)
            time.sleep(0.2)
        print("Task Scheduler is loaded.")

    def run_task_scheduler(self):
        """
        Step 4: Process tasks from the IN-MEMORY PRIORITY QUEUE.
        This simulates a "worker" or "scheduler" executing tasks
        in order of importance.
        """
        separator("Running Task Scheduler")
        while not self.task_scheduler.is_empty():
            priority, task = self.task_scheduler.extract_min()
            
            print(f"\nExecuting Task (Priority {priority}): '{task.title}'")
            print(f"  -> Changing status from '{task.status}' to 'in_progress'")
            
            # Update the task in the database
            success, old_status = self.db.update_task_status(task.task_id, 'in_progress')
            
            if success:
                # We PUSH the "undo" operation onto the IN-MEMORY STACK
                undo_action = ('update_status', {'task_id': task.task_id, 'old_status': old_status})
                self.undo_stack.push(undo_action)
                
            time.sleep(0.5) # Simulate work
            
            print(f"  -> Task '{task.title}' complete.")
            self.db.update_task_status(task.task_id, 'completed')
            
        print("Task Scheduler is empty. All high-priority work is done.")

    def undo_last_action(self):
        """
        Step 5: Demonstrate the IN-MEMORY STACK for state management.
        We pop the last action and reverse it.
        """
        separator("Undo Last Action")
        if self.undo_stack.is_empty():
            print("Nothing to undo.")
            return

        # Pop the last action from the Stack
        action, data = self.undo_stack.pop()
        
        if action == 'update_status':
            print(f"Undoing status update for Task ID {data['task_id']}...")
            print(f"  -> Reverting to status: '{data['old_status']}'")
            self.db.update_task_status(data['task_id'], data['old_status'])
        else:
            print(f"Unknown undo action: {action}")


# --- Main Execution ---
def main():
    """Main function to run the simulation."""
    
    # Use the context manager for safe connection
    with DatabaseConnector(DB_CONFIG) as db:
        
        manager = TaskManager(db)
        
        # 1. Simulate user input -> In-Memory Queue
        manager.submit_new_task("Fix login bug", "Login page crashes", 1)
        manager.submit_new_task("Deploy to prod", "Push v2.0", 2)
        manager.submit_new_task("Update docs", "Add new API endpoints", 4)
        manager.submit_new_task("Refactor legacy code", "Clean up utils.py", 5)
        manager.submit_new_task("Email team about meeting", "10am Friday", 1)
        
        # 2. Simulate processor -> Queue to DB
        manager.process_new_task_queue()
        
        # 3. Load from DB -> In-Memory Priority Queue
        manager.load_tasks_into_scheduler()
        
        # 4. Process tasks from Priority Queue -> DB
        manager.run_task_scheduler()
        
        # 5. Demonstrate Stack -> Undo last action
        # This will find the last task set to 'in_progress' and revert it.
        manager.undo_last_action()


if __name__ == "__main__":
    main()