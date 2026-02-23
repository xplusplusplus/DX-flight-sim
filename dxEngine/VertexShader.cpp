#include "VertexShader.h"
#include "GraphicsEngine.h"
#include <exception>

VertexShader::VertexShader(const void* shader_byte_code,size_t byte_code_size,RenderSystem* system):m_system(system)
{
    if (!SUCCEEDED(m_system->m_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &m_vs)))
    {
        throw std::exception("VertexShader not created sccessfully");
    }
    
}



VertexShader::~VertexShader()
{ 
    m_vs->Release();
}

