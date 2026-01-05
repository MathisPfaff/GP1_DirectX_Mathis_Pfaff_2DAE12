#include "src/pch.h"
#include "src/Renderer/HeaderFiles/DirectXRenderer.h"
#include "src/Scene/HeaderFiles/DirectXTexture.h"
#include "src/Utils.h"
#include "src/Scene/HeaderFiles/Effects.h"
#include "src/Scene/HeaderFiles/DirectMesh.h"

//DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

namespace dae {

	DirectXRenderer::DirectXRenderer(SDL_Window* pWindow, const Camera& camera) :
		m_pWindow(pWindow),
		m_Camera(camera)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";	

			//load diffuseMap
			m_pDiffuseMap = new DirectXTexture("Resources/vehicle_diffuse.png", m_pDevice);
			m_pNormalMap = new DirectXTexture("Resources/vehicle_normal.png", m_pDevice);
			m_pSpecularMap = new DirectXTexture("Resources/vehicle_specular.png", m_pDevice);
			m_pGlossinesMap = new DirectXTexture("Resources/vehicle_gloss.png", m_pDevice);

			m_pDiffuseMapFire = new DirectXTexture("Resources/fireFX_diffuse.png", m_pDevice);

			//Effects
			m_pFireEffect = new FireEffect(m_pDevice, L"Resources/Fire.fx");
			m_pVehicleEffect = new VehicleEffect(m_pDevice, L"Resources/PosCol3D.fx");

