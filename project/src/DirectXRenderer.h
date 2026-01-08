#pragma once

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"
#include "SDL_image.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Framework Headers
#include "Timer.h"
#include "Camera.h"
#include "Visuals/Effect.h"

class D_Texture;

namespace dae
{
	class D_Mesh;

	class D_Renderer final
	{
	public:
		D_Renderer(SDL_Window* pWindow, Camera* pSharedCamera, float* pMeshRotation);
		~D_Renderer();

		D_Renderer(const D_Renderer&) = delete;
		D_Renderer(D_Renderer&&) noexcept = delete;
		D_Renderer& operator=(const D_Renderer&) = delete;
		D_Renderer& operator=(D_Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void SetSamplerFilter(SamplerFilter filter);

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		Camera* m_pCamera{};
		float* m_pMeshRotation{};
		mutable SamplerFilter m_CurrentSamplerFilter{ SamplerFilter::Point };

		//DIRECTX
		HRESULT InitializeDirectX();
		IDXGIFactory1* m_pDXGIFactory{};
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		// Vehicle mesh and textures
		D_Mesh* m_pMesh{};
		D_Texture* m_pDiffuseTexture{};
		D_Texture* m_pNormalTexture{};
		D_Texture* m_pSpecularTexture{};
		D_Texture* m_pGlossinessTexture{};

		// Fire FX mesh and texture
		D_Mesh* m_pFireMesh{};
		D_Texture* m_pFireDiffuseTexture{};

		// Shared effect
		Effect* m_pSharedEffect{};
	};
}
