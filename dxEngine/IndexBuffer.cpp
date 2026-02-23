#include "IndexBuffer.h"
#include "RenderSystem.h"
#include <exception>

IndexBuffer::IndexBuffer(void* list_indices, UINT size_list,RenderSystem* system) :m_system(system), m_buffer(0)
{
	if (m_buffer) m_buffer->Release(); // eski buffer datasý silinir


	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT; // GPU kullanýmý tipi
	buff_desc.ByteWidth = 4 * size_list; // buffer boyutu
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ne için kullanýlacak
	buff_desc.CPUAccessFlags = 0; // CPU eriþimi yok
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = list_indices;

	m_size_list = size_list;



	if (FAILED(m_system->m_device->CreateBuffer(&buff_desc, &init_data, &m_buffer))) // m_buffer dolduyoruz
		throw std::exception("IndexBuffer not created successfully");


}

IndexBuffer::~IndexBuffer()
{
	m_buffer->Release();
}



UINT IndexBuffer::getSizeIndexList()
{
	return this->m_size_list;
}


