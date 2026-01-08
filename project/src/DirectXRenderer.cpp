//External includes
#include "SDL.h"
#include "SDL_surface.h"

// Standard includes
#include <iostream>
#include <cassert>

//Project includes
#include "DirectXRenderer.h"
#include "Visuals/Mesh.h"
#include "Visuals/DirectXTexture.h"
#include "Math/Utils.h"

using namespace dae;

D_Renderer::D_Renderer(SDL_Window* pWindow, Camera* pSharedCamera, float* pMeshRotation) :
	m_pWindow(pWindow),
	m_pCamera(pSharedCamera),
	m_pMeshRotation(pMeshRotation)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Initialize DirectX pipeline
	const HRESULT result = InitializeDirectX();
	if (result == S_OK)
	{
		m_IsInitialized = true;
		std::cout << "DirectX is initialized and ready!\n";
	}
	else
	{
		std::cout << "DirectX initialization failed!\n";
		return;
	}

	// Load vehicle textures
	m_pDiffuseTexture = new D_Texture(m_pDevice, "resources/vehicle_diffuse.png");
	m_pNormalTexture = new D_Texture(m_pDevice, "resources/vehicle_normal.png");
	m_pSpecularTexture = new D_Texture(m_pDevice, "resources/vehicle_specular.png");
	m_pGlossinessTexture = new D_Texture(m_pDevice, "resources/vehicle_gloss.png");

	// Validate vehicle textures
	if (!m_pDiffuseTexture->GetShaderResourceView())
	{
		std::cerr << "Failed to load vehicle diffuse texture!\n";
	}

	std::cout << "Vehicle textures loaded:\n";
	std::cout << "  Diffuse SRV: " << (m_pDiffuseTexture->GetShaderResourceView() != nullptr ? "Valid" : "NULL") << "\n";
	std::cout << "  Normal SRV: " << (m_pNormalTexture->GetShaderResourceView() != nullptr ? "Valid" : "NULL") << "\n";
	std::cout << "  Specular SRV: " << (m_pSpecularTexture->GetShaderResourceView() != nullptr ? "Valid" : "NULL") << "\n";
	std::cout << "  Glossiness SRV: " << (m_pGlossinessTexture->GetShaderResourceView() != nullptr ? "Valid" : "NULL") << "\n";

	// Create shared effect ONCE before creating meshes
	m_pSharedEffect = new Effect(m_pDevice, L"resources/PosCol3D.fx");
	assert(m_pSharedEffect != nullptr && "Failed to create shared effect");

	// Load vehicle mesh
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};

	if (dae::Utils::ParseOBJ("resources/vehicle.obj", vertices, indices, true))
	{
		m_pMesh = new D_Mesh(m_pDevice, vertices, indices, m_pDiffuseTexture, m_pSharedEffect, false);
		std::cout << "Vehicle mesh loaded successfully\n";
	}
	else
	{
		std::cerr << "Failed to load vehicle mesh from OBJ file!\n";
		m_pMesh = nullptr;
	}

	// Load fire FX texture
	m_pFireDiffuseTexture = new D_Texture(m_pDevice, "resources/fireFX_diffuse.png");

	if (!m_pFireDiffuseTexture->GetShaderResourceView())
	{
		std::cerr << "Warning: Fire FX texture not found. Fire effect will not render.\n";
	}
	else
	{
		std::cout << "Fire FX texture loaded successfully\n";
	}

	// Load fire FX mesh
	vertices.clear();
	indices.clear();

	if (dae::Utils::ParseOBJ("resources/fireFX.obj", vertices, indices, true))
	{
		m_pFireMesh = new D_Mesh(m_pDevice, vertices, indices, m_pFireDiffuseTexture, m_pSharedEffect, true);
		std::cout << "Fire FX mesh loaded successfully\n";
	}
	else
	{
		std::cerr << "Warning: Fire FX mesh not found. Fire effect will not render.\n";
		m_pFireMesh = nullptr;
	}
}

D_Renderer::~D_Renderer()
{
	// Release resources in REVERSE order of creation

	// Meshes first (they reference shared effect)
	if (m_pFireMesh)
	{
		delete m_pFireMesh;
		m_pFireMesh = nullptr;
	}

	if (m_pMesh)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}

	// Then textures
	if (m_pFireDiffuseTexture)
	{
		delete m_pFireDiffuseTexture;
		m_pFireDiffuseTexture = nullptr;
	}

	if (m_pGlossinessTexture)
	{
		delete m_pGlossinessTexture;
		m_pGlossinessTexture = nullptr;
	}

	if (m_pSpecularTexture)
	{
		delete m_pSpecularTexture;
		m_pSpecularTexture = nullptr;
	}

	if (m_pNormalTexture)
	{
		delete m_pNormalTexture;
		m_pNormalTexture = nullptr;
	}

	if (m_pDiffuseTexture)
	{
		delete m_pDiffuseTexture;
		m_pDiffuseTexture = nullptr;
	}

	// Shared effect (used by meshes, so delete last)
	if (m_pSharedEffect)
	{
		delete m_pSharedEffect;
		m_pSharedEffect = nullptr;
	}

	// DirectX resources
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}

	if (m_pRenderTargetBuffer)
	{
		m_pRenderTargetBuffer->Release();
		m_pRenderTargetBuffer = nullptr;
	}

	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}

	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = nullptr;
	}

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}

	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
		m_pDeviceContext = nullptr;
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pDXGIFactory)
	{
		m_pDXGIFactory->Release();
		m_pDXGIFactory = nullptr;
	}
}

