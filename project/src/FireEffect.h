#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Effect.h"
#include "Matrix.h"

class FireEffect final : public Effect
{
public:
	FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~FireEffect() override = default;

	FireEffect(const FireEffect&) = delete;
	FireEffect(FireEffect&&) noexcept = delete;
	FireEffect& operator=(const FireEffect&) = delete;
	FireEffect& operator=(FireEffect&&) noexcept = delete;

	// Override to set only diffuse map for fire
	virtual void SetDiffuseMap(ID3D11ShaderResourceView* pDiffuseTexture) override;

	// Fire effect doesn't use these, so we override them as no-ops
	virtual void SetNormalMap(ID3D11ShaderResourceView* pNormalTexture) override {}
	virtual void SetSpecularMap(ID3D11ShaderResourceView* pSpecularTexture) override {}
	virtual void SetGlossinessMap(ID3D11ShaderResourceView* pGlossinessTexture) override {}

private:
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
};