#pragma once
#include <string>
#include <sstream>

class Task {
public:
    int task_id;
    int assignee_id;
    std::string title;
    std::string description;
    std::string status;
    int priority; // 1 = High, 5 = Low

    // Default constructor
    Task() : task_id(0), assignee_id(0), priority(3), status("pending") {}

    // Parameterized constructor
    Task(std::string t, std::string d, int p = 3, std::string s = "pending", int id = 0, int assign_id = 0)
        : task_id(id),
          assignee_id(assign_id),
          title(t),
          description(d),
          status(s),
          priority(p) {}

    std::string toString() const {
        std::stringstream ss;
        ss << "Task(id=" << task_id << ", title='" << title 
           << "', priority=" << priority << ", status='" << status << "')";
        return ss.str();
    }
};