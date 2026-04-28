#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

int main() {
    const char* redis_host = std::getenv("REDIS_HOST");
    const char* redis_port = std::getenv("REDIS_PORT");

    std::cout << "gateway started" << std::endl;
    std::cout << "redis host: " << (redis_host ? redis_host : "not set") << std::endl;
    std::cout << "redis port: " << (redis_port ? redis_port : "not set") << std::endl;
    std::cout << "listening on port 9001" << std::endl;

    while (true) {
        std::cout << "gateway running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
