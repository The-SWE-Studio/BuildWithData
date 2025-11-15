#pragma once
#include <string>
#include <sstream> // For toString()

class User {
public:
    int user_id;
    std::string username;
    std::string created_at;

    // Default constructor
    User() : user_id(0), username("") {}

    // Parameterized constructor
    User(std::string name, int id = 0, std::string created = "")
        : user_id(id), username(name), created_at(created) {}

    std::string toString() const {
        std::stringstream ss;
        ss << "User(id=" << user_id << ", username='" << username << "')";
        return ss.str();
    }
};