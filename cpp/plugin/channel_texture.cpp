#include "channel_texture.h"

#include <string>

namespace kh
{
ChannelTexture::ChannelTexture(int width, int height, ID3D11Texture2D* texture)
	: width_(width), height_(height), texture_(texture)
{
}

ChannelTexture::~ChannelTexture()
{
	if (texture_)
		texture_->Release();
}

std::unique_ptr<ChannelTexture> ChannelTexture::create(ID3D11Device* device, int width, int height)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	HRESULT hr = device->CreateTexture2D(&desc, 0, &texture);

	if (FAILED(hr)) {
		std::string str = "ChannelTexture::create failed, result: " + std::to_string(hr) + ", texture: " + std::to_string((uint64_t)texture);
        throw std::exception(str.c_str());
	}

	return std::unique_ptr<ChannelTexture>(new ChannelTexture(width, height, texture));
}

ID3D11ShaderResourceView* ChannelTexture::getTextureView(ID3D11Device* device)
{
	ID3D11ShaderResourceView* texture_view;
	device->CreateShaderResourceView(texture_, 0, &texture_view);
	return texture_view;
}

void ChannelTexture::updatePixels(ID3D11Device* device,
								  ID3D11DeviceContext* device_context,
								  int width,
				 				  int height,
								  FFmpegFrame& frame,
								  int index)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	device_context->Map(texture_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	int row_pitch = mapped.RowPitch;
	uint8_t* texture_data = (uint8_t*)mapped.pData;

	uint8_t* frame_data = frame.av_frame()->data[index];
	int frame_linesize = frame.av_frame()->linesize[index];

	for (int i = 0; i < height; ++i)
		memcpy(texture_data + i * row_pitch, frame_data + i * frame_linesize, width);

	device_context->Unmap(texture_, 0);
}
}