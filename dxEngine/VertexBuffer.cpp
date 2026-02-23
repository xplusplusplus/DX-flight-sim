#include "VertexBuffer.h"
#include "RenderSystem.h"
#include <exception>

VertexBuffer::VertexBuffer(void* list_vertices,UINT size_vertex,UINT size_list,void* shader_byte_code,UINT size_byte_shader,RenderSystem* system):m_layout(0),m_buffer(0),m_system(system)
{


	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT; // GPU kullanýmý tipi
	buff_desc.ByteWidth = size_vertex * size_list; // buffer boyutu
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ne için kullanýlacak
	buff_desc.CPUAccessFlags = 0; // CPU eriþimi yok
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = list_vertices;

	m_size_vertex = size_vertex;
	m_size_list = size_list;



	if (FAILED(m_system->m_device->CreateBuffer(&buff_desc, &init_data, &m_buffer))) // m_buffer dolduyoruz
		throw ;


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// SEMANTIC NAME | SEMANTIC INDEX |      FORMAT             | INPUT SLOT | ALIGNED BYTE OFFSET |        INPUT SLOT CLASS        | INSTANCE DATA STEP RATE
		{ "POSITION",       0,          DXGI_FORMAT_R32G32B32_FLOAT,       0,              0,                    D3D11_INPUT_PER_VERTEX_DATA,           0 },
		{ "TEXCOORD",		0,          DXGI_FORMAT_R32G32_FLOAT,          0,              12,                    D3D11_INPUT_PER_VERTEX_DATA,           0 },
		{ "NORMAL",       0,          DXGI_FORMAT_R32G32B32_FLOAT,       0,					20,                    D3D11_INPUT_PER_VERTEX_DATA,           0 },

	};

	UINT size_layout = ARRAYSIZE(layout);

	if (FAILED(m_system->m_device->CreateInputLayout(layout, size_layout, shader_byte_code, size_byte_shader, &m_layout))) {
		throw std::exception("VertexBUffer not created sccessfully");
	}


}

VertexBuffer::~VertexBuffer()
{
	m_buffer->Release();
	m_layout->Release();
}



UINT VertexBuffer::getSizeVertexList()
{
	return this->m_size_list;
}

