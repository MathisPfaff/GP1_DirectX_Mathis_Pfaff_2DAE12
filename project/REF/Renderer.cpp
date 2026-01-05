#include "src/pch.h"
#include "src/Renderer/HeaderFiles/Renderer.h"
#include "src/Renderer/HeaderFiles/DirectXRenderer.h"
#include "src/Renderer/HeaderFiles/SoftwareRenderer.h"
#include "src/Utils.h"

namespace dae {

	Camera dae::Renderer::m_Camera{};

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize Camera
		m_Camera.origin = Vector3{ 0.0f, 0.0f, 0.f };
		m_Camera.fov = tanf((45.0f * TO_RADIANS) / 2.0f);
		m_Camera.aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
		m_Camera.nearPlane = .1f;
		m_Camera.farPlane = 100.0f;
		m_Camera.lookSpeed = 500.f;
		m_Camera.speed = 50.f;
		m_Camera.CalculateProjectionMatrix();
		m_Camera.CalculateViewMatrix();

		m_pDirectXRenderer = new DirectXRenderer(m_pWindow, m_Camera);
	
		m_pSoftwareRenderer = new SoftwareRenderer(m_pWindow, m_Camera);

		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		std::vector<Vertex> verticesFire{};
		std::vector<uint32_t> indicesFire{};
		Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices, false);
		dae::Utils::ParseOBJ("Resources/fireFX.obj", verticesFire, indicesFire, false);
		m_pDirectXRenderer->ConstructMesh(vertices, verticesFire, indices, indicesFire);
		m_pSoftwareRenderer->ConstructMeshes(vertices, indices);
	}

	Renderer::~Renderer()
	{
		delete m_pDirectXRenderer;
		delete m_pSoftwareRenderer;

		m_pDirectXRenderer = nullptr;
		m_pSoftwareRenderer = nullptr;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);
		m_pDirectXRenderer->UpdateCamera(m_Camera);
		m_pSoftwareRenderer->UpdateCamera(m_Camera);

		m_pDirectXRenderer->Update(pTimer);
		m_pSoftwareRenderer->Update(pTimer);	
	}

	void Renderer::Render() const
	{
		switch (m_pRenderMode)
		{
		case RenderMode::Software:
			m_pSoftwareRenderer->Render();
			break;
		case RenderMode::Hardware:
			m_pDirectXRenderer->Render();
			break;
		}

	}

	void Renderer::SwitchRenderMode()
	{
		switch (m_pRenderMode)
		{
		case RenderMode::Software:
			m_pRenderMode = RenderMode::Hardware;
			m_Camera.lookSpeed = 500.f;
			m_Camera.speed = 50.f;
			std::cout << "Raterizer Mode: Hardware \n";
			break;
		case RenderMode::Hardware:
			m_pRenderMode = RenderMode::Software;
			m_Camera.lookSpeed = 10.f;
			m_Camera.speed = 50.f;
			std::cout << "Rasterizer Mode: Software \n";
			break;
		}
	}

	void Renderer::ToggleRotation()
	{
		m_pDirectXRenderer->ToggleRotationMode();
		m_pSoftwareRenderer->ToggleRotateMesh();
	}

	void Renderer::ToggleBackgroundColor()
	{
		m_pDirectXRenderer->SwitchBackgroundColor();
		m_pSoftwareRenderer->SwitchBackgroundColor();
	}

	void Renderer::RendererSpecificToggles(const SDL_Event& e)
	{
		switch (m_pRenderMode)
		{
		case RenderMode::Hardware:
			if (e.key.keysym.scancode == SDL_SCANCODE_F3)
			{
				m_pDirectXRenderer->ToggleUseFireMeshMode();
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_F4)
			{
				m_pDirectXRenderer->ToggleSampleMode();
			}
			break;

		case RenderMode::Software:
			if (e.key.keysym.scancode == SDL_SCANCODE_F5)
			{
				m_pSoftwareRenderer->SwitchRenderMode();
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_F6)
			{
				m_pSoftwareRenderer->ToggleNormalMap();
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_F7)
			{
				m_pSoftwareRenderer->SwitchDepthBuffer();
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_F8)
			{
				m_pSoftwareRenderer->RenderModeBoundingBoxes();
			}
			break;
		}
	}

	void Renderer::CycleCullMode()
	{
		m_pSoftwareRenderer->SwitchCullMode();
		m_pDirectXRenderer->ToggleCullMode();
	}

	void Renderer::SetCameraSpeed(bool holding)
	{
		if (holding) m_Camera.speed = 100.f;
		else m_Camera.speed = 50.f;
			
	}

	void Renderer::ResetCameraPosition()
	{
		m_Camera.origin = Vector3{ 0.f, 0.f, 0.f };
		m_Camera.totalPitch = 0.f;
		m_Camera.totalYaw = 0.f;

	}
}
