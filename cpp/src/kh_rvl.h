#pragma once

#include <vector>

// This algorithm is from
// Wilson, A. D. (2017, October). Fast lossless depth image compression.
// In Proceedings of the 2017 ACM International Conference on Interactive Surfaces and Spaces (pp. 100-105). ACM.
namespace kh
{
namespace rvl
{
std::vector<uint8_t> compress(uint16_t* input, int num_pixels);
std::vector<uint16_t> decompress(uint8_t* input, int num_pixels);
void decompress(uint8_t* input, uint16_t* output, int width, int height, int row_pitch);
}
}