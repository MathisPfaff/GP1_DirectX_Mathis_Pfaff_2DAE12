#include "src/pch.h"
#include "src/Scene/HeaderFiles/VehicleEffect.h"

VehicleEffect::VehicleEffect(ID3D11Device* pDevice, const std::wstring& path)
	:Effect(pDevice, path)
{
	//FLOAT3
	//CameraPos
	m_pPosCameraVariable = m_pEffect->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pMatWorldVariable->IsValid()) std::wcout << L"Loading of cameraposition variable failed\n";
	//LightDirection
	m_pLightDirectionVariable = m_pEffect->GetVariableByName("gLightDir")->AsVector();
	if (!m_pMatWorldVariable->IsValid()) std::wcout << L"Loading of cameraposition variable failed\n";
	//MAPS
	//diffuseMap
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid()) std::wcout << L"Loading of diffuseMap variable failed\n";
	//normalMap
	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid()) std::wcout << L"Loading of normalMap variable failed\n";
	//SpecularMap
	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())	std::wcout << L"Loading of specularlMap variable failed\n";
	//GlossinesMap
	m_pGlossinesMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinesMapVariable->IsValid()) std::wcout << L"Loading of glossinesMap variable failed\n";
	//Bools
	//is using normal maps
	m_pIsUsingNormalMapVariable = m_pEffect->GetVariableByName("gUseNormalMap")->AsScalar();
	if (!m_pIsUsingNormalMapVariable->IsValid()) std::wcout << L"Loading of using normal maps bool variable failed\n";


}

VehicleEffect::~VehicleEffect()
{
	m_pIsUsingNormalMapVariable->Release();
	m_pGlossinesMapVariable->Release();
	m_pSpecularMapVariable->Release();
	m_pNormalMapVariable->Release();
	m_pDiffuseMapVariable->Release();
	m_pLightDirectionVariable->Release();
	m_pPosCameraVariable->Release();
	m_pMatWorldViewProjVariable->Release();
	m_pMatWorldVariable->Release();
	m_pTechniquePoint->Release();
	m_pTechniqueLinear->Release();
	m_pTechniqueAnisotropic->Release();
	m_pEffect->Release();
}

void VehicleEffect::SetMapVariables(const DirectXTexture& pDiffuseMap, const DirectXTexture& pNormalMap, const DirectXTexture& pSpecularMap, const DirectXTexture& pGlossinesMap)
{
	if (m_pDiffuseMapVariable) m_pDiffuseMapVariable->SetResource(pDiffuseMap.GetResourceView());

	if (m_pNormalMapVariable) m_pNormalMapVariable->SetResource(pNormalMap.GetResourceView());
	else std::cout << "No normal map\n";

	if (m_pSpecularMapVariable) m_pSpecularMapVariable->SetResource(pSpecularMap.GetResourceView());

	if (m_pGlossinesMapVariable) m_pGlossinesMapVariable->SetResource(pGlossinesMap.GetResourceView());
}

void VehicleEffect::SetFloat3Variables(const dae::Vector3& cameraPos, const dae::Vector3& lightDir)
{
	if (m_pPosCameraVariable) m_pPosCameraVariable->SetFloatVector(reinterpret_cast<const float*>(&cameraPos));

	if (m_pLightDirectionVariable) m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<const float*>(&lightDir));
}

void VehicleEffect::SetIsUsingNorMapsVariable(const bool isUsingNormalMap)
{
	if (m_pIsUsingNormalMapVariable) m_pIsUsingNormalMapVariable->SetBool(isUsingNormalMap);
	else std::cout << "Bool fails normal map\n";
}