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

    if (argc < 2)
    {
        std::cerr << "Syntax: " << argv[0] << " server.xml\n";
        return EXIT_FAILURE;
    }
    ExchangeServer server(argv[1]);

    while (!shutting_down)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cerr << "Shutdown normally" << std::endl;
    return EXIT_SUCCESS;
}
