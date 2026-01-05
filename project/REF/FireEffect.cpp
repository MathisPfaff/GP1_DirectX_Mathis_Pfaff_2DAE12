#include "src/pch.h"
#include "src/Scene/HeaderFiles/FireEffect.h"

FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& path)
	:Effect(pDevice, path)
{
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"Loading diffuseMap variable failed\n";
}

FireEffect::~FireEffect()
{
	m_pDiffuseMapVariable->Release();
	m_pMatWorldViewProjVariable->Release();
	m_pMatWorldVariable->Release();
	m_pTechniquePoint->Release();
	m_pTechniqueLinear->Release();
	m_pTechniqueAnisotropic->Release();
	m_pEffect->Release();
}

void FireEffect::SetDiffuseMap(const DirectXTexture& pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture.GetResourceView());
}
