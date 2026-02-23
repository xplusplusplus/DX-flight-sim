#include "Texture.h"
#include <DirectXTex.h>
#include "GraphicsEngine.h"

Texture::Texture(const wchar_t* full_path): Resource(full_path)
{
	DirectX::ScratchImage image_data;
	HRESULT res = DirectX::LoadFromWICFile(full_path, DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, image_data); // image_data doldurulur (cpu)

	if (SUCCEEDED(res)) 
	{
		res = DirectX::CreateTexture(GraphicsEngine::get()->getRenderSystem()->m_device, image_data.GetImages()
			, image_data.GetImageCount(), image_data.GetMetadata(), &m_texture); // gpu tarafýnda cpuda ki image_data texture düþtürülür m_texture da o texture tutar
		if(FAILED(res)) throw std::exception("Texture not created successfully");

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = image_data.GetMetadata().format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = image_data.GetMetadata().mipLevels;
		desc.Texture2D.MostDetailedMip = 0;


		D3D11_SAMPLER_DESC sampler_desc = {};
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = image_data.GetMetadata().mipLevels;


		res = GraphicsEngine::get()->getRenderSystem()->m_device->CreateSamplerState(&sampler_desc,&m_sampler_state);
		if (FAILED(res)) throw std::exception("Texture not created successfully");

		res =GraphicsEngine::get()->getRenderSystem()->m_device->CreateShaderResourceView(m_texture, &desc, &m_shader_res_view); // shader texture eriþimi saðlanýr
		if (FAILED(res)) throw std::exception("Texture not created successfully");

	}
	else
	{
		throw std::exception("Texture not created successfully");
	}
}

Texture::~Texture()
{
	m_shader_res_view->Release();
	m_texture->Release();
}
