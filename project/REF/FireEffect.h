#pragma once
#include "src/Scene/HeaderFiles/DirectXTexture.h"
#include "src/Scene/HeaderFiles/Effect.h"
#include "src/Math/HeaderFiles/Math.h"

class FireEffect final : public Effect
{
public:
	FireEffect(ID3D11Device* pDevice, const std::wstring& path);
	virtual ~FireEffect() override;

	virtual void SetDiffuseMap(const DirectXTexture& pDiffuseTexture) override;

	virtual void SetMapVariables(const DirectXTexture& pDiffuseMap, const DirectXTexture& pNormalMap, const DirectXTexture& pSpecularMap, const DirectXTexture& pGlossinesMap)override {};
	virtual void SetFloat3Variables(const dae::Vector3& cameraPos, const dae::Vector3& lightDir) override {};
	virtual void SetIsUsingNorMapsVariable(const bool isUsingNormalMap) override {};


private:
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
};
