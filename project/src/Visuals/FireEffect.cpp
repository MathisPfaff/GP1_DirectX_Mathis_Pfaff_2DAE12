#include "FireEffect.h"

FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: Effect(pDevice, assetFile)
{
	// Get the diffuse map variable from the parent effect
	ID3DX11Effect* pEffect = GetEffect();
	if (pEffect != nullptr)
	{
		m_pDiffuseMapVariable = pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"FireEffect: Diffuse map variable not valid!\n";
		}
	}
}

void FireEffect::SetDiffuseMap(ID3D11ShaderResourceView* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable && m_pDiffuseMapVariable->IsValid())
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture);
	}
}