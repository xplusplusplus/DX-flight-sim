#include "RenderSystem.h"
#include "RenderSystem.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "d3dcompiler.h"
#include <exception>
#include <iostream>

RenderSystem::RenderSystem()
{
    D3D_DRIVER_TYPE driver_types[] =
    {
        D3D_DRIVER_TYPE_HARDWARE, // gerçek ekran kartý
        D3D_DRIVER_TYPE_WARP,     // cpu
        D3D_DRIVER_TYPE_REFERENCE //
    };
    UINT num_driver_types = ARRAYSIZE(driver_types);

    D3D_FEATURE_LEVEL feature_levels[] =
    {
        D3D_FEATURE_LEVEL_11_0 // directx sürümü
    };
    UINT num_feature_levels = ARRAYSIZE(feature_levels);
    HRESULT res = 0;

    
    for (UINT driver_type_index = 0; driver_type_index < num_driver_types;)
    {
        res = D3D11CreateDevice(
            NULL, // varsayýlan adaptör
            driver_types[driver_type_index], // GPU tipi ekran katýmý cpu mu falan
            NULL, 
            NULL,
            feature_levels, // dx sürümü
            num_feature_levels,
            D3D11_SDK_VERSION,
            &m_device,      // GPU temsil eden nesne
            &m_feature_level,
            &m_context // gpu gönderilecek context
        );
        if (SUCCEEDED(res))
            break;
        ++driver_type_index; // GPU seçme sürücü kontollü bellek ayýrma gpu ile ilk temas
    }

    if (FAILED(res))
        throw std::exception("RenderSystem not created sccessfully");

    m_device_context = std::make_shared<DeviceContext>(m_context,this);

    m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
    m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
    m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);
    
    initRasterizerState();
}





RenderSystem::~RenderSystem()
{
    m_dxgi_factory->Release();
    m_dxgi_adapter->Release();
    m_dxgi_device->Release();


    m_device->Release();

    if (m_vs) m_vs->Release();
    if (m_ps) m_ps->Release();
    if (m_vsblob) m_vsblob->Release();
    if (m_psblob) m_psblob->Release();
}

SwapChainPtr RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
{
   
    
    return std::make_shared<SwapChain>(hwnd, width, height,this);
   
    
}

DeviceContextPtr RenderSystem::getDevieContext()
{
    return this->m_device_context;
}

VertexBufferPtr RenderSystem::createVertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, UINT size_byte_shader)
{
    VertexBufferPtr vb = nullptr;
    try
    {
        vb = std::make_shared<VertexBuffer>(list_vertices, size_vertex, size_list, shader_byte_code, size_byte_shader, this);
    }
    catch (...) {}
    return vb;
}

IndexBufferPtr RenderSystem::createIndexBuffer(void* list_indices, UINT size_list)
{
    IndexBufferPtr ib = nullptr;
    try
    {
        ib = std::make_shared<IndexBuffer>(list_indices, size_list,this);
    }
    catch (...) {}
    return ib;
}

ConstantBufferPtr RenderSystem::createConstantBuffer(void* buffer, UINT size_buffer)
{
    ConstantBufferPtr ib = nullptr;
    try
    {
        ib = std::make_shared<ConstantBuffer>(buffer, size_buffer, this);
    }
    catch (...) {}
    return ib;
}

VertexShaderPtr RenderSystem::createVertexShader(const void* shader_byte_code, size_t byte_code_size)
{
    VertexShaderPtr vs = nullptr;
    try
    {
        vs = std::make_shared<VertexShader>(shader_byte_code, byte_code_size, this);
    }
    catch (...) {}
    return vs;

    
}

PixelShaderPtr RenderSystem::createPixelShader(const void* shader_byte_code, size_t byte_code_size)
{
    
    PixelShaderPtr ps = nullptr;
    try
    {
        ps = std::make_shared<PixelShader>(shader_byte_code, byte_code_size, this);
    }
    catch (...) {}
    return ps;
    
    
}

bool RenderSystem::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
    ID3DBlob* error_blob = nullptr;

    if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "vs_5_0", 0, 0, &m_blob, &error_blob)))
    {
        if (error_blob) error_blob->Release();
        return false;
    }


    *shader_byte_code = m_blob->GetBufferPointer();
    *byte_code_size = m_blob->GetBufferSize();
    return true;
}

bool RenderSystem::compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
    ID3DBlob* error_blob = nullptr;

    if (!SUCCEEDED(::D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "ps_5_0", 0, 0, &m_blob, &error_blob)))
    {
        if (error_blob) error_blob->Release();
        return false;
    }


    *shader_byte_code = m_blob->GetBufferPointer();
    *byte_code_size = m_blob->GetBufferSize();
    return true;
}

void RenderSystem::releaseCompiledShader()
{
    if (m_blob)m_blob->Release();
}

void RenderSystem::setRasterizerState(bool cull_front)
{
    if (cull_front)
        m_context->RSSetState(m_cull_front_state);
    else
        m_context->RSSetState(m_cull_back_state);

}

void RenderSystem::initRasterizerState()
{
    D3D11_RASTERIZER_DESC desc = {};
    desc.CullMode = D3D11_CULL_FRONT;
    desc.DepthClipEnable = true;
    desc.FillMode = D3D11_FILL_SOLID;

    m_device->CreateRasterizerState(&desc, &m_cull_front_state);

    desc.CullMode = D3D11_CULL_BACK;

    m_device->CreateRasterizerState(&desc, &m_cull_back_state);

}






