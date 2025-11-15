#pragma once
#include <string>
#include <map>

/*
 * This struct represents the "command" we push onto the undo stack.
 * It's a C++ equivalent of the Python tuple:
 * ('update_status', {'task_id': 1, 'old_status': 'pending'})
 */
struct UndoAction {
    std::string action_name;
    // We use a map to store the context data
    std::map<std::string, std::string> data;

    UndoAction(std::string name, std::map<std::string, std::string> d)
        : action_name(name), data(d) {}
};