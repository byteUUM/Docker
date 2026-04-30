#include <arpa/inet.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

std::map<std::string, std::string> load_config(const std::string& path) {
    std::map<std::string, std::string> config;
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "failed to open config file: " << path << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        config[key] = value;
    }

    return config;
}

std::string getenv_or_default(const char* key, const std::string& default_value) {
    const char* value = std::getenv(key);
    if (value == nullptr || std::string(value).empty()) {
        return default_value;
    }
    return value;
}

int main() {
    std::string port_str = getenv_or_default("SERVER_PORT", "9001");
    std::string config_path = getenv_or_default("CONFIG_PATH", "/app/config/gateway.conf");

    int port = std::stoi(port_str);

    auto config = load_config(config_path);

    std::string log_level = config.count("LOG_LEVEL") ? config["LOG_LEVEL"] : "INFO";
    std::string service_name = config.count("SERVICE_NAME") ? config["SERVICE_NAME"] : "gateway";

    std::cout << "service_name=" << service_name << std::endl;
    std::cout << "log_level=" << log_level << std::endl;
    std::cout << "server_port=" << port << std::endl;
    std::cout << "config_path=" << config_path << std::endl;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "failed to create socket" << std::endl;
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "failed to bind port: " << port << std::endl;
        return 1;
    }

    if (listen(server_fd, 8) < 0) {
        std::cerr << "failed to listen" << std::endl;
        return 1;
    }

    std::cout << "gateway listening on port " << port << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            continue;
        }

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "OK\n";

        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
