#pragma once
#include <cstdint>
#include <vector>
#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class S_Texture;
	class Timer;
	class Scene;

	class S_Renderer final
	{
	public:

		enum class ShadingMode
		{
			COMBINED,
			OBSERVEDAREA,
			DIFFUSE,
			SPECULAR
		};

		S_Renderer(SDL_Window* pWindow);
		~S_Renderer();

		S_Renderer(const S_Renderer&) = delete;
		S_Renderer(S_Renderer&&) noexcept = delete;
		S_Renderer& operator=(const S_Renderer&) = delete;
		S_Renderer& operator=(S_Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();
		bool SaveBufferToImage() const;
		void SwitchDepthBuffer();
		void ToggleNormalMap();
		void ToggleRotateMesh();
		void CycleShadingMode();

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer;
		SDL_Surface* m_pBackBuffer;
		uint32_t* m_pBackBufferPixels;

		std::unique_ptr<S_Texture> m_pDiffuseMap;
		std::unique_ptr<S_Texture> m_pGlossMap;
		std::unique_ptr<S_Texture> m_pNormalMap;
		std::unique_ptr<S_Texture> m_pSpecularMap;

		const float m_Shininess;
		const float m_LightIntensity;

		bool m_UseNormalMap{ true };
		bool m_DepthBufferEnabled{ false };
		ShadingMode m_ShadingMode;

		std::vector<Mesh> m_Meshes;

		float* m_pDepthBufferPixels{};
		int m_AllPixels{};
		std::vector<int> m_PixelIndices;

		Camera m_Camera{};

		int m_Width{};
		int m_Height{};

		const Vector3 m_LightDirection;
		const ColorRGB m_Ambient;

		bool m_RotateMesh{ true };
		float m_RotateTimer;


		Uint32 ColorToUint32(const ColorRGB& color);

		bool InsideFrustum(const Vertex_Out& vertex) const;

		float Remap(float v, float min, float max) const;

		void VertexTransformationFunction(const std::vector<Vertex>& vertices, std::vector<Vertex_Out>& verticesOut, const Matrix& worldMatrix) const;
		void PixelShading(const Vertex_Shader& shaderVertex, int indexBuffer);
	};
}
