#pragma once
#include "src/Math/HeaderFiles/Math.h"
#include "vector"

namespace dae
{
	struct Vertex
	{
		Vector3 position{};
		ColorRGB color{colors::White};
		Vector2 uv{}; 
		Vector3 normal{}; 
		Vector3 tangent{}; 
		Vector3 viewDirection{}; 
	};

	struct Vertex_Out
	{
		bool inFrustum;
		Vector4 position{};
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};

		float invZ;
	};

	struct Vertex_Shader
	{
		float depth;
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Mesh
	{
		Mesh(std::vector<Vertex>& vertices_in, const std::vector<uint32_t>& indices_in, PrimitiveTopology primitiveTopology_in, const Vector3& worldLoc) :
			vertices{ vertices_in },
			indices{ indices_in },
			primitiveTopology{ primitiveTopology_in },
			vertices_out{},
			worldMatrix{ Matrix::CreateTranslation(worldLoc) }
		{
			vertices_out.reserve(vertices.size());
			for (int i{}; i < vertices.size(); ++i) vertices_out.emplace_back(Vertex_Out{});
		}


		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		PrimitiveTopology primitiveTopology;

		std::vector<Vertex_Out> vertices_out;
		Matrix worldMatrix;
	};
}
