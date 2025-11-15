#include <iostream>
#include <string>
#include <vector>
#include <memory> // For smart pointers
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::milliseconds

// Project includes
#include "../db/DatabaseConnector.h"
#include "../models/Task.h"
#include "../models/UndoAction.h"
#include "../data_structures/Queue.h"
#include "../data_structures/Stack.h"
#include "../data_structures/PriorityQueue.h"

// --- Configuration ---
const std::string DB_HOST = "localhost";
const std::string DB_USER = "root";
const std::string DB_PASS = "YOUR_MYSQL_PASSWORD"; // <-- CHANGE THIS
const std::string DB_NAME = "buildwithdata_db";


void separator(std::string title) {
    std::cout << "\n" << std::string(25, '=') << " " << title << " " << std::string(25, '=') << std::endl;
}

/*
 * TaskManager class orchestrates the data flow.
 *
 * It uses C++ smart pointers for memory safety:
 * - `std::unique_ptr<Task>`: For the new task queue. The queue
 * has *unique ownership* of the new task data.
 * - `std::shared_ptr<Task>`: For the priority queue. Multiple
 * parts of the system might (in theory) refer to a task
 * that is actively being processed.
 * - `UndoAction`: This is a simple struct, so we store it
 * by value in the stack.
 */
class TaskManager {
private:
    DatabaseConnector* db;
    Queue<std::unique_ptr<Task>> new_task_queue;
    PriorityQueue<std::shared_ptr<Task>, int> task_scheduler;
    Stack<UndoAction> undo_stack;

public:
    TaskManager(DatabaseConnector* db_conn) : db(db_conn) {
        std::cout << "TaskManager initialized with Queue, PriorityQueue, and Stack." << std::endl;
    }

    // Step 1: Submit new task to IN-MEMORY QUEUE
    void submit_new_task(std::string title, std::string desc, int priority, int user_id = 1) {
        std::cout << "\nUser submitted new task: '" << title << "'" << std::endl;
        
        // Use std::make_unique to create a smart pointer for the new task
        auto task_ptr = std::make_unique<Task>(title, desc, priority, "pending", 0, user_id);
        
        // Move ownership of the pointer into the queue
        new_task_queue.enqueue(std::move(task_ptr));
        std::cout << "[Queue]: Enqueued " << title << std::endl;
    }

    // Step 2: Process queue -> PERSISTENT DATABASE
    void process_new_task_queue() {
        separator("Processing New Task Queue");
        while (!new_task_queue.isEmpty()) {
            // Dequeue gives us ownership of the unique_ptr
            std::unique_ptr<Task> task_to_save = new_task_queue.dequeue();
            
            std::cout << "Processor: Saving '" << task_to_save->title << "' to database..." << std::endl;
            
            // Pass the raw pointer to the DB. The .get() method
            // does *not* release ownership.
            db->createTask(task_to_save.get());
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // When task_to_save goes out of scope here, the
            // unique_ptr is automatically destroyed, freeing the memory.
        }
        std::cout << "Task queue empty. All new tasks persisted." << std::endl;
    }

    // Step 3: Load from DB -> IN-MEMORY PRIORITY QUEUE
    void load_tasks_into_scheduler() {
        separator("Loading Pending Tasks into Scheduler");
        std::cout << "Fetching 'pending' tasks from database..." << std::endl;
        
        // DB returns a vector of raw pointers (it owns this memory)
        std::vector<Task*> pending_tasks = db->getPendingTasks();
        
        if (pending_tasks.empty()) {
            std::cout << "No pending tasks found." << std::endl;
            return;
        }

        std::cout << "Found " << pending_tasks.size() << " pending tasks. Loading into PriorityQueue..." << std::endl;
        
        for (Task* task_ptr : pending_tasks) {
            // Create a shared_ptr to manage this task's lifetime.
            // The Priority Queue will now "own" this task.
            std::shared_ptr<Task> task_sptr(task_ptr);
            task_scheduler.insert(task_sptr, task_sptr->priority);
            
            std::cout << "[P-Queue]: Inserted '" << task_sptr->title << "' with priority " << task_sptr->priority << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        std::cout << "Task Scheduler is loaded." << std::endl;
    }

    // Step 4: Process from PRIORITY QUEUE -> DB
    void run_task_scheduler() {
        separator("Running Task Scheduler");
        while (!task_scheduler.isEmpty()) {
            // Extract the (priority, data) pair
            auto item = task_scheduler.extract_min();
            int priority = item.first;
            std::shared_ptr<Task> task = item.second; // Get the shared_ptr to the task
            
            std::cout << "\nExecuting Task (Priority " << priority << "): '" << task->title << "'" << std::endl;
            std::cout << "  -> Changing status from '" << task->status << "' to 'in_progress'" << std::endl;

            // Update the task in the database
            auto result = db->updateTaskStatus(task->task_id, "in_progress");
            bool success = result.first;
            std::string old_status = result.second;
            
            if (success) {
                // We PUSH the "undo" operation onto the IN-MEMORY STACK
                std::map<std::string, std::string> data;
                data["task_id"] = std::to_string(task->task_id);
                data["old_status"] = old_status;
                
                undo_stack.push(UndoAction("update_status", data));
                std::cout << "[Stack]: Pushed undo action for task " << task->task_id << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            std::cout << "  -> Task '" << task->title << "' complete." << std::endl;
            db->updateTaskStatus(task->task_id, "completed");
        }
        // When task shared_ptrs go out of scope, the memory is freed.
        std::cout << "Task Scheduler is empty. All high-priority work is done." << std::endl;
    }

    // Step 5: Demonstrate IN-MEMORY STACK
    void undo_last_action() {
        separator("Undo Last Action");
        if (undo_stack.isEmpty()) {
            std::cout << "Nothing to undo." << std::endl;
            return;
        }

        UndoAction action = undo_stack.pop();
        
        if (action.action_name == "update_status") {
            int task_id = std::stoi(action.data["task_id"]);
            std::string status_to_revert = action.data["old_status"];
            
            std::cout << "Undoing status update for Task ID " << task_id << "..." << std::endl;
            std::cout << "  -> Reverting to status: '" << status_to_revert << "'" << std::endl;
            db->updateTaskStatus(task_id, status_to_revert);
        }
    }
};

// --- Main Execution ---
int main() {
    std::cout << "Starting BuildWithData C++ Project..." << std::endl;
    
    DatabaseConnector db(DB_HOST, DB_USER, DB_PASS, DB_NAME);
    db.connect();
    
    TaskManager manager(&db);
    
    // 1. Simulate user input -> In-Memory Queue
    manager.submit_new_task("Fix login bug (C++)", "Login page crashes", 1, 1);
    manager.submit_new_task("Deploy to prod (C++)", "Push v2.0", 2, 1);
    manager.submit_new_task("Update docs (C++)", "Add new API endpoints", 4, 2);
    manager.submit_new_task("Refactor legacy code (C++)", "Clean up utils.cpp", 5, 2);
    manager.submit_new_task("Email team about meeting (C++)", "10am Friday", 1, 1);
    
    // 2. Simulate processor -> Queue to DB
    manager.process_new_task_queue();
    
    // 3. Load from DB -> In-Memory Priority Queue
    manager.load_tasks_into_scheduler();
    
    // 4. Process tasks from Priority Queue -> DB
    manager.run_task_scheduler();
    
    // 5. Demonstrate Stack -> Undo last action
    manager.undo_last_action();
    
    db.disconnect();
    std::cout << "BuildWithData C++ Project finished." << std::endl;
    return 0;
}