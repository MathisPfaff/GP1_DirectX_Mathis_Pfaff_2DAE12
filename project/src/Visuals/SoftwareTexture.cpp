#include "SoftwareTexture.h"
#include "Math/Vector2.h"
#include <SDL_image.h>

namespace dae
{
	S_Texture::S_Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{
	}

	S_Texture::~S_Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	S_Texture* S_Texture::LoadFromFile(const std::string& path)
	{
		return new S_Texture{ IMG_Load(path.c_str()) };
	}

	ColorRGB S_Texture::Sample(const Vector2& uv) const
	{
		const Uint32 u{ Uint32(std::clamp(uv.x, 0.0f, 1.0f) * m_pSurface->w) };
		const Uint32 v{ Uint32(std::clamp(uv.y, 0.0f, 1.0f) * m_pSurface->h) };

		Uint8 r{}, g{}, b{};

		SDL_GetRGB(m_pSurfacePixels[u + (v * m_pSurface->w)], m_pSurface->format, &r, &g, &b);

		return ColorRGB{ float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f };
	}

	Vector3 S_Texture::SampleNormal(const Vector2& uv) const
	{
		const Uint32 u{ Uint32(std::clamp(uv.x, 0.0f, 1.0f) * m_pSurface->w) };
		const Uint32 v{ Uint32(std::clamp(uv.y, 0.0f, 1.0f) * m_pSurface->h) };

		Uint8 r{}, g{}, b{};

		SDL_GetRGB(m_pSurfacePixels[u + (v * m_pSurface->w)], m_pSurface->format, &r, &g, &b);

		float x{ float(r) }, y{ float(g) }, z{ float(b) };

		x = (2 * (x / 255)) - 1.0f;
		y = (2 * (y / 255)) - 1.0f;
		z = (2 * (z / 255)) - 1.0f;

		return Vector3{ x, y, z };
	}
}