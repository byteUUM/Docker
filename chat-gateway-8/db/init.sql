CREATE DATABASE IF NOT EXISTS chat_gateway;

USE chat_gateway;

CREATE TABLE IF NOT EXISTS user_status (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(64) NOT NULL,
    status VARCHAR(32) NOT NULL
);

INSERT INTO user_status(username, status) VALUES
('conwy', 'online'),
('docker', 'learning');
