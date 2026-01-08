#include "DirectXTexture.h"
#include <SDL_image.h>

D_Texture::D_Texture(ID3D11Device* pDevice, const std::string& filePath)
	: m_pShaderResourceView(nullptr), m_pResource(nullptr)
{
	if (!LoadTextureFromFile(pDevice, filePath))
	{
		std::cerr << "Failed to load texture: " << filePath << std::endl;
	}
}

D_Texture::~D_Texture()
{
	if (m_pShaderResourceView)
	{
		m_pShaderResourceView->Release();
		m_pShaderResourceView = nullptr;
	}

	if (m_pResource)
	{
		m_pResource->Release();
		m_pResource = nullptr;
	}
}

ID3D11ShaderResourceView* D_Texture::GetShaderResourceView() const
{
	return m_pShaderResourceView;
}

bool D_Texture::LoadTextureFromFile(ID3D11Device* pDevice, const std::string& filePath)
{
	SDL_Surface* pSurface = IMG_Load(filePath.c_str());
	if (!pSurface)
	{
		std::cerr << "Failed to load image file: " << filePath << std::endl;
		return false;
	}

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = pSurface->w;
	desc.Height = pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

	HRESULT result = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);

	if (FAILED(result))
	{
		std::cerr << "Failed to create texture resource" << std::endl;
		SDL_FreeSurface(pSurface);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pShaderResourceView);

	if (FAILED(result))
	{
		std::cerr << "Failed to create shader resource view" << std::endl;
		SDL_FreeSurface(pSurface);
		return false;
	}

	SDL_FreeSurface(pSurface);
	pSurface = nullptr;

	return true;
}