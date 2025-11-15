# CMakeLists.txt (Simplified Example)
cmake_minimum_required(VERSION 3.10)
project(BuildWithData_CPP)

# Find the MySQL Connector/C++ library
# This path must be set by the student
set(MYSQL_CONNECTOR_PATH "/path/to/mysql-connector-c++-8.0")

# --- Find MySQL includes and libraries ---
include_directories(${MYSQL_CONNECTOR_PATH}/include)
link_directories(${MYSQL_CONNECTOR_PATH}/lib64) # or /lib

# Add our source directories
include_directories(./models ./db ./data_structures)

# --- Define Source Files ---
file(GLOB_RECURSE SOURCES 
    "main/*.cpp"
    "models/*.cpp"
    "db/*.cpp"
    "data_structures/*.cpp"
)

# --- Create the Executable ---
add_executable(task_manager ${SOURCES})

# --- Link Libraries ---
# Note: The C++ connector requires dynamic linking
target_link_libraries(task_manager mysqlcppconn)