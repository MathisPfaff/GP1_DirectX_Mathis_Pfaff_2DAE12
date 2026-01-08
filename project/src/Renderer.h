#pragma once

// SDL Headers
#include "SDL.h"
#include "DataTypes.h"
#include "Camera.h"

// Forward declarations
class D_Texture;

namespace dae
{
	class D_Renderer;
	class S_Renderer;
	class Timer;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void SetSamplerFilter(SamplerFilter filter);
		void SwitchRenderer();
		void Changerotation();
		void ToggleFireMesh();

		// Software renderer controls
		void CycleShadingMode();
		void ToggleNormalMap();
		void ToggleDepthBuffer();
		void ToggleBoundingBox();

		// Cull mode control
		void CycleCullMode();

		// Clear color control
		void ToggleClearColor();

		// Getters for shared camera and rotation
		const Camera& GetCamera() const { return m_Camera; }
		float GetMeshRotationRadians() const { return m_MeshRotationRadians; }
		CullMode GetCurrentCullMode() const { return m_CurrentCullMode; }
		bool GetUseUniformClearColor() const { return m_UseUniformClearColor; }

	private:
		SDL_Window* m_pWindow{};
		RendererType m_CurrentRenderer{ RendererType::DirectX };

		int m_Width{};
		int m_Height{};

		bool m_RotateMesh{ true };

		// Shared camera
		Camera m_Camera{};

		// Shared mesh rotation state
		float m_MeshRotationRadians{ 0.0f };
		static constexpr float MESH_ROTATION_SPEED{ 45.0f }; // degrees per second

		// Renderer instances
		std::unique_ptr<S_Renderer> m_pSoftwareRenderer{};
		std::unique_ptr<D_Renderer> m_pDirectXRenderer{};

		// Shared rendering state
		CullMode m_CurrentCullMode{ CullMode::BackFace };
		bool m_UseUniformClearColor{ false };
	};
}
