#include "Effect.h"

static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect{ nullptr };

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i{ 0 }; i < pErrorBlob->GetBufferSize(); i++)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) :
	m_pMatWorldViewProjVariable{ nullptr },
	m_pMatWorldVariable{ nullptr },
	m_pCameraPositionVariable{ nullptr },
	m_pDiffuseMapVariable{ nullptr },
	m_pNormalMapVariable{ nullptr },
	m_pSpecularMapVariable{ nullptr },
	m_pGlossinessMapVariable{ nullptr },
	m_pTechniquePoint{ nullptr },
	m_pTechniqueLinear{ nullptr },
	m_pTechniqueAnisotropic{ nullptr }
{
	m_pEffect = LoadEffect(pDevice, assetFile);

	if (m_pEffect != nullptr)
	{
		m_pTechniquePoint = m_pEffect->GetTechniqueByName("PointTechnique");
		if (!m_pTechniquePoint->IsValid())
		{
			std::wcout << L"Effect: PointTechnique not valid!\n";
		}

		m_pTechniqueLinear = m_pEffect->GetTechniqueByName("LinearTechnique");
		if (!m_pTechniqueLinear->IsValid())
		{
			std::wcout << L"Effect: LinearTechnique not valid!\n";
		}

		m_pTechniqueAnisotropic = m_pEffect->GetTechniqueByName("AnisotropicTechnique");
		if (!m_pTechniqueAnisotropic->IsValid())
		{
			std::wcout << L"Effect: AnisotropicTechnique not valid!\n";
		}

		m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pMatWorldViewProjVariable->IsValid())
		{
			std::wcout << L"Effect: WorldViewProj matrix variable not valid!\n";
		}

		m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
		if (!m_pMatWorldVariable->IsValid())
		{
			std::wcout << L"Effect: World matrix variable not valid!\n";
		}

		m_pCameraPositionVariable = m_pEffect->GetVariableByName("gCameraPosition")->AsVector();
		if (!m_pCameraPositionVariable->IsValid())
		{
			std::wcout << L"Effect: Camera position variable not valid!\n";
		}

		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"Effect: Diffuse map variable not valid!\n";
		}

		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid())
		{
			std::wcout << L"Effect: Normal map variable not valid!\n";
		}

		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid())
		{
			std::wcout << L"Effect: Specular map variable not valid!\n";
		}

		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
		{
			std::wcout << L"Effect: Glossiness map variable not valid!\n";
		}
	}
	else
	{
		std::wcout << L"Effect: Failed to load effect file!\n";
	}
}

Effect::~Effect()
{
	if (m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = nullptr;
	}
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique(SamplerFilter filter) const
{
	switch (filter)
	{
	case SamplerFilter::Point:
		return m_pTechniquePoint;
	case SamplerFilter::Linear:
		return m_pTechniqueLinear;
	case SamplerFilter::Anisotropic:
		return m_pTechniqueAnisotropic;
	default:
		return m_pTechniquePoint;
	}
}

ID3DX11EffectMatrixVariable* Effect::GetWorldViewProjVariable() const
{
	return m_pMatWorldViewProjVariable;
}

ID3DX11EffectMatrixVariable* Effect::GetWorldMatrixVariable() const
{
	return m_pMatWorldVariable;
}

ID3DX11EffectVectorVariable* Effect::GetCameraPositionVariable() const
{
	return m_pCameraPositionVariable;
}

void Effect::SetDiffuseMap(ID3D11ShaderResourceView* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture);
	}
}

void Effect::SetNormalMap(ID3D11ShaderResourceView* pNormalTexture)
{
	if (m_pNormalMapVariable)
	{
		m_pNormalMapVariable->SetResource(pNormalTexture);
	}
}

void Effect::SetSpecularMap(ID3D11ShaderResourceView* pSpecularTexture)
{
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->SetResource(pSpecularTexture);
	}
}

void Effect::SetGlossinessMap(ID3D11ShaderResourceView* pGlossinessTexture)
{
	if (m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->SetResource(pGlossinessTexture);
	}
}