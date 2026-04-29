#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

bool check_tcp_connection(const std::string& host, int port) {
    addrinfo hints{}, *res;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "DNS resolve failed: " << host << std::endl;
        return false;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        freeaddrinfo(res);
        return false;
    }

    bool ok = connect(sockfd, res->ai_addr, res->ai_addrlen) == 0;

    close(sockfd);
    freeaddrinfo(res);

    return ok;
}

int main() {
    const char* redis_host_env = std::getenv("REDIS_HOST");
    const char* redis_port_env = std::getenv("REDIS_PORT");

    std::string redis_host = redis_host_env ? redis_host_env : "redis";
    int redis_port = redis_port_env ? std::atoi(redis_port_env) : 6379;

    std::cout << "gateway starting..." << std::endl;
    std::cout << "REDIS_HOST=" << redis_host << std::endl;
    std::cout << "REDIS_PORT=" << redis_port << std::endl;

    if (!check_tcp_connection(redis_host, redis_port)) {
        std::cerr << "failed to connect redis" << std::endl;
        return 1;
    }

    std::cout << "redis connection ok" << std::endl;
    std::cout << "gateway is running" << std::endl;

    while (true) {
        std::cout << "gateway heartbeat..." << std::endl;
        sleep(5);
    }

    return 0;
}