void D_Renderer::Update(const Timer* pTimer)
{

}

void D_Renderer::SetSamplerFilter(SamplerFilter filter)
{
	m_CurrentSamplerFilter = filter;

	switch (filter)
	{
	case SamplerFilter::Point:
		std::cout << "Switched to Point filtering\n";
		break;
	case SamplerFilter::Linear:
		std::cout << "Switched to Linear filtering\n";
		break;
	case SamplerFilter::Anisotropic:
		std::cout << "Switched to Anisotropic filtering\n";
		break;
	}
}

void D_Renderer::ToggleFireMesh()
{
	m_IsFireMeshEnabled = !m_IsFireMeshEnabled;
	std::cout << "Fire mesh -> " << (m_IsFireMeshEnabled ? "On" : "Off") << std::endl;
}

void D_Renderer::Render() const
{
	if (!m_IsInitialized || !m_pMesh)
		return;

	// 1. Clear RTV and DSV
	constexpr float color[4] = { 0.f, 0.f, 0.3f, 1.f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	// 2. Set Pipeline + Invoke Draw Calls
	// Pre-compute matrices to avoid redundant calculations
	Matrix worldMatrix = Matrix::CreateRotationY(*m_pMeshRotation) * Matrix::CreateTranslation(0.f, 0.f, 50.f);
	Matrix viewMatrix = m_pCamera->GetViewMatrix();
	Matrix projMatrix = m_pCamera->GetProjectionMatrix();
	Matrix viewProjMatrix = viewMatrix * projMatrix;
	Matrix worldViewProjMatrix = worldMatrix * viewProjMatrix;

	// IMPORTANT: Render vehicle FIRST (opaque)
	// This writes to the depth buffer so transparent fire is rendered correctly behind it
	m_pMesh->Render(m_pDeviceContext, worldViewProjMatrix, worldMatrix, m_pCamera->GetPosition(),
		m_pNormalTexture, m_pSpecularTexture, m_pGlossinessTexture, m_CurrentSamplerFilter);

	// IMPORTANT: Render fire SECOND (transparent)
	// Fire uses blend states and doesn't write to depth buffer
	// Render only if fire mesh is valid, has texture, and is enabled
	if (m_IsFireMeshEnabled && m_pFireMesh && m_pFireDiffuseTexture && m_pFireDiffuseTexture->GetShaderResourceView())
	{
		Matrix fireWorldViewProjMatrix = worldMatrix * viewProjMatrix;
		m_pFireMesh->Render(m_pDeviceContext, fireWorldViewProjMatrix, worldMatrix, m_pCamera->GetPosition(),
			nullptr, nullptr, nullptr, m_CurrentSamplerFilter);
	}

	// 3. Present backbuffer (swap)
	m_pSwapChain->Present(0, 0);
}

HRESULT D_Renderer::InitializeDirectX()
{
	//1. Create Device and Device Context
	//==============================
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
	uint32_t createDeviceFlags = 0;
#if defined(_DEBUG) || defined(DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create DXGI Factory
	//==============================
	HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&m_pDXGIFactory));

	if (FAILED(result))
	{
		return result;
	}

	// Create DXGI Adapter
	//==============================
	IDXGIAdapter* adapter = nullptr;
	IDXGIAdapter* selectedAdapter = nullptr;

	for (UINT i = 0; m_pDXGIFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		std::wcout << L"Adapter " << i << L": " << desc.Description << L"\n";

		if (desc.VendorId != 0x8086)
		{
			selectedAdapter = adapter;
			break;
		}
		else
		{
			adapter->Release();
		}
	}

	if (selectedAdapter == nullptr)
	{
		std::wcout << L"No suitable adapter found, defaulting to first adapter.\n";
		result = m_pDXGIFactory->EnumAdapters(0, &selectedAdapter);
		if (FAILED(result))
		{
			return result;
		}
	}

	result = D3D11CreateDevice(selectedAdapter, D3D_DRIVER_TYPE_UNKNOWN,
		0, createDeviceFlags, &featureLevel, 1,
		D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

	if (FAILED(result))
	{
		if (selectedAdapter)
		{
			selectedAdapter->Release();
		}
		return result;
	}

	// Release the selected adapter after device creation
	if (selectedAdapter)
	{
		selectedAdapter->Release();
		selectedAdapter = nullptr;
	}

	//2. Create Swap Chain
	//==============================
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// Get the handle (HWND) from the SDL buffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_GetVersion(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	// Create the swap chain
	if (m_pDevice != nullptr)
	{
		result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

		if (FAILED(result))
		{
			return result;
		}
	}
	else
	{
		return E_FAIL;
	}

	//3. Create DepthStencil (DS) and DepthStencilView (DSV)
	//==============================
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
	{
		return result;
	}

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);

	if (FAILED(result))
	{
		return result;
	}

	//4. Create RenderTarget (RT) and RenderTargetView (RTV)
	//==============================

	//Resource
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));

	if (FAILED(result))
	{
		return result;
	}

	//View
	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);

	if (FAILED(result))
	{
		return result;
	}

	//5. Bind RTV and DSV to Output Merger Stage
	//==============================
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	//6. Set Viewport
	//==============================
	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(m_Width);
	viewport.Height = static_cast<float>(m_Height);
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	return S_OK;
}