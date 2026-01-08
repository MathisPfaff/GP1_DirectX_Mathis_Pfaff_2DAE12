#pragma once

#include <vector>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <DirectXMath.h>
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Effect.h"
#include "Math/Matrix.h"
#include "DirectXTexture.h"

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
	Mesh(ID3D11Device* pDevice, const std::vector<dae::Vertex_PosCol>& vertices, 
		 const std::vector<uint32_t>& indices, D_Texture* pTexture,
		 Effect* pSharedEffect, bool isFireFX = false);
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContext, const dae::Matrix& worldViewProjMatrix, 
	           const dae::Matrix& worldMatrix, const dae::Vector3& cameraPos, 
			   D_Texture* pNormalMap, D_Texture* pSpecularMap, D_Texture* pGlossinessMap,
	           SamplerFilter filter = SamplerFilter::Point) const;

	bool IsFireFX() const { return m_IsFireFX; }
	bool HasValidTexture() const { return m_pTexture != nullptr; }

private:
	// Core DirectX resources
	ID3D11Device* m_pDevice;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	// Mesh data
	std::vector<dae::Vertex_PosCol> m_Vertices{};
	uint32_t m_NumIndices{ 0 };

	// References (not owned)
	Effect* m_pEffect;
	D_Texture* m_pTexture;

	// Flags
	bool m_IsFireFX = false;
	bool m_bOwnEffect = false;

	// Helper functions
	void SetupTechnique(ID3D11DeviceContext* pDeviceContext, SamplerFilter filter) const;
	void BindTextures(const D_Texture* pNormalMap, const D_Texture* pSpecularMap, const D_Texture* pGlossinessMap) const;
};