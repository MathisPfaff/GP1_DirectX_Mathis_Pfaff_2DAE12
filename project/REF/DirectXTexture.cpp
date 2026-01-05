#include "src/pch.h"
#include "src/Scene/HeaderFiles/DirectXTexture.h"

DirectXTexture::DirectXTexture(const std::string& path, ID3D11Device* pDevice)
{
	SDL_Surface* pSurface = IMG_Load(path.c_str());
	if (pSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		return;
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

	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
	if (FAILED(hr))
	{
		std::cout << "loading imgResource failed\n";
		SDL_FreeSurface(pSurface);
		pSurface = nullptr;
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);
	if (FAILED(hr))
	{
		std::cout << "loading imgResourceView failed\n";
		SDL_FreeSurface(pSurface);
		pSurface = nullptr;
		return;
	}

	SDL_FreeSurface(pSurface);
	pSurface = nullptr;
}

DirectXTexture::~DirectXTexture()
{
	m_pSRV->Release();
	m_pResource->Release();
}

dae::ColorRGB DirectXTexture::Sample(const dae::Vector2& uv)
{
	return dae::ColorRGB{};
}
