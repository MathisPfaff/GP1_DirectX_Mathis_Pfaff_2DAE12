#pragma once

#include <iostream>
#include <sstream>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Matrix.h"

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
	virtual ~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique(SamplerFilter filter = SamplerFilter::Point) const;
	ID3DX11EffectMatrixVariable* GetWorldViewProjVariable() const;
	ID3DX11EffectMatrixVariable* GetWorldMatrixVariable() const;
	ID3DX11EffectVectorVariable* GetCameraPositionVariable() const;

	virtual void SetDiffuseMap(ID3D11ShaderResourceView* pDiffuseTexture);
	virtual void SetNormalMap(ID3D11ShaderResourceView* pNormalTexture);
	virtual void SetSpecularMap(ID3D11ShaderResourceView* pSpecularTexture);
	virtual void SetGlossinessMap(ID3D11ShaderResourceView* pGlossinessTexture);

protected:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechniquePoint;
	ID3DX11EffectTechnique* m_pTechniqueLinear;
	ID3DX11EffectTechnique* m_pTechniqueAnisotropic;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable;
	ID3DX11EffectVectorVariable* m_pCameraPositionVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
};