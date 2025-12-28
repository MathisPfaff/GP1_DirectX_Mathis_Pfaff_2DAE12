#pragma once

#include <iostream>

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// SDL Headers
#include "SDL_surface.h"

class Texture
{
public:
	Texture(ID3D11Device* pDevice, const std::string& filePath);
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&&) noexcept = delete;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) noexcept = delete;

	ID3D11ShaderResourceView* GetShaderResourceView() const;

private:
	ID3D11ShaderResourceView* m_pShaderResourceView;
	ID3D11Texture2D* m_pResource;

	bool LoadTextureFromFile(ID3D11Device* pDevice, const std::string& filePath);
};