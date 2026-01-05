#pragma once

#include <vector>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <DirectXMath.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Effect.h"
#include "Matrix.h"
#include "Texture.h"

namespace dae
{
	struct Vertex_PosCol
	{
		Vector3 position;
		Vector3 color;
		Vector2 texCoord;
		Vector3 normal;
		Vector3 tangent;
	};
}

class Mesh final
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<dae::Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices, Texture* pTexture, Effect* pSharedEffect, bool isFireFX = false); // Add Effect parameter
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContext, const dae::Matrix& worldViewProjMatrix, const dae::Matrix& worldMatrix, const dae::Vector3& cameraPos, Texture* pNormalMap, Texture* pSpecularMap, Texture* pGlossinessMap, SamplerFilter filter = SamplerFilter::Point) const;

private:
	ID3D11Device* m_pDevice;
	std::vector<dae::Vertex_PosCol> m_Vertices{};
	std::vector<uint32_t> m_Indices{};
	Effect* m_pEffect; // Now points to shared effect (don't delete!)
	Texture* m_pTexture;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	bool m_IsFireFX = false;
	bool m_bOwnEffect = false; // Track if we own it
};