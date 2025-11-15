-- Create the database if it doesn't exist
CREATE DATABASE IF NOT EXISTS buildwithdata_db;

-- Use the newly created database
USE buildwithdata_db;

-- -----------------------------------------------------
-- Table `Users`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS Users (
  user_id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(100) NOT NULL UNIQUE,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- -----------------------------------------------------
-- Table `Tasks`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS Tasks (
  task_id INT AUTO_INCREMENT PRIMARY KEY,
  assignee_id INT NULL,
  title VARCHAR(255) NOT NULL,
  description TEXT,
  -- 'pending', 'in_progress', 'completed'
  status ENUM('pending', 'in_progress', 'completed') NOT NULL DEFAULT 'pending',
  -- 1 = Highest, 5 = Lowest
  priority INT NOT NULL DEFAULT 3,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  
  FOREIGN KEY (assignee_id) REFERENCES Users(user_id)
    ON DELETE SET NULL
    ON UPDATE CASCADE
) ENGINE=InnoDB;

-- Insert some dummy users for testing
INSERT INTO Users (username) VALUES ('alice'), ('bob') ON DUPLICATE KEY UPDATE username=username;