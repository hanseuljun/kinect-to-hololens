#include <vector>

// This algorithm comes from
// Wilson, A. D. (2017, October). Fast lossless depth image compression.
// In Proceedings of the 2017 ACM International Conference on Interactive Surfaces and Spaces (pp. 100-105). ACM.
namespace rgbd_streamer
{
namespace rvl
{
// compress() does not have a helper function with ByteFrames since it might have two kinds of sizes,
// (1) the actual size of the array and (2) the size of the portion of the array containing information.
// Also, it is not used via Unity, therefore it is uncessary to have a helper function.
std::vector<uint8_t> compress(uint16_t* depth_frame, int num_pixels);
std::vector<uint16_t> decompress(uint8_t* rvl_frame, int num_pixels);
void decompress(uint8_t* input, uint16_t* output, int width, int height, int row_pitch);
}
}