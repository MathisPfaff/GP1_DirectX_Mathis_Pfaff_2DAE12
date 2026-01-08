//External includes
#include "SDL.h"
#include "SDL_surface.h"

// Standard includes
#include <iostream>
#include <cassert>
#include <memory>

//Project includes
#include "Renderer.h"
#include "SoftwareRenderer.h"
#include "DirectXRenderer.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow),
	m_CurrentRenderer(RendererType::DirectX),
	m_MeshRotationRadians(0.0f),
	m_CurrentCullMode(CullMode::BackFace),
	m_UseUniformClearColor(false)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	// Initialize shared camera
	m_Camera.Initialize(45.f, Vector3(0.f, 0.f, 0.f), float(m_Width) / float(m_Height));
	m_Camera.CalculateViewMatrix();
	m_Camera.CalculateProjectionMatrix();

	// Initialize both renderers
	try
	{
		m_pDirectXRenderer = std::make_unique<D_Renderer>(pWindow, &m_Camera, &m_MeshRotationRadians);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize DirectX renderer: " << e.what() << "\n";
		m_pDirectXRenderer = nullptr;
	}

	try
	{
		m_pSoftwareRenderer = std::make_unique<S_Renderer>(pWindow, &m_Camera, &m_MeshRotationRadians);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize Software renderer: " << e.what() << "\n";
		m_pSoftwareRenderer = nullptr;
	}

	// If DirectX failed to initialize, fall back to Software
	if (!m_pDirectXRenderer && m_pSoftwareRenderer)
	{
		m_CurrentRenderer = RendererType::Software;
	}
}

Renderer::~Renderer()
{
	m_pDirectXRenderer.reset();
	m_pSoftwareRenderer.reset();
}

void Renderer::Update(const Timer* pTimer)
{
	// Update shared camera
	m_Camera.Update(const_cast<Timer*>(pTimer));

	// Update shared mesh rotation (45 degrees per second)
	if (m_RotateMesh)
	{
		m_MeshRotationRadians += MESH_ROTATION_SPEED * pTimer->GetElapsed() * 3.14159f / 180.f;
	}

	// Update the active renderer
	if (m_CurrentRenderer == RendererType::DirectX && m_pDirectXRenderer)
	{
		m_pDirectXRenderer->Update(pTimer);
	}
	else if (m_CurrentRenderer == RendererType::Software && m_pSoftwareRenderer)
	{
		m_pSoftwareRenderer->Update(pTimer);
	}
}

void Renderer::Render() const
{
	if (m_CurrentRenderer == RendererType::DirectX && m_pDirectXRenderer)
	{
		m_pDirectXRenderer->Render();
	}
	else if (m_CurrentRenderer == RendererType::Software && m_pSoftwareRenderer)
	{
		m_pSoftwareRenderer->Render();
	}
}

void Renderer::SetSamplerFilter(SamplerFilter filter)
{
	std::cout << "** F4 KEY ** Sampler Filter changed to: ";
	switch (filter)
	{
	case SamplerFilter::Point:
		std::cout << "Point";
		break;
	case SamplerFilter::Linear:
		std::cout << "Linear";
		break;
	case SamplerFilter::Anisotropic:
		std::cout << "Anisotropic";
		break;
	}
	std::cout << std::endl;

	if (m_CurrentRenderer == RendererType::DirectX && m_pDirectXRenderer)
	{
		m_pDirectXRenderer->SetSamplerFilter(filter);
	}
}

void Renderer::SwitchRenderer()
{
	if (m_CurrentRenderer == RendererType::DirectX)
	{
		if (m_pSoftwareRenderer)
		{
			m_CurrentRenderer = RendererType::Software;
			std::cout << "** F1 KEY ** Renderer switched to: Software Renderer" << std::endl;
		}
	}
	else
	{
		if (m_pDirectXRenderer)
		{
			m_CurrentRenderer = RendererType::DirectX;
			std::cout << "** F1 KEY ** Renderer switched to: DirectX Renderer" << std::endl;
		}
	}
}

void Renderer::Changerotation()
{
	m_RotateMesh = !m_RotateMesh;
	std::cout << "** F2 KEY ** Mesh rotation: " << (m_RotateMesh ? "ON" : "OFF") << std::endl;
}

void Renderer::ToggleFireMesh()
{
	if (m_CurrentRenderer == RendererType::DirectX && m_pDirectXRenderer)
	{
		std::cout << "** F3 KEY ** ";
		m_pDirectXRenderer->ToggleFireMesh();
	}
}

void Renderer::CycleShadingMode()
{
	if (m_CurrentRenderer == RendererType::Software && m_pSoftwareRenderer)
	{
		std::cout << "** F5 KEY ** ";
		m_pSoftwareRenderer->CycleShadingMode();
	}
}

void Renderer::ToggleNormalMap()
{
	if (m_CurrentRenderer == RendererType::Software && m_pSoftwareRenderer)
	{
		std::cout << "** F6 KEY ** ";
		m_pSoftwareRenderer->ToggleNormalMap();
	}
}

void Renderer::ToggleDepthBuffer()
{
	if (m_CurrentRenderer == RendererType::Software && m_pSoftwareRenderer)
	{
		std::cout << "** F7 KEY ** ";
		m_pSoftwareRenderer->SwitchDepthBuffer();
	}
}

void Renderer::ToggleBoundingBox()
{
	if (m_CurrentRenderer == RendererType::Software && m_pSoftwareRenderer)
	{
		std::cout << "** F8 KEY ** ";
		m_pSoftwareRenderer->ToggleBoundingBox();
	}
}

void Renderer::CycleCullMode()
{
	std::string cullModeName;
	switch (m_CurrentCullMode)
	{
	case CullMode::BackFace:
		m_CurrentCullMode = CullMode::FrontFace;
		cullModeName = "Front Face";
		break;
	case CullMode::FrontFace:
		m_CurrentCullMode = CullMode::None;
		cullModeName = "None";
		break;
	case CullMode::None:
		m_CurrentCullMode = CullMode::BackFace;
		cullModeName = "Back Face";
		break;
	}

	std::cout << "** F9 KEY ** Cull Mode changed to: " << cullModeName << std::endl;

	// Apply cull mode to both renderers
	if (m_pDirectXRenderer)
	{
		m_pDirectXRenderer->SetCullMode(m_CurrentCullMode);
	}
	if (m_pSoftwareRenderer)
	{
		m_pSoftwareRenderer->SetCullMode(m_CurrentCullMode);
	}
}

void Renderer::ToggleClearColor()
{
	m_UseUniformClearColor = !m_UseUniformClearColor;
	std::cout << "** F10 KEY ** Uniform Clear Color: " << (m_UseUniformClearColor ? "ON" : "OFF") << std::endl;

	// Apply clear color settings to both renderers
	if (m_pDirectXRenderer)
	{
		m_pDirectXRenderer->SetUniformClearColor(m_UseUniformClearColor);
	}
	if (m_pSoftwareRenderer)
	{
		m_pSoftwareRenderer->SetUniformClearColor(m_UseUniformClearColor);
	}
}