//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_image.h"
#include "SDL_syswm.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "Timer.h"
#include "Renderer.h"
#if defined(_DEBUG)
	#include "LeakDetector.h"
#endif

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	// Leak detection
	#if defined(_DEBUG)
		LeakDetector detector{};
	#endif

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - ***Mathis Pfaff 2DAE12***",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	SamplerFilter currentFilter = SamplerFilter::Point;
	bool printFPS = false;

	// Cull mode enumeration
	enum class CullMode
	{
		BackFace,
		FrontFace,
		None
	};
	CullMode currentCullMode = CullMode::BackFace;

	// Clear color state
	bool useUniformColor = false;

	std::cout << "\033[33m[Key Bindings - SHARED]\033[0m\n";
	std::cout << "\033[33m   [F1]  Toggle Rasterizer Mode (HARDWARE/SOFTWARE)\033[0m\n";
	std::cout << "\033[33m   [F2]  Toggle Vehicle Rotation (ON/OFF)\033[0m\n";
	std::cout << "\033[33m   [F9]  Cycle CullMode (BACK/FRONT/NONE)\033[0m\n";
	std::cout << "\033[33m   [F10] Toggle Uniform ClearColor (ON/OFF)\033[0m\n";
	std::cout << "\033[33m   [F11] Toggle Print FPS (ON/OFF)\033[0m\n\n";

	std::cout << "\033[32m[Key Bindings - HARDWARE]\033[0m\n";
	std::cout << "\033[32m   [F3]  Toggle FireFX (ON/OFF)\033[0m\n";
	std::cout << "\033[32m   [F4]  Cycle Sampler State (POINT/LINEAR/ANISOTROPIC)\033[0m\n\n";

	std::cout << "\033[35m[Key Bindings - SOFTWARE]\033[0m\n";
	std::cout << "\033[35m   [F5]  Cycle Shading Mode (COMBINED/OBSERVED_AREA/DIFFUSE/SPECULAR)\033[0m\n";
	std::cout << "\033[35m   [F6]  Toggle NormalMap (ON/OFF)\033[0m\n";
	std::cout << "\033[35m   [F7]  Toggle DepthBuffer Visualization (ON/OFF)\033[0m\n";
	std::cout << "\033[35m   [F8]  Toggle BoundingBox Visualization (ON/OFF)\033[0m\n\n";

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					pRenderer->SwitchRenderer();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->Changerotation();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F3)
				{
					pRenderer->ToggleFireMesh();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					// Cycle through sampler filters
					switch (currentFilter)
					{
					case SamplerFilter::Point:
						currentFilter = SamplerFilter::Linear;
						break;
					case SamplerFilter::Linear:
						currentFilter = SamplerFilter::Anisotropic;
						break;
					case SamplerFilter::Anisotropic:
						currentFilter = SamplerFilter::Point;
						break;
					}
					pRenderer->SetSamplerFilter(currentFilter);
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					pRenderer->CycleShadingMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					pRenderer->ToggleNormalMap();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F7)
				{
					pRenderer->ToggleDepthBuffer();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F8)
				{
					pRenderer->ToggleBoundingBox();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					pRenderer->CycleCullMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderer->ToggleClearColor();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					printFPS = !printFPS;
					std::cout << "** F11 KEY ** FPS printing -> " << (printFPS ? "On" : "Off") << std::endl;
				}
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f && printFPS)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}