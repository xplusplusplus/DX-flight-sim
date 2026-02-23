#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include "DeviceContext.h"
#include <exception>

GraphicsEngine* GraphicsEngine::m_engine = nullptr;

GraphicsEngine::GraphicsEngine()
{	try 
	{
		m_render_system = new RenderSystem();
	}
	catch (...) { throw std::exception("Graphic engine not created sccessfully"); }

	try
	{
		m_tex_manager = new TextureManager();
	}
	catch (...) { throw std::exception("TextureManager not created sccessfully"); }

	try
	{
		m_mesh_manager = new MeshManager();
	}
	catch (...) { throw std::exception("MeshManager not created sccessfully"); }


	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	m_render_system->compileVertexShader(
		L"VertexMeshLayoutShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	::memcpy(m_mesh_layout_byte_code, shader_byte_code,size_shader);
	m_mesh_layout_size = size_shader;
	m_render_system->releaseCompiledShader();  // serbest býrak
}



GraphicsEngine::~GraphicsEngine()
{
	GraphicsEngine::m_engine = nullptr;
	delete m_mesh_manager;
	delete m_tex_manager;
	delete m_render_system;
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	
	return m_render_system;
}

TextureManager* GraphicsEngine::getTextureManager()
{
	
	return m_tex_manager;
}

MeshManager* GraphicsEngine::getMeshManager()
{
	return m_mesh_manager;
}

MaterialPtr GraphicsEngine::createMaterial(const wchar_t* vertex_shader_path, const wchar_t* pixel_shader_path)
{
	MaterialPtr mat = nullptr;
	try
	{
		mat = std::make_shared<Material>(vertex_shader_path,pixel_shader_path);
	}
	catch (...) {}
	return mat;
}

MaterialPtr GraphicsEngine::createMaterial(const MaterialPtr& material)
{
	MaterialPtr mat = nullptr;
	try
	{
		mat = std::make_shared<Material>(material);
	}
	catch (...) {}
	return mat;
}

void GraphicsEngine::setMaterial(const MaterialPtr& material)
{
	GraphicsEngine::get()->getRenderSystem()->setRasterizerState((material->m_cull_mode == CULL_MODE_FRONT));
	GraphicsEngine::get()->getRenderSystem()->getDevieContext()->setConstantBuffer(material->m_vertex_shader, material->m_constant_buffer);

	GraphicsEngine::get()->getRenderSystem()->getDevieContext()->setConstantBuffer(material->m_pixel_shader, material->m_constant_buffer);

	// shaderlarý etkinleþtir
	GraphicsEngine::get()->getRenderSystem()->getDevieContext()->setVertexShader(material->m_vertex_shader);
	GraphicsEngine::get()->getRenderSystem()->getDevieContext()->setPixelShader(material->m_pixel_shader);

	GraphicsEngine::get()->getRenderSystem()->getDevieContext()->setTextureBuffer(material->m_pixel_shader, &material->m_vec_textures[0], material->m_vec_textures.size());
}

void GraphicsEngine::getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size)
{
	*byte_code = m_mesh_layout_byte_code;
	*size = m_mesh_layout_size;
}

GraphicsEngine* GraphicsEngine::get()
{
	
	return m_engine;
}

void GraphicsEngine::create()
{
	if (GraphicsEngine::m_engine) throw std::exception("Graphics engine already created");
	GraphicsEngine::m_engine = new GraphicsEngine();
}

void GraphicsEngine::release()
{
	if (!GraphicsEngine::m_engine) return;
	delete GraphicsEngine::m_engine;
}
