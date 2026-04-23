#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "chat-gateway started on port 9001" << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
    return 0;
}
