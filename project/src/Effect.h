#pragma once

#include <iostream>
#include <sstream>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

enum class SamplerFilter
{
	Point = 0,
	Linear = 1,
	Anisotropic = 2
};

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique(SamplerFilter filter = SamplerFilter::Point) const;
	ID3DX11EffectMatrixVariable* GetMatrixVariable() const;

	void SetDiffuseMap(ID3D11ShaderResourceView* pDiffuseTexture);

private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechniquePoint;
	ID3DX11EffectTechnique* m_pTechniqueLinear;
	ID3DX11EffectTechnique* m_pTechniqueAnisotropic;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
};