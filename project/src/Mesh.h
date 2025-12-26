#pragma once

#include <vector>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <DirectXMath.h>
#include "Effect.h"


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

		Position position;
		Color color;
	};

	class Mesh final
	{
	public:

		

		Mesh(ID3D11Device* pDevice, const std::vector<Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;

		void Render(ID3D11DeviceContext* pDeviceContext) const;

	private:
		ID3D11Device* m_pDevice;
		std::vector<Vertex_PosCol> m_Vertices{};
		std::vector<uint32_t> m_Indices{};
		Effect* m_pEffect;
		ID3D11InputLayout* m_pInputLayout;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
	};


