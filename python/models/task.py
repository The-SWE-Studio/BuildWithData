# models/task.py
"""
Data model for a Task.
"""

class Task:
    def __init__(self, title, description, priority=3, 
                 status='pending', task_id=None, assignee_id=None):
        self.task_id = task_id
        self.assignee_id = assignee_id
        self.title = title
        self.description = description
        self.status = status
        self.priority = priority # 1 = High, 5 = Low

    def __repr__(self):
        return (f"Task(id={self.task_id}, title='{self.title}', "
                f"priority={self.priority}, status='{self.status}')")