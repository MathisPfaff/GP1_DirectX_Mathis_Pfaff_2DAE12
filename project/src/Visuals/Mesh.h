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
#include "DataTypes.h"

namespace dae
{
	class D_Mesh final
	{
	public:
		D_Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, 
			   const std::vector<uint32_t>& indices, D_Texture* pTexture,
			   Effect* pSharedEffect, bool isFireFX = false);
		~D_Mesh();

		D_Mesh(const D_Mesh&) = delete;
		D_Mesh(D_Mesh&&) noexcept = delete;
		D_Mesh& operator=(const D_Mesh&) = delete;
		D_Mesh& operator=(D_Mesh&&) noexcept = delete;

		void Render(ID3D11DeviceContext* pDeviceContext, const Matrix& worldViewProjMatrix, 
		           const Matrix& worldMatrix, const Vector3& cameraPos, 
				   D_Texture* pNormalMap, D_Texture* pSpecularMap, D_Texture* pGlossinessMap,
		           SamplerFilter filter = SamplerFilter::Point, CullMode cullMode = CullMode::BackFace) const;

		bool IsFireFX() const { return m_IsFireFX; }
		bool HasValidTexture() const { return m_pTexture != nullptr; }

	private:
		// Core DirectX resources
		ID3D11Device* m_pDevice;
		ID3D11InputLayout* m_pInputLayout;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;

		// Mesh data
		std::vector<Vertex> m_Vertices{};
		uint32_t m_NumIndices{ 0 };

		// References (not owned)
		Effect* m_pEffect;
		D_Texture* m_pTexture;

		// Flags
		bool m_IsFireFX = false;
		bool m_bOwnEffect = false;

		// Helper functions
		void SetupTechnique(ID3D11DeviceContext* pDeviceContext, SamplerFilter filter, CullMode cullMode) const;
		void BindTextures(const D_Texture* pNormalMap, const D_Texture* pSpecularMap, const D_Texture* pGlossinessMap) const;
	};
}