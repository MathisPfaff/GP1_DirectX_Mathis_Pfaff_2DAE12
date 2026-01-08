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
	m_MeshRotationRadians(0.0f)
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
		std::cout << "DirectX renderer initialized successfully\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize DirectX renderer: " << e.what() << "\n";
		m_pDirectXRenderer = nullptr;
	}

	try
	{
		m_pSoftwareRenderer = std::make_unique<S_Renderer>(pWindow, &m_Camera, &m_MeshRotationRadians);
		std::cout << "Software renderer initialized successfully\n";
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
		std::cout << "Switched to Software renderer due to DirectX initialization failure\n";
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
	m_MeshRotationRadians += MESH_ROTATION_SPEED * pTimer->GetElapsed() * 3.14159f / 180.f;

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
			std::cout << "Switched to Software renderer\n";
		}
		else
		{
			std::cout << "Software renderer not available\n";
		}
	}
	else
	{
		if (m_pDirectXRenderer)
		{
			m_CurrentRenderer = RendererType::DirectX;
			std::cout << "Switched to DirectX renderer\n";
		}
		else
		{
			std::cout << "DirectX renderer not available\n";
		}
	}
}