			m_BackgroundColor = m_LBlueBackground;

		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}
	}

	DirectXRenderer::~DirectXRenderer()
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapchain->Release();
		if (m_pRasterizerStateNone)  m_pRasterizerStateNone->Release();
		if (m_pRasterizerStateFront) m_pRasterizerStateFront->Release();
		if (m_pRasterizerStateBack)  m_pRasterizerStateBack->Release();
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		m_pDevice->Release();

		delete m_Mesh;
		delete m_MeshFire;
		delete m_pDiffuseMap;
		delete m_pGlossinesMap;
		delete m_pSpecularMap;
		delete m_pNormalMap;
		delete m_pDiffuseMapFire;
		delete m_pVehicleEffect;
		delete m_pFireEffect;

		m_Mesh = nullptr;
		m_MeshFire = nullptr;
		m_pDiffuseMap = nullptr;
		m_pGlossinesMap = nullptr;
		m_pSpecularMap = nullptr;
		m_pNormalMap = nullptr;
		m_pDiffuseMapFire = nullptr;
		m_pVehicleEffect = nullptr;
		m_pFireEffect = nullptr;
		m_pRasterizerStateNone = nullptr;
		m_pRasterizerStateFront = nullptr;
		m_pRasterizerStateBack = nullptr;
	}

	void DirectXRenderer::Update(const Timer* pTimer)
	{
		float dt = pTimer->GetElapsed();

		m_Camera.Update(pTimer);
		const Matrix viewMatrix = m_Camera.GetViewMatrix();
		const Matrix projectionMatrix = m_Camera.GetProjectionMatrix();
		const Matrix vpMatrix = viewMatrix * projectionMatrix;
		m_Mesh->Update(vpMatrix, dt);
		m_MeshFire->Update(vpMatrix, dt);
	}

	void DirectXRenderer::Render() const
	{
		//0. Set Cullmode back to current mode before drawing
		//---------------------------------
		switch (m_CullMode)
		{
		case CullMode::None:
			m_pDeviceContext->RSSetState(m_pRasterizerStateNone);
			break;
		case CullMode::Front:
			m_pDeviceContext->RSSetState(m_pRasterizerStateFront);
			break;
		case CullMode::Back:
		default:
			m_pDeviceContext->RSSetState(m_pRasterizerStateBack);
			break;
		};
		//1. Clear RTV & DSV
		//---------------------------------
		float color[4] = { m_BackgroundColor.r, m_BackgroundColor.g, m_BackgroundColor.b, 1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//2. Set pipline + invoke Draw calls (= Render)
		//---------------------------------
		m_Mesh->Render(m_pDeviceContext, m_CurrSampleMode);

		if (m_EnableFlames)
		{
			//Keeps the fire mesh on cullmode mone
			m_pDeviceContext->RSSetState(m_pRasterizerStateNone);
			m_MeshFire->Render(m_pDeviceContext, m_CurrSampleMode);
		}

		//3. Present BackBuffer (Swap)
		//---------------------------------
		m_pSwapchain->Present(0, 0);

		if (!m_IsInitialized)
			return;

	}

	void DirectXRenderer::ToggleSampleMode()
	{
		m_CurrSampleMode = static_cast<SampleMode>((int(m_CurrSampleMode) + 1) % m_NrSampleModes);
		std::cout << "Current DirectXTexture sampling state: ";
		switch (int(m_CurrSampleMode))
		{
		case 0:
			std::cout << "Point\n";
			break;
		case 1:
			std::cout << "Linear\n";
			break;
		case 2:
			std::cout << "Anisotropic\n";
			break;
		default:
			std::cout << "This should not happen\n";
			break;
		}
	}

	void DirectXRenderer::ToggleRotationMode()
	{
		m_ShouldRotate = !m_ShouldRotate;
		m_Mesh->ToggleRotation();
		m_MeshFire->ToggleRotation();
		if (m_ShouldRotate)
		{
			std::cout << "Rotating is enabled\n";
		}
		else std::cout << "Rotating is disabled\n";
	}

	void DirectXRenderer::ToggleUseNormalMapMode()
	{
		m_EnableNmaps = !m_EnableNmaps;
		m_Mesh->SetUsingNormalMaps(m_EnableNmaps);
		if (m_EnableNmaps)
		{
			std::cout << " Normal mapping is enabled\n";
		}
		else std::cout << " Normal mapping is disabled\n";
	}

	void DirectXRenderer::ToggleUseFireMeshMode()
	{
		m_EnableFlames = !m_EnableFlames;
		if (m_EnableFlames)
		{
			std::cout << "FireFX mesh is enabled\n";
		}
		else std::cout << "FireFX mesh is disabled\n";
	}

	HRESULT DirectXRenderer::InitializeDirectX()
	{
		//1. Create Device and DeviceContext
		//---------------------------------
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			createDeviceFlags,
			&featureLevel,
			1,
			D3D11_SDK_VERSION,
			&m_pDevice,
			nullptr,
			&m_pDeviceContext
		);
		if (FAILED(result))
			return result;

		//Create DXGI Factory
		//---------------------------------
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		//2. Create Swapchain
		//---------------------------------
		DXGI_SWAP_CHAIN_DESC swapchainDesc{};
		swapchainDesc.BufferDesc.Width = m_Width;
		swapchainDesc.BufferDesc.Height = m_Height;
		swapchainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapchainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = 1;
		swapchainDesc.Windowed = true;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapchainDesc.Flags = 0;

		//Get Handle (HWND) from SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapchainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapchainDesc, &m_pSwapchain);
		pDxgiFactory->Release();

		if (FAILED(result))
			return result;

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		//---------------------------------
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		//---------------------------------
		//Resource
		result = m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		//5. Create RTV & DSV to Output Merger stage
		//---------------------------------
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//6. Set viewport
		//---------------------------------
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);
		if (FAILED(result))
			return result;

		//7. Create CullMode states
		//---------------------------------
		D3D11_RASTERIZER_DESC rastDesc{};
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.FrontCounterClockwise = false;    // CCW is front
		rastDesc.DepthClipEnable = true;    // good practice for typical 3D scenes

		// 1) Cull = NONE
		rastDesc.CullMode = D3D11_CULL_NONE;
		HRESULT hr = m_pDevice->CreateRasterizerState(&rastDesc, &m_pRasterizerStateNone);
		if (FAILED(hr)) return hr;

		// 2) Cull = FRONT
		rastDesc.CullMode = D3D11_CULL_FRONT;
		hr = m_pDevice->CreateRasterizerState(&rastDesc, &m_pRasterizerStateFront);
		if (FAILED(hr)) return hr;

		// 3) Cull = BACK
		rastDesc.CullMode = D3D11_CULL_BACK;
		hr = m_pDevice->CreateRasterizerState(&rastDesc, &m_pRasterizerStateBack);
		if (FAILED(hr)) return hr;
	}

	dae::Vector3 DirectXRenderer::GetLightNormalisedDirection()
	{
		return  m_LightDirection.Normalized();
	}

	void DirectXRenderer::SwitchBackgroundColor()
	{
		if (m_BackgroundColor == m_LBlueBackground) m_BackgroundColor = m_UniformBackground;
		else m_BackgroundColor = m_LBlueBackground;
	}

	void DirectXRenderer::UpdateCamera(const Camera& camera)
	{
		m_Camera = camera;
	}

	void DirectXRenderer::ToggleCullMode()
	{
		switch (m_CullMode)
		{
		case CullMode::None:
			m_CullMode = CullMode::Front;
			m_pDeviceContext->RSSetState(m_pRasterizerStateFront);
			break;
		case CullMode::Front:
			m_CullMode = CullMode::Back;
			m_pDeviceContext->RSSetState(m_pRasterizerStateBack);
			break;
		case CullMode::Back:
		default:
			m_CullMode = CullMode::None;
			m_pDeviceContext->RSSetState(m_pRasterizerStateNone);
			break;
		}

		// Apply immediately
		SetCullMode(m_CullMode);
	}

	void DirectXRenderer::SetCullMode(const CullMode& cullMode)
	{
		m_CullMode = cullMode;

		switch (m_CullMode)
		{
		case CullMode::None:
			m_pDeviceContext->RSSetState(m_pRasterizerStateNone);
			break;
		case CullMode::Front:
			m_pDeviceContext->RSSetState(m_pRasterizerStateFront);
			break;
		case CullMode::Back:
		default:
			m_pDeviceContext->RSSetState(m_pRasterizerStateBack);
			break;
		}
	}

	void DirectXRenderer::ConstructMesh(const std::vector<Vertex>& vertices, const std::vector<Vertex>& fireVertices, const std::vector<uint32_t>& vehicleIndices, const std::vector<uint32_t>& fireIndices)
	{
		m_VehicleVerticesIn.clear();
		m_VehicleVerticesIn.reserve(vertices.size());

		for (const auto& vertex : vertices)
		{
			Vertex_In vertexIn;
			vertexIn.position = vertex.position;
			vertexIn.uv = vertex.uv;
			vertexIn.normal = vertex.normal;
			vertexIn.tangent = vertex.tangent;

			m_VehicleVerticesIn.push_back(vertexIn);
		}

		m_FireVerticesIn.clear();
		m_FireVerticesIn.reserve(fireVertices.size());
		for (const auto& vertex : fireVertices)
		{
			Vertex_In vertexIn;
			vertexIn.position = vertex.position;
			vertexIn.uv = vertex.uv;
			vertexIn.normal = vertex.normal;
			vertexIn.tangent = vertex.tangent;

			m_FireVerticesIn.push_back(vertexIn);
		}

		m_FireIndices = fireIndices;
		m_VehicleIndices = vehicleIndices;

		m_Mesh = new DirectMesh(m_pDevice, m_VehicleVerticesIn, m_VehicleIndices, m_pVehicleEffect);
		m_Mesh->SetShaderResourceMaps(*m_pDiffuseMap, *m_pNormalMap, *m_pSpecularMap, *m_pGlossinesMap);
		m_Mesh->SetFloat3Shader(m_Camera.GetOrigin(), GetLightNormalisedDirection());
		m_Mesh->SetUsingNormalMaps(m_EnableNmaps);

		m_MeshFire = new DirectMesh(m_pDevice, m_FireVerticesIn, m_FireIndices, m_pFireEffect);
		m_MeshFire->SetShaderResourceMap(*m_pDiffuseMapFire);
		m_MeshFire->SetFloat3Shader(m_Camera.GetOrigin(), GetLightNormalisedDirection());
	}
}


