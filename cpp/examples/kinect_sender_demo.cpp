#include <iostream>
#include <asio.hpp>

#include "sender.h"

namespace rgbd_streamer
{
void send_frames()
{
    int port = 7777;
    asio::io_context io_context;
    Sender sender(io_context, port);
    sender.accept();

    bool stopped = false;
    std::thread thread([&io_context, &stopped]() {
        while (!stopped)
            io_context.run();
    });

    int n = 0;
    while (true) {
        if (sender.accepted()) {
            std::cout << "accepted" << std::endl;
            try {
                sender.send(n++);
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                break;
            }
        }
    }

    stopped = true;
    io_context.stop();
    thread.join();
}
}

int main()
{
    rgbd_streamer::send_frames();

    return 0;
}