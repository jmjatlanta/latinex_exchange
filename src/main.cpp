#include "exchange_server.h"
#include <thread>
#include <chrono>
#include <atomic>

std::atomic<bool> shutting_down = false;

void signal_thrown(int signal)
{
    shutting_down = true;
}

int main(int argc, char** argv)
{
    signal(SIGINT, signal_thrown);

    ExchangeServer server;

    while (!shutting_down)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cerr << "Shutdown normally" << std::endl;
    return EXIT_SUCCESS;
}
