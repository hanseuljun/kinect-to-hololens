#include <iostream>
#include <asio.hpp>
#include "receiver.h"

namespace rgbd_streamer
{
void receive_frames()
{
    std::string ip_address = "127.0.0.1";
    int port = 7777;
    asio::io_context io_context;
    Receiver receiver(io_context);
    receiver.connect(ip_address, port);

    while (true) {
        receiver.receive();
    }
}
}

int main()
{
    rgbd_streamer::receive_frames();

    return 0;
}