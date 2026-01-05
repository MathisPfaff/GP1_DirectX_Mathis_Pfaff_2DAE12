#pragma once
#include "src/Renderer/HeaderFiles/SoftwareRenderer.h"
#include "src/Renderer/HeaderFiles/DirectXRenderer.h"
struct SDL_Window;
struct SDL_Surface;

class SoftwareRenderer;
class DirectXRenderer;

namespace dae
{
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

		enum class RenderMode
		{
			Software, Hardware
		};

		void SwitchRenderMode();
		void ToggleRotation();
		void ToggleBackgroundColor();
		void RendererSpecificToggles(const SDL_Event& e);
		void CycleCullMode();
		void SetCameraSpeed(bool holding);
		void ResetCameraPosition();

	private:
		SDL_Window* m_pWindow{};
		SoftwareRenderer* m_pSoftwareRenderer;
		DirectXRenderer* m_pDirectXRenderer;

		static Camera m_Camera;
		RenderMode m_pRenderMode{ RenderMode::Hardware };

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

	};
}
