# config.py
"""
Database configuration settings.
**IMPORTANT**: Students should change these values to match their
local MySQL server setup.
"""

DB_CONFIG = {
    'host': 'localhost',
    'user': 'root',  # Use a less-privileged user in production!
    'password': '', # <-- CHANGE THIS
    'database': 'buildwithdata_db',
    'ssl_disabled': True
}