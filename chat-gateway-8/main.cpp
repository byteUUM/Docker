#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

std::string getenv_or_default(const char* key, const char* default_value) {
    const char* value = std::getenv(key);
    return value ? value : default_value;
}

bool check_tcp_connect(const std::string& host, const std::string& port) {
    struct addrinfo hints{}, *res = nullptr;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (ret != 0) {
        std::cerr << "[gateway] getaddrinfo failed: " << gai_strerror(ret) << std::endl;
        return false;
    }

    int sockfd = -1;
    bool connected = false;

    for (auto p = res; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
            connected = true;
            close(sockfd);
            break;
        }

        close(sockfd);
    }

    freeaddrinfo(res);
    return connected;
}

int main() {
    std::string server_port = getenv_or_default("SERVER_PORT", "9001");
    std::string db_host = getenv_or_default("DB_HOST", "127.0.0.1");
    std::string db_port = getenv_or_default("DB_PORT", "3306");
    std::string service_name = getenv_or_default("SERVICE_NAME", "gateway-default");

    std::cout << "[gateway] service_name=" << service_name << std::endl;
    std::cout << "[gateway] server_port=" << server_port << std::endl;
    std::cout << "[gateway] db_host=" << db_host << std::endl;
    std::cout << "[gateway] db_port=" << db_port << std::endl;

    if (!check_tcp_connect(db_host, db_port)) {
        std::cerr << "[gateway] failed to connect mysql at " 
                  << db_host << ":" << db_port << std::endl;
        return 1;
    }

    std::cout << "[gateway] mysql tcp connection success" << std::endl;
    std::cout << "[gateway] gateway started successfully" << std::endl;

    while (true) {
        sleep(5);
        std::cout << "[gateway] running..." << std::endl;
    }

    return 0;
}
