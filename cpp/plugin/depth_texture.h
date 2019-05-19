#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>

namespace kh
{
class DepthTexture
{
public:
    DepthTexture(ID3D11Device* device, int width, int height);
    ~DepthTexture();
    int width() { return width_; }
    int height() { return height_; }
    ID3D11ShaderResourceView* getTextureView(ID3D11Device* device);
	void updatePixels(ID3D11Device* device,
					  ID3D11DeviceContext* device_context,
					  int width,
					  int height,
					  std::vector<uint8_t>& frame);

private:
    int width_;
    int height_;
    ID3D11Texture2D* texture_;
};
}