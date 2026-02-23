#include "SwapChain.h"
#include "RenderSystem.h"
#include <exception>

SwapChain::SwapChain(HWND hwnd, UINT width, UINT height,RenderSystem* system) :m_system(system)
{
	ID3D11Device* device = m_system->m_device;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferCount = 1;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hwnd;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    

	HRESULT hr= m_system->m_dxgi_factory->CreateSwapChain(device,&desc, &m_swapChain);

	if (FAILED(hr))
		throw std::exception("swapChain not created sccessfully");

	reloadBuffers(width, height);
}

void SwapChain::setFullScreen(bool fullscreen, unsigned int width, unsigned int height)
{
	resize(width, height);
	m_swapChain->SetFullscreenState(fullscreen, nullptr);
}



	

void SwapChain::resize(unsigned int width, unsigned int height)
{
	if (m_rtv) m_rtv->Release();
	if (m_dsv) m_dsv->Release();


	m_swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	reloadBuffers(width, height);
}

bool SwapChain::present(bool vsync)
{
	m_swapChain->Present(vsync, 0);
	
	return true;
}



SwapChain::~SwapChain()
{
	m_rtv->Release();
	m_swapChain->Release();
}

void SwapChain::reloadBuffers(unsigned int width, unsigned int height)
{
	ID3D11Device* device = m_system->m_device;

	ID3D11Texture2D* buffer = NULL;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

	if (FAILED(hr))
		throw std::exception("swapChain not created sccessfully");

	hr = device->CreateRenderTargetView(buffer, NULL, &m_rtv);
	buffer->Release();

	if (FAILED(hr))
		throw std::exception("swapChain not created sccessfully");


	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex_desc.MipLevels = 1;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.MiscFlags = 0;
	tex_desc.ArraySize = 1;
	tex_desc.CPUAccessFlags = 0;

	hr = device->CreateTexture2D(&tex_desc, nullptr, &buffer);

	if (FAILED(hr))
		throw std::exception("swapChain not created sccessfully");

	hr = device->CreateDepthStencilView(buffer, NULL, &m_dsv);
	buffer->Release();

	if (FAILED(hr))
		throw std::exception("CreateDepthStencilView failed");
}
