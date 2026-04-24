#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
    std::string config = "/app/config/gateway.conf";
    int port = 9001;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg.rfind("--config=", 0) == 0)
        {
            config = arg.substr(9);
        }
        else if (arg.rfind("--port=", 0) == 0)
        {
            port = std::stoi(arg.substr(7));
        }
    }

    std::ofstream log("/app/logs/gateway.log", std::ios::app);

    std::cout << "gateway start" << std::endl;
    std::cout << "config: " << config << std::endl;
    std::cout << "port: " << port << std::endl;

    log << "gateway start" << std::endl;
    log << "config: " << config << std::endl;
    log << "port: " << port << std::endl;

    while (true)
    {
        std::cout << "gateway running on port " << port << std::endl;
        log << "gateway running on port " << port << std::endl;
        log.flush();

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
