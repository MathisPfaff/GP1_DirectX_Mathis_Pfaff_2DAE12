#pragma once

#include <iostream>

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// SDL Headers
#include "SDL_surface.h"

class D_Texture
{
public:
	D_Texture(ID3D11Device* pDevice, const std::string& filePath);
	~D_Texture();

	D_Texture(const D_Texture&) = delete;
	D_Texture(D_Texture&&) noexcept = delete;
	D_Texture& operator=(const D_Texture&) = delete;
	D_Texture& operator=(D_Texture&&) noexcept = delete;

	ID3D11ShaderResourceView* GetShaderResourceView() const;

private:
	ID3D11ShaderResourceView* m_pShaderResourceView;
	ID3D11Texture2D* m_pResource;

	bool LoadTextureFromFile(ID3D11Device* pDevice, const std::string& filePath);
};