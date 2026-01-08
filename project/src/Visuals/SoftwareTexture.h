#pragma once
#include <SDL_surface.h>
#include <string>
#include "Math/ColorRGB.h"
#include "Math/Vector3.h"

namespace dae
{
	class S_Texture
	{
	public:
		~S_Texture();

		static S_Texture* LoadFromFile(const std::string& path);
		ColorRGB Sample(const Vector2& uv) const;
		Vector3 SampleNormal(const Vector2& uv) const;

	private:
		S_Texture(SDL_Surface* pSurface);

		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
	};
}