#pragma once
#include "src/Math/HeaderFiles/Math.h"
#include <string>

//DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

class DirectXTexture
{
public:
	DirectXTexture(const std::string& path, ID3D11Device* pDevice);
	~DirectXTexture();

	dae::ColorRGB Sample(const dae::Vector2& uv);
	ID3D11Texture2D* GetResource()const { return m_pResource; }
	ID3D11ShaderResourceView* GetResourceView()const { return m_pSRV; }
private:
	ID3D11ShaderResourceView* m_pSRV{ nullptr };
	ID3D11Texture2D* m_pResource{ nullptr };
};