# db/database.py
"""
The Database Layer.

This module is the *only* place in the application that
should interact directly with the MySQL database. It
abstracts all the SQL complexity away from the main logic.

It uses NO ORM, only the raw `mysql.connector` library.
"""

import mysql.connector
from mysql.connector import errorcode
from config import DB_CONFIG
from models.task import Task
from models.user import User

class DatabaseConnector:
    def __init__(self, config):
        self.config = config
        self.connection = None
        self.cursor = None

    def connect(self):
        """Establish a connection to the MySQL database."""
        try:
            self.connection = mysql.connector.connect(**self.config)
            self.cursor = self.connection.cursor(dictionary=True) # Returns rows as dicts
            print("Database connection successful.")
        except mysql.connector.Error as err:
            if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
                print("Error: Access denied. Check username/password in config.py")
            elif err.errno == errorcode.ER_BAD_DB_ERROR:
                print(f"Error: Database '{self.config['database']}' does not exist.")
                print("Please run `init_db.sql` first.")
            else:
                print(f"Database connection failed: {err}")
            exit(1) # Exit the program if DB connection fails

    def disconnect(self):
        """Close the database connection."""
        if self.cursor:
            self.cursor.close()
        if self.connection:
            self.connection.close()
            print("Database connection closed.")

    def __enter__(self):
        """Context manager: connect on entry."""
        self.connect()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager: disconnect on exit."""
        self.disconnect()

    # --- Task CRUD Operations ---

    def create_task(self, task):
        """
        Creates a new task in the database.
        Demonstrates: CREATE
        """
        sql = """
        INSERT INTO Tasks (title, description, priority, status, assignee_id)
        VALUES (%s, %s, %s, %s, %s)
        """
        val = (task.title, task.description, task.priority, task.status, task.assignee_id)
        
        try:
            self.cursor.execute(sql, val)
            self.connection.commit() # Commit the transaction
            task.task_id = self.cursor.lastrowid
            print(f"DB: Created Task ID {task.task_id}")
            return task
        except mysql.connector.Error as err:
            print(f"Failed to create task: {err}")
            self.connection.rollback() # Rollback on error
            return None

    def get_task_by_id(self, task_id):
        """
        Fetches a single task by its ID.
        Demonstrates: READ
        """
        sql = "SELECT * FROM Tasks WHERE task_id = %s"
        try:
            self.cursor.execute(sql, (task_id,))
            result = self.cursor.fetchone()
            if result:
                return Task(**result) # Unpack dict into Task object
            return None
        except mysql.connector.Error as err:
            print(f"Failed to get task: {err}")
            return None
            
    def get_pending_tasks_by_priority(self):
        """
        Fetches all 'pending' tasks, ordered by priority (highest first).
        Demonstrates: READ (with ordering)
        """
        sql = """
        SELECT * FROM Tasks 
        WHERE status = 'pending' 
        ORDER BY priority ASC, created_at ASC
        """
        try:
            self.cursor.execute(sql)
            results = self.cursor.fetchall()
            # Map the list of dicts to a list of Task objects
            return [Task(**row) for row in results]
        except mysql.connector.Error as err:
            print(f"Failed to get tasks: {err}")
            return []

    def update_task_status(self, task_id, new_status, old_status=None):
        """
        Updates a task's status, demonstrating a simple transaction.
        Demonstrates: UPDATE, Transaction
        """
        # Validate status
        if new_status not in ['pending', 'in_progress', 'completed']:
            print(f"Invalid status: {new_status}")
            return False, old_status

        try:
            print(f"\nDB: Attempting to update Task {task_id} to '{new_status}'...")
            # Start a transaction (optional, but good practice)
            self.connection.start_transaction()

            # 1. Get the current status for the "undo" operation
            if old_status is None:
                query_old = "SELECT status FROM Tasks WHERE task_id = %s FOR UPDATE"
                self.cursor.execute(query_old, (task_id,))
                result = self.cursor.fetchone()
                if not result:
                    print(f"DB: No task found with id {task_id}")
                    self.connection.rollback()
                    return False, None
                old_status = result['status']

            # 2. Perform the update
            sql = "UPDATE Tasks SET status = %s WHERE task_id = %s"
            self.cursor.execute(sql, (new_status, task_id))
            
            # 3. Commit the transaction
            self.connection.commit()
            print(f"DB: Successfully updated Task {task_id} from '{old_status}' to '{new_status}'")
            return True, old_status # Return success and the *previous* status
            
        except mysql.connector.Error as err:
            print(f"DB: Error updating task. Rolling back. {err}")
            self.connection.rollback()
            return False, old_status

    def delete_task(self, task_id):
        """
        Deletes a task by its ID.
        Demonstrates: DELETE
        """
        sql = "DELETE FROM Tasks WHERE task_id = %s"
        try:
            self.cursor.execute(sql, (task_id,))
            self.connection.commit()
            print(f"DB: Deleted Task ID {task_id}")
            return self.cursor.rowcount > 0 # Returns True if a row was deleted
        except mysql.connector.Error as err:
            print(f"Failed to delete task: {err}")
            self.connection.rollback()
            return False