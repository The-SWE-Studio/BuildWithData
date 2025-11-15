#include "DatabaseConnector.h"
#include <stdexcept>
#include <iostream>

// Include specific MySQL Connector headers
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>


DatabaseConnector::DatabaseConnector(std::string h, std::string u, std::string p, std::string d)
    : host(h), user(u), pass(p), db(d), driver(nullptr), con(nullptr) {
    
    try {
        // Get the MySQL driver instance
        driver = sql::mysql::get_mysql_driver_instance();
    } catch (sql::SQLException &e) {
        std::cerr << "Could not get MySQL driver instance: " << e.what() << std::endl;
        exit(1); // Fatal error
    }
}

DatabaseConnector::~DatabaseConnector() {
    disconnect();
}

void DatabaseConnector::connect() {
    try {
        con = driver->connect(host, user, pass);
        con->setSchema(db);
        std::cout << "Database connection successful." << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
        exit(1); // Fatal error
    }
}

void DatabaseConnector::disconnect() {
    if (con) {
        delete con;
        con = nullptr;
        std::cout << "Database connection closed." << std::endl;
    }
}

// --- CRUD Operations ---

Task* DatabaseConnector::createTask(Task* task) {
    sql::PreparedStatement* pstmt = nullptr;
    sql::Statement* stmt = nullptr;
    sql::ResultSet* res = nullptr;
    
    try {
        const char* sql = "INSERT INTO Tasks (title, description, priority, status, assignee_id) VALUES (?, ?, ?, ?, ?)";
        pstmt = con->prepareStatement(sql);
        
        pstmt->setString(1, task->title);
        pstmt->setString(2, task->description);
        pstmt->setInt(3, task->priority);
        pstmt->setString(4, task->status);
        if (task->assignee_id == 0) {
            pstmt->setNull(5, 0);
        } else {
            pstmt->setInt(5, task->assignee_id);
        }
        
        pstmt->execute();
        delete pstmt;

        // Get the last inserted ID to update the task object
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT LAST_INSERT_ID()");
        if (res->next()) {
            task->task_id = res->getInt(1);
        }
        
        std::cout << "DB: Created Task ID " << task->task_id << std::endl;
        
        delete res;
        delete stmt;
        return task;

    } catch (sql::SQLException &e) {
        std::cerr << "Failed to create task: " << e.what() << std::endl;
        if (pstmt) delete pstmt;
        if (res) delete res;
        if (stmt) delete stmt;
        return nullptr;
    }
}

std::vector<Task*> DatabaseConnector::getPendingTasks() {
    std::vector<Task*> tasks;
    sql::Statement* stmt = nullptr;
    sql::ResultSet* res = nullptr;
    
    try {
        const char* sql = "SELECT * FROM Tasks WHERE status = 'pending' ORDER BY priority ASC, created_at ASC";
        stmt = con->createStatement();
        res = stmt->executeQuery(sql);
        
        // Map rows to Task objects
        while (res->next()) {
            tasks.push_back(new Task(
                res->getString("title"),
                res->getString("description"),
                res->getInt("priority"),
                res->getString("status"),
                res->getInt("task_id"),
                res->getInt("assignee_id")
            ));
        }
        
        delete res;
        delete stmt;
        
    } catch (sql::SQLException &e) {
        std::cerr << "Failed to get tasks: " << e.what() << std::endl;
        if (res) delete res;
        if (stmt) delete stmt;
    }
    return tasks;
}

/*
 * Demonstrates an UPDATE query inside a Transaction.
 * Returns a pair: (success_bool, old_status_string)
 */
std::pair<bool, std::string> DatabaseConnector::updateTaskStatus(int task_id, std::string new_status) {
    sql::PreparedStatement* pstmt_select = nullptr;
    sql::PreparedStatement* pstmt_update = nullptr;
    sql::ResultSet* res = nullptr;
    std::string old_status = "";

    try {
        std::cout << "\nDB: Attempting to update Task " << task_id << " to '" << new_status << "'..." << std::endl;
        
        // Start transaction
        con->setAutoCommit(false); 

        // 1. Get the current status for "undo" and lock the row
        const char* sql_select = "SELECT status FROM Tasks WHERE task_id = ? FOR UPDATE";
        pstmt_select = con->prepareStatement(sql_select);
        pstmt_select->setInt(1, task_id);
        res = pstmt_select->executeQuery();

        if (!res->next()) {
            std::cerr << "DB: No task found with id " << task_id << std::endl;
            throw std::runtime_error("Task not found");
        }
        old_status = res->getString("status");
        
        // 2. Perform the update
        const char* sql_update = "UPDATE Tasks SET status = ? WHERE task_id = ?";
        pstmt_update = con->prepareStatement(sql_update);
        pstmt_update->setString(1, new_status);
        pstmt_update->setInt(2, task_id);
        pstmt_update->executeUpdate();

        // 3. Commit the transaction
        con->commit();
        con->setAutoCommit(true); // Reset autocommit
        
        std::cout << "DB: Successfully updated Task " << task_id << " from '" 
                  << old_status << "' to '" << new_status << "'" << std::endl;

        delete res;
        delete pstmt_select;
        delete pstmt_update;
        
        return std::make_pair(true, old_status);

    } catch (sql::SQLException &e) {
        std::cerr << "DB: Error updating task. Rolling back. " << e.what() << std::endl;
        try {
            con->rollback(); // Rollback on error
            con->setAutoCommit(true);
        } catch (sql::SQLException &rb_e) {
            std::cerr << "Rollback failed: " << rb_e.what() << std::endl;
        }
        
        if (res) delete res;
        if (pstmt_select) delete pstmt_select;
        if (pstmt_update) delete pstmt_update;
        
        return std::make_pair(false, old_status);
    }
}