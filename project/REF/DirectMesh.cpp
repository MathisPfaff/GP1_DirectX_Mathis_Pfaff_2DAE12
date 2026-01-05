#include "src/pch.h"
#include "src/Math/HeaderFiles/Math.h"
#include "src/Scene/HeaderFiles/DirectMesh.h"
#include "src/Scene/HeaderFiles/Effects.h"

DirectMesh::DirectMesh(ID3D11Device* pDevice, std::vector<Vertex_In> vertices, std::vector<uint32_t> indices, Effect* effect)
	:m_pDevice{ pDevice },
	m_pEffect{ effect }
{

	//vertex layout
	//---------------------------------
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create vertex buffer
	//---------------------------------
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_In) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	//Create Iput Layout
	//---------------------------------
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechniquePoint()->GetPassByIndex(0)->GetDesc(&passDesc);

	const HRESULT hResult = m_pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(hResult))
		return;

	//Create index uffer
	//---------------------------------
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;

}


DirectMesh::~DirectMesh()
{
	m_pIndexBuffer->Release();
	m_pInputLayout->Release();
	m_pVertexBuffer->Release();
}


void DirectMesh::Render(ID3D11DeviceContext* pDeviceContext, const SampleMode& mode)const
{
	//1. Set Primitive Topology
	//---------------------------------
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	//---------------------------------
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//3. Set VertexBuffer
	//---------------------------------
	constexpr UINT stride = sizeof(Vertex_In);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. Set IndexBuffer
	//---------------------------------
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	//---------------------------------
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechniquePoint()->GetDesc(&techDesc);

	switch (mode)
	{
	case SampleMode::Point:
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			m_pEffect->GetTechniquePoint()->GetPassByIndex(i)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
		break;

	case SampleMode::Linear:
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			m_pEffect->GetTechniqueLinear()->GetPassByIndex(i)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
		break;

	case SampleMode::Anisotropic:
		for (UINT i = 0; i < techDesc.Passes; ++i)
		{
			m_pEffect->GetTechniqueAnisotropic()->GetPassByIndex(i)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
		break;

	default:
		break;
	}

}

void DirectMesh::Update(const dae::Matrix& viewProjectionMatrix, float dt)
{
	if (m_Rotate)
	{
		const float angle{ 45.f };
		m_AngleOfModel += (angle * dae::TO_RADIANS) * dt;

		m_WorldMatrix = dae::Matrix::CreateRotationY(m_AngleOfModel) * dae::Matrix::CreateTranslation(0, 0, 50.f);
	}
	SetMatrixShader(viewProjectionMatrix);
}

void DirectMesh::SetMatrixShader(const dae::Matrix& viewDir)
{
	m_pEffect->SetViewMatrixVariables(m_WorldMatrix * viewDir, m_WorldMatrix);
}

void DirectMesh::SetFloat3Shader(const dae::Vector3& cameraPos, const dae::Vector3& lightDir)
{
	m_pEffect->SetFloat3Variables(cameraPos, lightDir);
}

void DirectMesh::SetShaderResourceMap(const DirectXTexture& diffuseMap)
{
	m_pEffect->SetDiffuseMap(diffuseMap);
}

void DirectMesh::SetShaderResourceMaps(const DirectXTexture& pDiffuseMap, const DirectXTexture& pNormalMap, const DirectXTexture& pSpecularMap, const DirectXTexture& pGlossinesMap)
{
	m_pEffect->SetMapVariables(pDiffuseMap, pNormalMap, pSpecularMap, pGlossinesMap);
}

void DirectMesh::SetUsingNormalMaps(bool isUsingNormalMaps)
{
	m_pEffect->SetIsUsingNorMapsVariable(isUsingNormalMaps);
}

void DirectMesh::ToggleRotation()
{
	m_Rotate = !m_Rotate;
}

