CREATE DATABASE IF NOT EXISTS chat_gateway;

USE chat_gateway;
CREATE TABLE IF NOT EXISTS messages (
    id INT AUTO_INCREMENT PRIMARY KEY,
    content VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO messages(content) VALUES ('hello from mysql init sql');