# models/user.py
"""
Data model for a User.

This is NOT an ORM model. It's a plain Python class (a "POPO")
that acts as a simple data container. This separation of
concerns (data representation vs. database logic) is key.
"""

class User:
    def __init__(self, username, user_id=None, created_at=None):
        self.user_id = user_id
        self.username = username
        self.created_at = created_at

    def __repr__(self):
        return f"User(id={self.user_id}, username='{self.username}')"