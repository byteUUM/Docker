#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

std::string getenv_or_default(const char* key, const std::string& def) {
    const char* val = std::getenv(key);
    return val ? std::string(val) : def;
}

bool tcp_connect(const std::string& host, int port) {
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    std::string port_str = std::to_string(port);

    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result) != 0) {
        return false;
    }

    bool ok = false;

    for (addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
        int fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) continue;

        timeval timeout{};
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            ok = true;
            close(fd);
            break;
        }

        close(fd);
    }

    freeaddrinfo(result);
    return ok;
}

std::string http_response(const std::string& body, int status = 200) {
    std::string status_text = status == 200 ? "OK" : "SERVICE UNAVAILABLE";

    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << status_text << "\r\n";
    oss << "Content-Type: text/plain\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << body;

    return oss.str();
}

int main() {
    std::string service_name = getenv_or_default("SERVICE_NAME", "gateway-default");
    int server_port = std::stoi(getenv_or_default("SERVER_PORT", "9001"));

    std::string mysql_host = getenv_or_default("MYSQL_HOST", "127.0.0.1");
    int mysql_port = std::stoi(getenv_or_default("MYSQL_PORT", "3306"));

    std::string redis_host = getenv_or_default("REDIS_HOST", "127.0.0.1");
    int redis_port = std::stoi(getenv_or_default("REDIS_PORT", "6379"));

    std::cout << "[gateway] service_name=" << service_name << std::endl;
    std::cout << "[gateway] server_port=" << server_port << std::endl;
    std::cout << "[gateway] mysql=" << mysql_host << ":" << mysql_port << std::endl;
    std::cout << "[gateway] redis=" << redis_host << ":" << redis_port << std::endl;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "failed to create socket" << std::endl;
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(server_port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "failed to bind port " << server_port << std::endl;
        return 1;
    }

    if (listen(server_fd, 16) < 0) {
        std::cerr << "failed to listen" << std::endl;
        return 1;
    }

    std::cout << "[gateway] listening on " << server_port << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            continue;
        }

        char buffer[4096]{};
        read(client_fd, buffer, sizeof(buffer) - 1);

        std::string request(buffer);
        std::string response;

        if (request.find("GET /health") != std::string::npos) {
            response = http_response("gateway alive\n");
        } else if (request.find("GET /info") != std::string::npos) {
            std::ostringstream body;
            body << "service_name=" << service_name << "\n";
            body << "server_port=" << server_port << "\n";
            body << "mysql=" << mysql_host << ":" << mysql_port << "\n";
            body << "redis=" << redis_host << ":" << redis_port << "\n";
            response = http_response(body.str());
        } else if (request.find("GET /ready") != std::string::npos) {
            bool mysql_ok = tcp_connect(mysql_host, mysql_port);
            bool redis_ok = tcp_connect(redis_host, redis_port);

            std::ostringstream body;
            body << "mysql=" << (mysql_ok ? "ok" : "failed") << "\n";
            body << "redis=" << (redis_ok ? "ok" : "failed") << "\n";

            response = http_response(body.str(), mysql_ok && redis_ok ? 200 : 503);
        } else {
            response = http_response("not found\n", 503);
        }

        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
    }

    return 0;
}