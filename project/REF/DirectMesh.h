#pragma once
#include "src/Math/HeaderFiles/Math.h"
#include "DirectXTexture.h"

//DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

class Effect;

struct Vertex_In
{
	dae::Vector3 position{};
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
};

struct Vertex
{
	dae::Vector3 position{};
	dae::ColorRGB color{ dae::colors::White };
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
	dae::Vector3 viewDirection{};
};

enum class SampleMode
{
	Point,
	Linear,
	Anisotropic
};

class Effect;

class DirectMesh
{
public:
	DirectMesh(ID3D11Device* pDevice, std::vector<Vertex_In> vertices, std::vector<uint32_t> indices, Effect* effect);
	DirectMesh() = default;
	~DirectMesh();

	void Render(ID3D11DeviceContext* pDeviceContext, const SampleMode& mode)const;
	void Update(const dae::Matrix& viewProjectionMatrix, float dt);

	void SetMatrixShader(const dae::Matrix& viewDir);
	void SetFloat3Shader(const dae::Vector3& cameraPos, const dae::Vector3& lightDir);
	void SetShaderResourceMap(const DirectXTexture& diffuseMap);
	void SetShaderResourceMaps(const DirectXTexture& pDiffuseMap, const DirectXTexture& pNormalMap, const DirectXTexture& pSpecularMap, const DirectXTexture& pGlossinesMap);
	void SetUsingNormalMaps(bool isUsingNormalMaps);
	void ToggleRotation();

private:
	ID3D11Buffer* m_pVertexBuffer{ nullptr };
	ID3D11Buffer* m_pIndexBuffer{ nullptr };
	uint32_t m_NumIndices{};
	ID3D11Device* m_pDevice{ nullptr };
	Effect* m_pEffect{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };
	dae::Matrix m_WorldMatrix{ dae::Matrix::CreateRotation(0.0f, 0.0f, 0.0f) };
	float m_AngleOfModel{};
	bool m_Rotate{ true };
};