#pragma once
#include <string>
// MySQL Connector C++ headers
#include "mysql_driver.h"
#include "mysql_connection.h"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include "../models/Task.h"

class DatabaseConnector {
private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
    
    std::string host;
    std::string user;
    std::string pass;
    std::string db;

public:
    DatabaseConnector(std::string host, std::string user, std::string pass, std::string db);
    ~DatabaseConnector();

    void connect();
    void disconnect();

    // CRUD Operations
    Task* createTask(Task* task);
    Task* getTaskById(int taskId);
    bool updateTaskStatus(int taskId, std::string newStatus);
    std.vector<Task*> getPendingTasks(); // Uses std::vector (allowed)
};