#include "ConstantBuffer.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include <execution>

ConstantBuffer::ConstantBuffer(void* buffer, UINT size_buffer,RenderSystem* system) :m_system(system)
{
	//if (m_buffer) m_buffer->Release(); // eski buffer datasý silinir

	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT; // GPU kullanýmý tipi
	buff_desc.ByteWidth = size_buffer; // buffer boyutu
	buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // ne için kullanýlacak
	buff_desc.CPUAccessFlags = 0; // CPU eriþimi yok
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = buffer;





	if (FAILED(m_system->m_device->CreateBuffer(&buff_desc, &init_data, &m_buffer))) // m_buffer dolduyoruz
		throw std::exception("ConstantBUffer not created successfully");

}

ConstantBuffer::~ConstantBuffer()
{
	if (m_buffer) m_buffer->Release();
}


void ConstantBuffer::update(DeviceContextPtr context, void* buffer)
{
	context->m_device_context->UpdateSubresource(this->m_buffer, NULL, NULL, buffer, NULL, NULL); // buffer içeriðini editliyoruzz her frame 
}


