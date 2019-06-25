#include "kh_core.h"
#include "kh_sender.h"
#include "kh_vp8.h"
#include "kinect/kinect.h"

namespace kh
{
// This method returns a KinectIntrinsics of a Kinect v2 device from our lab (VHIL).
// Used when kh::kinect::obtainKinectIntrinsics() does not work.
// kh::kinect::obtainKinectIntrinsics() heavily relies on Freenect2 which is quite old
// and I chose this detour as the solution.
kinect::KinectIntrinsics get_substitute_kinect_intrinsics()
{
    libfreenect2::Freenect2Device::ColorCameraParams color_params;
    color_params.fx = 1081.37f;
    color_params.fy = 1081.37f;
    color_params.cx = 959.5f;
    color_params.cy = 539.5f;
    color_params.shift_d = 863.0f;
    color_params.shift_m = 52.0f;
    color_params.mx_x3y0 = 0.000433573f;
    color_params.mx_x0y3 = 3.11985e-06f;
    color_params.mx_x2y1 = 4.89289e-05f;
    color_params.mx_x1y2 = 0.000329721f;
    color_params.mx_x2y0 = 0.000753273f;
    color_params.mx_x0y2 = 3.57279e-05f;
    color_params.mx_x1y1 = -0.000761282f;
    color_params.mx_x1y0 = 0.633183f;
    color_params.mx_x0y1 = 0.00562461f;
    color_params.mx_x0y0 = 0.17028f;
    color_params.my_x3y0 = 3.31803e-06f;
    color_params.my_x0y3 = 0.000587018f;
    color_params.my_x2y1 = 0.000425902f;
    color_params.my_x1y2 = 1.76095e-05f;
    color_params.my_x2y0 = -0.0002469f;
    color_params.my_x0y2 = -0.000945311f;
    color_params.my_x1y1 = 0.000648708f;
    color_params.my_x1y0 = -0.00578545f;
    color_params.my_x0y1 = 0.632964f;
    color_params.my_x0y0 = -0.000370404f;

    libfreenect2::Freenect2Device::IrCameraParams ir_params;
    ir_params.fx = 368.147f;
    ir_params.fy = 368.147f;
    ir_params.cx = 264.317f;
    ir_params.cy = 208.964f;
    ir_params.k1 = 0.0807732f;
    ir_params.k2 = -0.27181f;
    ir_params.k3 = 0.103199f;
    ir_params.p1 = 0.0f;
    ir_params.p2 = 0.0f;

    kinect::KinectIntrinsics kinect_intrinsics;
    kinect_intrinsics.color_params = color_params;
    kinect_intrinsics.ir_params = ir_params;
    return kinect_intrinsics;
}

// Sends Kinect frames through a TCP port.
void _send_frames(int port)
{
    std::cout << "Start sending Kinect frames (port: " << port << ")." << std::endl;

    // Collecting KinectIntrinsics.
    // If kh::kinect::obtainKinectIntrinsics() fails, uses get_substitute_kinect_intrinsics().
    auto intrinsics = kinect::obtainKinectIntrinsics();
    if (!intrinsics) {
        std::cout << "Could not find intrinsics of the connected Kinect." << std::endl;
        std::cout << "Using a substitute KinectIntrinsics." << std::endl;
        intrinsics = get_substitute_kinect_intrinsics();
    }

    // Obtaining kh::kinect::KinectDevice to access Kinect v2 APIs.
    // This is neccessary for the sender to function.
    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "Could not find a Kinect." << std::endl;
        return;
    }

    // The width and height of the Vp8Encoder is the half of the Kinect since
    // the frames will be halved before getting encoded.
    // The bitrate is chosen arbitrarily.
    std::cout << "Waiting for a client..." << std::endl;
    Vp8Encoder encoder(960, 540, 2000);

    // Creating a tcp socket with the port and waiting for a connection.
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    auto socket = acceptor.accept();
    
    std::cout << "Accepted a client!" << std::endl;

    // Sender is a class that will use the socket to send frames to the receiver that has the socket connected to this socket.
    Sender sender(std::move(socket));
    // The sender sends athe KinectIntrinsics, so the renderer from the receiver side can prepare rendering Kinect frames.
    sender.send(*intrinsics);

    // The amount of frames this sender will send before receiveing a feedback from a receiver.
    const int MAXIMUM_FRAME_ID_DIFF = 2;
    // frame_id is the ID of the frame the sender sends.
    int frame_id = 0;
    // receiver_frame_id is the ID that the receiver sent back saying it received the frame of that ID.
    int receiver_frame_id = 0;

    // Variables for profiling the sender.
    auto start = std::chrono::system_clock::now();
    int frame_count = 0;
    int frame_size = 0;
    for (;;) {
        // Try acquiring a kh::Kinect::KinectFrame until a vaild one gets found.
        auto kinect_frame = device->acquireFrame();
        if (!kinect_frame)
            continue;

        // Try receiving a frame ID from the receiver and update receiver_frame_id if possible.
        auto receive_result = sender.receive();
        if(receive_result) {
            int cursor = 0;

            // Currently, message_type is not getting used.
            auto message_type = (*receive_result)[0];
            cursor += 1;

            if(message_type == 0) {
                memcpy(&receiver_frame_id, receive_result->data() + cursor, 4);
            }
        }

        // If more than MAXIMUM_FRAME_ID_DIFF frames are sent to the receiver without receiver_frame_id getting updated,
        // stop sending more.
        if(frame_id - receiver_frame_id > MAXIMUM_FRAME_ID_DIFF)
            continue;

        // Format the color pixels from the Kinect for the Vp8Encoder then encode the pixels with Vp8Encoder.
        auto yuv_image = createHalvedYuvImageFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto vp8_frame = encoder.encode(yuv_image);

        // Compress the depth pixels.
        auto rvl_frame = createRvlFrameFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());

        // Print profile measures every 100 frames.
        if (frame_id % 100 == 0) {
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> diff = end - start;
            std::cout << "Sending frame " << frame_id
                      << ", FPS: " << frame_count / diff.count()
                      << ", Bandwidth: " << frame_size / (diff.count() * 131072) // 131072 = 1024 * 1024 / 8
                      << " Mbps.\r";
            start = end;
            frame_count = 0;
            frame_size = 0;
        }

        // Try sending the frame. Escape the loop if there is a network error.
        try {
            sender.send(frame_id++, vp8_frame, rvl_frame);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            break;
        }

        // Updating variables for profiling.
        ++frame_count;
        frame_size += vp8_frame.size() + rvl_frame.size();
    }

    std::cout << "Stopped sending Kinect frames." << std::endl;
}

// Repeats collecting the port number from the user and calling _send_frames() with it.
void send_frames()
{
    for (;;) {
        std::cout << "Enter a port number to start sending frames: ";
        std::string line;
        std::getline(std::cin, line);
        // The default port (the port when nothing is entered) is 7777.
        int port = line.empty() ? 7777 : std::stoi(line);
        try {
            _send_frames(port);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
}

int main()
{
    kh::send_frames();
    return 0;
}