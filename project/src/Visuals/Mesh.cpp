#include "Mesh.h"
#include <cassert>


Mesh::Mesh(ID3D11Device* pDevice, const std::vector<dae::Vertex_PosCol>& vertices,
           const std::vector<uint32_t>& indices, D_Texture* pTexture,
           Effect* pSharedEffect, bool isFireFX) :
    m_pDevice{ pDevice },
    m_pEffect{ pSharedEffect },
    m_pTexture{ pTexture },
    m_IsFireFX{ isFireFX },
    m_bOwnEffect{ false }
{
	assert(pDevice != nullptr && "Device cannot be null");
	assert(pSharedEffect != nullptr && "Effect cannot be null");
	assert(pTexture != nullptr && "Texture cannot be null");

	// Create vertex layout
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create Input layout from first pass of the technique
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = m_pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
		assert(false && "Failed to create input layout");

	// Create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(dae::Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = vertices.data();

	result = m_pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		assert(false && "Failed to create vertex buffer");

	// Create index buffer
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(indices.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	initData.pSysMem = indices.data();
	result = m_pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		assert(false && "Failed to create index buffer");

	// Store vertex and index count for rendering
	m_Vertices = vertices;
	m_NumIndices = static_cast<uint32_t>(indices.size());
}

Mesh::~Mesh()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}

	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}

	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}

	// Only delete effect if we own it (we don't in current implementation)
	if(m_pEffect && m_bOwnEffect)
	{
		delete m_pEffect;
		m_pEffect = nullptr;
	}

	// Don't delete m_pTexture - Renderer owns it
	m_pTexture = nullptr;
}

void Mesh::SetupTechnique(ID3D11DeviceContext* pDeviceContext, SamplerFilter filter) const
{
	ID3DX11EffectTechnique* pTechnique = nullptr;
	
	if (m_IsFireFX)
	{
		// Fire techniques have special blend and depth states
		switch (filter)
		{
		case SamplerFilter::Point:
			pTechnique = m_pEffect->GetEffect()->GetTechniqueByName("FirePointTechnique");
			break;
		case SamplerFilter::Linear:
			pTechnique = m_pEffect->GetEffect()->GetTechniqueByName("FireLinearTechnique");
			break;
		case SamplerFilter::Anisotropic:
			pTechnique = m_pEffect->GetEffect()->GetTechniqueByName("FireAnisotropicTechnique");
			break;
		default:
			pTechnique = m_pEffect->GetEffect()->GetTechniqueByName("FirePointTechnique");
			break;
		}
	}
	else
	{
		// Standard vehicle techniques
		pTechnique = m_pEffect->GetTechnique(filter);
	}

	assert(pTechnique != nullptr && pTechnique->IsValid() && "Technique is invalid");

	// Apply technique passes
	D3DX11_TECHNIQUE_DESC techDesc{};
	pTechnique->GetDesc(&techDesc);
	
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::BindTextures(const D_Texture* pNormalMap, const D_Texture* pSpecularMap, const D_Texture* pGlossinessMap) const
{
	// Bind diffuse texture (always required)
	if (m_pTexture)
	{
		m_pEffect->SetDiffuseMap(m_pTexture->GetShaderResourceView());
	}

	// Only bind additional maps for non-fire meshes
	if (!m_IsFireFX)
	{
		if (pNormalMap)
		{
			m_pEffect->SetNormalMap(pNormalMap->GetShaderResourceView());
		}

		if (pSpecularMap)
		{
			m_pEffect->SetSpecularMap(pSpecularMap->GetShaderResourceView());
		}

		if (pGlossinessMap)
		{
			m_pEffect->SetGlossinessMap(pGlossinessMap->GetShaderResourceView());
		}
	}
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const dae::Matrix& worldViewProjMatrix, 
                  const dae::Matrix& worldMatrix, const dae::Vector3& cameraPos, 
				  D_Texture* pNormalMap, D_Texture* pSpecularMap, D_Texture* pGlossinessMap,
                  SamplerFilter filter) const
{
	assert(pDeviceContext != nullptr && "Device context cannot be null");

	// Set transformation matrices
	m_pEffect->GetWorldViewProjVariable()->SetMatrix(reinterpret_cast<const float*>(&worldViewProjMatrix));
	m_pEffect->GetWorldMatrixVariable()->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));
	m_pEffect->GetCameraPositionVariable()->SetFloatVector(reinterpret_cast<const float*>(&cameraPos));

	// Bind textures
	BindTextures(pNormalMap, pSpecularMap, pGlossinessMap);

	// Set up input assembly
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// Set vertex and index buffers
	constexpr UINT stride = sizeof(dae::Vertex_PosCol);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Setup and execute technique
	SetupTechnique(pDeviceContext, filter);
}