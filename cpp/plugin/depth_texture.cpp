#include "depth_texture.h"

#include <string>
#include "kh_rvl.h"

namespace kh
{
DepthTexture::DepthTexture(ID3D11Device* device, int width, int height)
	: width_(width), height_(height), texture_(nullptr)
{
    // D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE are chosen to update pixels in DepthTexture::updatePixels().
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R16_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&desc, 0, &texture_);

    if (FAILED(hr)) {
        std::string str = "DepthTexture::DepthTexture failed, result: " + std::to_string(hr) + ", texture: " + std::to_string((uint64_t)texture_);
        throw std::exception(str.c_str());
    }
}

DepthTexture::~DepthTexture()
{
	if (texture_)
		texture_->Release();
}

ID3D11ShaderResourceView* DepthTexture::getTextureView(ID3D11Device* device)
{
	ID3D11ShaderResourceView* texture_view;
	HRESULT hr = device->CreateShaderResourceView(texture_, 0, &texture_view);
	return texture_view;
}

// Update the pixels of the texture with depth pixels compressed by RVL.
// The pixels get decompressed and assigned at once for optimization.
void DepthTexture::updatePixels(ID3D11Device* device,
								ID3D11DeviceContext* device_context,
								int width,
								int height,
								std::vector<uint8_t>& frame)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = device_context->Map(texture_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr)) {
		std::string str = "DepthTexture::updatePixels failed, result: " + std::to_string(hr) + ", texture: " + std::to_string((uint64_t)texture_);
		throw std::exception(str.c_str());
	}

	// row_pitch should be divided by two to support uint16* since mapped.RowPitch is for bytes.
	int row_pitch = mapped.RowPitch / 2;
	rvl::decompress(frame.data(), reinterpret_cast<uint16_t*>(mapped.pData), width, height, row_pitch);

	device_context->Unmap(texture_, 0);
}
}