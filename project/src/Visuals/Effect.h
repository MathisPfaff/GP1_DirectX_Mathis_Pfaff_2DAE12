#pragma once

#include <iostream>
#include <sstream>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Math/Matrix.h"
#include "DataTypes.h"

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
	ID3DX11EffectTechnique* GetTechnique(dae::SamplerFilter filter = dae::SamplerFilter::Point, dae::CullMode cullMode = dae::CullMode::BackFace) const;
	ID3DX11EffectMatrixVariable* GetWorldViewProjVariable() const;
	ID3DX11EffectMatrixVariable* GetWorldMatrixVariable() const;
	ID3DX11EffectVectorVariable* GetCameraPositionVariable() const;

	virtual void SetDiffuseMap(ID3D11ShaderResourceView* pDiffuseTexture);
	virtual void SetNormalMap(ID3D11ShaderResourceView* pNormalTexture);
	virtual void SetSpecularMap(ID3D11ShaderResourceView* pSpecularTexture);
	virtual void SetGlossinessMap(ID3D11ShaderResourceView* pGlossinessTexture);

protected:
	ID3DX11Effect* m_pEffect;

	// Vehicle techniques with different cull modes
	ID3DX11EffectTechnique* m_pTechniquePointBackCull;
	ID3DX11EffectTechnique* m_pTechniquePointFrontCull;
	ID3DX11EffectTechnique* m_pTechniquePointNoCull;

	ID3DX11EffectTechnique* m_pTechniqueLinearBackCull;
	ID3DX11EffectTechnique* m_pTechniqueLinearFrontCull;
	ID3DX11EffectTechnique* m_pTechniqueLinearNoCull;

	ID3DX11EffectTechnique* m_pTechniqueAnisotropicBackCull;
	ID3DX11EffectTechnique* m_pTechniqueAnisotropicFrontCull;
	ID3DX11EffectTechnique* m_pTechniqueAnisotropicNoCull;

	// Matrix and resource variables
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable;
	ID3DX11EffectVectorVariable* m_pCameraPositionVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
};