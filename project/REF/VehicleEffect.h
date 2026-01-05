#pragma once
#include "src/Scene/HeaderFiles/DirectXTexture.h"
#include "src/Scene/HeaderFiles/Effect.h"

class VehicleEffect final : public Effect
{
public:
	VehicleEffect(ID3D11Device* pDevice, const std::wstring& path);
	virtual ~VehicleEffect() override;

	virtual void SetMapVariables(const DirectXTexture& pDiffuseMap, const DirectXTexture& pNormalMap, const DirectXTexture& pSpecularMap, const DirectXTexture& pGlossinesMap)override;
	virtual void SetFloat3Variables(const dae::Vector3& cameraPos, const dae::Vector3& lightDir)override;
	virtual void SetIsUsingNorMapsVariable(const bool isUsingNormalMap)override;

	virtual void SetDiffuseMap(const DirectXTexture& pDiffuseMap) override {};
private:

	ID3DX11EffectVectorVariable* m_pPosCameraVariable{ nullptr };
	ID3DX11EffectVectorVariable* m_pLightDirectionVariable{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinesMapVariable{ nullptr };

	ID3DX11EffectScalarVariable* m_pIsUsingNormalMapVariable{ nullptr };


};
