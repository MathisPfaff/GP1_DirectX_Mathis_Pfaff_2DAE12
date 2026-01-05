#pragma once
#include "src/Scene/HeaderFiles/DirectMesh.h"
#include "src/Scene/HeaderFiles/Camera.h"
#include "src/Scene/HeaderFiles/Effects.h"
#include "src/Utils.h"
#include <memory>

//DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

struct SDL_Window;
struct SDL_Surface;
class DirectMesh;

namespace dae
{
	enum class CullMode
	{
		None,
		Front,
		Back
	};

	class DirectXRenderer final
	{
	public:
		DirectXRenderer(SDL_Window* pWindow, const Camera& camera);
		~DirectXRenderer();

		DirectXRenderer(const DirectXRenderer&) = delete;
		DirectXRenderer(DirectXRenderer&&) noexcept = delete;
		DirectXRenderer& operator=(const DirectXRenderer&) = delete;
		DirectXRenderer& operator=(DirectXRenderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

		void ToggleSampleMode();
		void ToggleRotationMode();
		void ToggleUseNormalMapMode();
		void ToggleUseFireMeshMode();

		void SwitchBackgroundColor();
		void UpdateCamera(const Camera& camera);

		dae::Vector3 GetLightNormalisedDirection();
		void SetCullMode(const CullMode& cullMode);
		void ToggleCullMode();
		void ConstructMesh(const std::vector<Vertex>& vertices, const std::vector<Vertex>& fireVertices, const std::vector<uint32_t>& vehicleIndices, const std::vector<uint32_t>& fireIndices);
	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		HRESULT InitializeDirectX();
		ID3D11DeviceContext* m_pDeviceContext;
		ID3D11Device* m_pDevice;
		IDXGISwapChain* m_pSwapchain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		DirectMesh* m_Mesh{ nullptr };
		DirectMesh* m_MeshFire{ nullptr };

		VehicleEffect* m_pVehicleEffect{ nullptr };
		FireEffect* m_pFireEffect{ nullptr };

		Camera m_Camera{};
		dae::Vector3 m_LightDirection{ 0.577f, -0.577f, 0.577f };
		DirectXTexture* m_pDiffuseMap{ nullptr };
		DirectXTexture* m_pNormalMap{ nullptr };
		DirectXTexture* m_pSpecularMap{ nullptr };
		DirectXTexture* m_pGlossinesMap{ nullptr };
		DirectXTexture* m_pDiffuseMapFire{ nullptr };
		ID3D11RasterizerState* m_pRasterizerStateNone{ nullptr };
		ID3D11RasterizerState* m_pRasterizerStateFront{ nullptr };
		ID3D11RasterizerState* m_pRasterizerStateBack{ nullptr };

		SampleMode m_CurrSampleMode{ SampleMode::Point };
		const int m_NrSampleModes{ 3 };
		bool m_ShouldRotate{ true };
		bool m_EnableNmaps{ true };
		bool m_EnableFlames{ true };

		ColorRGB m_BackgroundColor{};
		ColorRGB m_LBlueBackground{ .39f, .59f, .93f };
		ColorRGB m_UniformBackground{ .1f, .1f, .1f };

		CullMode m_CullMode{ CullMode::Back };

		std::vector<Vertex_In> m_VehicleVerticesIn{};
		std::vector<uint32_t> m_VehicleIndices{};

		std::vector<Vertex_In> m_FireVerticesIn{};
		std::vector<uint32_t> m_FireIndices{};
	};
}
