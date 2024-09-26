//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <iostream>

#include "Maths.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_AspectRatio = (m_Width / float(m_Height)) ;
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			//float gradient = px / static_cast<float>(m_Width);
			//gradient += py / static_cast<float>(m_Width);
			//gradient /= 2.0f;


			//creates a vector that holds a coordinate in 3D space dependent on which pixel the loop is on
			Vector3 rayDirection{ float((2 * ((px + 0.5) / m_Width) - 1) * m_AspectRatio),float(1 - 2 * ((py + 0.5) / m_Height)),1};
			rayDirection.Normalize();

			//Creates a Ray from origin to the point where the current pixel in loop is
			Ray viewRay{ {0,0,0},rayDirection};

			//Sets screen to black
			ColorRGB finalColor{ };

			HitRecord closestHit{};
			pScene->GetClosestHit(viewRay, closestHit);

			////Makes a tempSphere
			//Sphere testSphere{ {0.f,0.f,100.f},50.f,0 };

			////Perform Sphere HitTest
			//GeometryUtils::HitTest_Sphere(testSphere, viewRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();

				//const float scaled_t = (closestHit.t - 50.f) / 40.f; //shades spheres based on proximity
				//finalColor = { scaled_t,scaled_t,scaled_t };

				//const float scaled_t = (closestHit.t) / 500.f;    //shades planes based on proximity
				//finalColor = { scaled_t,scaled_t,scaled_t };
			}
			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
