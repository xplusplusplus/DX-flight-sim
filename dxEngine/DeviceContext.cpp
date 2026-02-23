#include "DeviceContext.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <execution>

DeviceContext::DeviceContext(ID3D11DeviceContext* device_context,RenderSystem* system) :m_system(system),m_device_context(device_context)
{
}

void DeviceContext::clearRenderTargetColor(const SwapChainPtr& swap_chain,
	float red, float green, float blue, float alpha)
{
	FLOAT clear_color[] = { red, green, blue, alpha }; // RTV'yi bu renge boya

	m_device_context->ClearRenderTargetView(
		swap_chain->m_rtv,     // Hangi render target temizlenecek (arka buffer)
		clear_color            // Temizleme rengi (RGBA)
	);

	m_device_context->ClearDepthStencilView(
		swap_chain->m_dsv,                     // Depth-Stencil buffer
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, // Depth + Stencil temizle
		1.0f,                                  // Depth deðeri (en uzak)
		0                                      // Stencil deðeri
	);

	m_device_context->OMSetRenderTargets(
		1,                     // Kaç render target baðlanacak
		&swap_chain->m_rtv,    // Render target array
		swap_chain->m_dsv      // Depth-stencil buffer
	);
}


void DeviceContext::setVertexBuffer(const VertexBufferPtr& vertex_buffer)
{
	UINT stride = vertex_buffer->m_size_vertex; // 1 vertex kaç byte
	UINT offset = 0;                            // Buffer içindeki baþlangýç offseti

	m_device_context->IASetVertexBuffers(
		0,                          // Slot (genelde 0)
		1,                          // Kaç vertex buffer
		&vertex_buffer->m_buffer,   // ID3D11Buffer*
		&stride,                    // Vertex baþýna byte
		&offset                     // Baþlangýç offset
	);

	m_device_context->IASetInputLayout(
		vertex_buffer->m_layout     // Vertex struct ? VS input eþleþtirmesi vertex nasýl okunacak
	);
}


void DeviceContext::drawTriangleList(UINT vertex_count, UINT start_vertex_index)
{
	m_device_context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST // 3 vertex = 1 triangle
	);

	m_device_context->Draw(
		vertex_count,        // Kaç vertex çizilecek
		start_vertex_index   // Baþlangýç vertex index
	);
}


void DeviceContext::drawIndexedTriangleList(UINT index_count,
											UINT start_vertex_index,
											UINT start_index_loc)
{
	m_device_context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST // Indexed triangle
	);

	m_device_context->DrawIndexed(
		index_count,          // Kaç index
		start_index_loc,      // Index buffer baþlangýcý
		start_vertex_index    // Vertex buffer offset
	);
}


void DeviceContext::drawTriangleStrip(UINT vertex_count, UINT start_vertex_index)
{
	m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_device_context->Draw(vertex_count, start_vertex_index);
}

void DeviceContext::setIndexBuffer(const IndexBufferPtr& Index_buffer)
{
	m_device_context->IASetIndexBuffer(
		Index_buffer->m_buffer,     // Index buffer
		DXGI_FORMAT_R32_UINT,       // Index tipi (unsigned int)
		0                           // Offset
	);
}



void DeviceContext::setViewportSize(UINT width, UINT height)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = (float)width;   // Viewport geniþliði (pixel)
	vp.Height = (float)height;  // Viewport yüksekliði
	vp.MinDepth = 0.0f;           // Depth minimum
	vp.MaxDepth = 1.0f;           // Depth maksimum

	m_device_context->RSSetViewports(
		1,        // Kaç viewport
		&vp       // Viewport bilgisi
	);
}

/*
void DeviceContext::VSSetShader(ID3D11VertexShader* vs)
{
	m_device_context->VSSetShader(vs, nullptr, 0);
}

void DeviceContext::PSSetShader(ID3D11PixelShader* ps)
{
	m_device_context->PSSetShader(ps, nullptr, 0);
}*/

void DeviceContext::setVertexShader(const VertexShaderPtr& vertex_shader)
{
	m_device_context->VSSetShader(
		vertex_shader->m_vs, // Aktif olacak vertex shader
		nullptr,             // Class instance (DX11'de kullanýlmaz)
		0                    // Instance sayýsý
	);
}


void DeviceContext::setPixelShader(const PixelShaderPtr& pixel_shader)
{
	m_device_context->PSSetShader(
		pixel_shader->m_ps, // Aktif pixel shader
		nullptr,
		0
	);
}


void DeviceContext::setTextureBuffer(const VertexShaderPtr& vertex_shader, const TexturePtr* texture, unsigned int num_texs)
{
	ID3D11ShaderResourceView* list_res[32];
	ID3D11SamplerState* list_sampler[32];
	for (unsigned int i = 0; i < num_texs; i++)
	{
		list_res[i] = texture[i]->m_shader_res_view;
		list_sampler[i] = texture[i]->m_sampler_state;
	}

	m_device_context->VSSetShaderResources(0, num_texs, list_res); // slot 0dan baþla num_texs kadar kaçýncý slota kadar yüklenvek list_res de texture
	m_device_context->VSSetSamplers(0, num_texs, list_sampler);
}

void DeviceContext::setTextureBuffer(const PixelShaderPtr& pixel_shader,const TexturePtr* texture, unsigned int num_texs)
{
	ID3D11ShaderResourceView* list_res[32];
	ID3D11SamplerState* list_sampler[32];
	for (unsigned int i = 0; i < num_texs; i++)
	{
		list_res[i] = texture[i]->m_shader_res_view;
		list_sampler[i] = texture[i]->m_sampler_state;
	}

	m_device_context->PSSetShaderResources(0, num_texs, list_res);
	m_device_context->PSSetSamplers(0, num_texs, list_sampler);
}

void DeviceContext::setConstantBuffer(const VertexShaderPtr& vertex_shader, const ConstantBufferPtr& buffer)
{
	m_device_context->VSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

void DeviceContext::setConstantBuffer(const PixelShaderPtr& pixel_shader, const ConstantBufferPtr& buffer)
{
	m_device_context->PSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

DeviceContext::~DeviceContext()
{
	m_device_context->Release();
}
