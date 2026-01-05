#include "Mesh.h"


Mesh::Mesh(ID3D11Device* pDevice, const std::vector<dae::Vertex_PosCol>& vertices, 
           const std::vector<uint32_t>& indices, Texture* pTexture, 
           Effect* pSharedEffect, bool isFireFX) :
    m_pDevice{ pDevice },
    m_Vertices{ vertices },
    m_Indices{ indices },
    m_pTexture{ pTexture },
    m_IsFireFX{ isFireFX },
    m_pEffect{ pSharedEffect },  // Use shared effect
    m_bOwnEffect{ false }  // We don't own this
{
    // Remove: m_pEffect = new Effect(pDevice, L"resources/PosCol3D.fx");

	// create vertex layout
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


	// create Input layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = m_pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
		assert(false);

	//create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(dae::Vertex_PosCol) * static_cast<uint32_t>(m_Vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = vertices.data();

	result = m_pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	if (FAILED(result))
		assert(false);

	//create index buffer
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(indices.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = m_pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

	if (FAILED(result))
		assert(false);
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

	// Only delete if we own it
	if(m_pEffect && m_bOwnEffect)
	{
		delete m_pEffect;
		m_pEffect = nullptr;
	}

	m_pTexture = nullptr;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const dae::Matrix& worldViewProjMatrix, const dae::Matrix& worldMatrix, const dae::Vector3& cameraPos, Texture* pNormalMap, Texture* pSpecularMap, Texture* pGlossinessMap, SamplerFilter filter) const
{
	// Set the WorldViewProjection matrix
	m_pEffect->GetWorldViewProjVariable()->SetMatrix(reinterpret_cast<const float*>(&worldViewProjMatrix));

	// Set the World matrix
	m_pEffect->GetWorldMatrixVariable()->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));

	// Set the Camera position
	m_pEffect->GetCameraPositionVariable()->SetFloatVector(reinterpret_cast<const float*>(&cameraPos));

	// Set the diffuse texture
	if (m_pTexture)
	{
		m_pEffect->SetDiffuseMap(m_pTexture->GetShaderResourceView());
	}

	// Set the normal map (only for non-fireFX meshes)
	if (!m_IsFireFX && pNormalMap)
	{
		m_pEffect->SetNormalMap(pNormalMap->GetShaderResourceView());
	}

	// Set the specular map (only for non-fireFX meshes)
	if (!m_IsFireFX && pSpecularMap)
	{
		m_pEffect->SetSpecularMap(pSpecularMap->GetShaderResourceView());
	}

	// Set the glossiness map (only for non-fireFX meshes)
	if (!m_IsFireFX && pGlossinessMap)
	{
		m_pEffect->SetGlossinessMap(pGlossinessMap->GetShaderResourceView());
	}

	// 1. Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// 3. Set Vertex Buffer
	constexpr UINT stride = sizeof(dae::Vertex_PosCol);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// 4. Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 5. Draw using appropriate technique based on mesh type
	ID3DX11EffectTechnique* pTechnique = nullptr;
	
	if (m_IsFireFX)
	{
		// Use fire techniques with no-cull rasterizer state
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
		}
	}
	else
	{
		// Use standard techniques
		pTechnique = m_pEffect->GetTechnique(filter);
	}

	D3DX11_TECHNIQUE_DESC techDesc{};
	pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
	}
}