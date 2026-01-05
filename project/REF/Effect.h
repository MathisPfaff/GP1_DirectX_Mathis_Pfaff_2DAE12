#pragma once
#include "DirectXTexture.h"
#include "src/Math/HeaderFiles/Math.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& path);
	virtual ~Effect() = default;

	ID3DX11Effect* GetEffect() { return m_pEffect; };
	ID3DX11EffectTechnique* GetTechniquePoint() { return m_pTechniquePoint; };
	ID3DX11EffectTechnique* GetTechniqueLinear() { return m_pTechniqueLinear; };
	ID3DX11EffectTechnique* GetTechniqueAnisotropic() { return m_pTechniqueAnisotropic; };

	void SetViewMatrixVariables(const dae::Matrix& viewDirMatrix, const dae::Matrix& worldMatrix);
	//Pure virtual methodes
	virtual void SetMapVariables(const DirectXTexture& pDiffuseMap, const DirectXTexture& pNormalMap, const DirectXTexture& pSpecularMap, const DirectXTexture& pGlossinesMap) = 0;
	virtual void SetFloat3Variables(const dae::Vector3& cameraPos, const dae::Vector3& lightDir) = 0;
	virtual void SetIsUsingNorMapsVariable(const bool isUsingNormalMap) = 0;
	virtual void SetDiffuseMap(const DirectXTexture& pDiffuseTexture) = 0;
protected:
	ID3DX11Effect* m_pEffect{};
	//SampleTechinque
	ID3DX11EffectTechnique* m_pTechniquePoint{ nullptr };
	ID3DX11EffectTechnique* m_pTechniqueLinear{ nullptr };
	ID3DX11EffectTechnique* m_pTechniqueAnisotropic{ nullptr };
	//Matrices
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable{ nullptr };

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& path);

};
