#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <netinet/in.h>

std::string get_env(const char* name, const char* default_value) {
    const char* value = std::getenv(name);
    return value ? value : default_value;
}

int main() {
    std::string port_str = get_env("SERVER_PORT", "9001");
    std::string redis_host = get_env("REDIS_HOST", "not-set");

    int port = std::stoi(port_str);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket failed\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "bind failed\n";
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "listen failed\n";
        return 1;
    }

    std::cout << "gateway running on port " << port << std::endl;
    std::cout << "redis host: " << redis_host << std::endl;

    while (true) {
        int client = accept(server_fd, nullptr, nullptr);
        if (client < 0) continue;

        std::string body = "gateway ok\nredis host: " + redis_host + "\n";

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" +
            body;

        send(client, response.c_str(), response.size(), 0);
        close(client);
    }

    close(server_fd);
    return 0;
}
