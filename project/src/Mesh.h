#pragma once

#include <vector>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <DirectXMath.h>
#include "Effect.h"
#include "Matrix.h"
#include "Texture.h"


struct Vertex_PosCol
{
	struct Position
	{
		float x;
		float y;
		float z;
	};

	struct Color
	{
		float r;
		float g;
		float b;
	};

	struct TexCoord 
	{
		float u;
		float v;
	};

	struct Normal
	{
		float x;
		float y;
		float z;
	};

	struct Tangent
	{
		float x;
		float y;
		float z;
	};

	Position position;
	Color color;
	TexCoord texCoord;
	Normal normal;
	Tangent tangent;
};

class Mesh final
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices, Texture* pTexture);
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContext, const dae::Matrix& worldViewProjMatrix, const dae::Matrix& worldMatrix, const dae::Vector3& cameraPos, Texture* pNormalMap, Texture* pSpecularMap, Texture* pGlossinessMap, SamplerFilter filter = SamplerFilter::Point) const;

private:
	ID3D11Device* m_pDevice;
	std::vector<Vertex_PosCol> m_Vertices{};
	std::vector<uint32_t> m_Indices{};
	Effect* m_pEffect;
	Texture* m_pTexture;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
};