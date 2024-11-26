//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <execution>
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

	m_HorizontalIterator.resize(m_Width);
	m_VerticalIterator.resize(m_Height);

	for (uint32_t i = 0; i < m_Width; i++) m_HorizontalIterator[i] = i;
	for (uint32_t i = 0; i < m_Height; i++) m_VerticalIterator[i] = i;
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	const Matrix cameraToWorld = camera.CalculateCameraToWorld();
	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };

	if (m_IsMultiThreadingEnabled)
	{
		std::vector<uint32_t> pixelIndices{};

		pixelIndices.reserve(amountOfPixels);
		for (uint32_t index{ 0 }; index < amountOfPixels; index++) pixelIndices.emplace_back(index);

		std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(), [&](int i)
			{
				RenderPixel(pScene, i, camera.FOV, m_AspectRatio, cameraToWorld, camera.origin);
			});
	}

	else
	{
		for (uint32_t pixelIndex{}; pixelIndex < amountOfPixels; pixelIndex++)
		{
			RenderPixel(pScene, pixelIndex, camera.FOV, m_AspectRatio, cameraToWorld, camera.origin);
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix& cameraToWorld,const Vector3& cameraOrigin) const
{
	auto& lights = pScene->GetLights();

	auto& materials{ pScene->GetMaterials() };

	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width };

	float rx{ px + 0.5f }, ry{ py + 0.5f };
	float cx{ (2 * ( rx / float(m_Width)) -1) * aspectRatio * fov};
	float cy{ (1 -( 2 * ( ry / float(m_Height)))) * fov};

	
	//Sets screen to black
	ColorRGB finalColor{ };

	//creates a vector that holds a coordinate in 3D space dependent on which pixel the loop is on
	Vector3 rayDirection{ cx,cy,1 };
	rayDirection.Normalize();
	rayDirection = cameraToWorld.TransformVector(rayDirection);

	//Creates a Ray from origin to the point where the current pixel in loop is
	Ray viewRay{cameraOrigin,rayDirection };

	HitRecord closestHit{};
	pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{
		for (int idx{ 0 }; idx < lights.size(); idx++)
		{
			Vector3 lightVec = LightUtils::GetDirectionToLight(lights[idx], closestHit.origin);
			float maxRayLenght = lightVec.Normalize();
			Ray shadowRay(closestHit.origin + closestHit.normal * 0.0001f, lightVec, 0.0001f, maxRayLenght);
			bool shadowDoesHit = pScene->DoesHit(shadowRay);
			float observedArea = Vector3::Dot(closestHit.normal, lightVec);

			if (observedArea > 0.f)
			{
				if (!m_ShadowsEnabled or (m_ShadowsEnabled && !shadowDoesHit))
				{
					if (m_CurrentLightingMode == LightingMode::ObservedArea)
					{
						finalColor += ColorRGB{ observedArea,observedArea,observedArea };
					}

					if (m_CurrentLightingMode == LightingMode::Radiance)
					{
						finalColor += LightUtils::GetRadiance(lights[idx], closestHit.origin);
					}

					if (m_CurrentLightingMode == LightingMode::BRDF)
					{
						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, lightVec, viewRay.direction);
					}

					if (m_CurrentLightingMode == LightingMode::Combined)
					{
						ColorRGB ObservedArea = ColorRGB{ observedArea, observedArea,observedArea };

						ColorRGB Radiance = LightUtils::GetRadiance(lights[idx], closestHit.origin);

						ColorRGB BRDF = materials[closestHit.materialIndex]->Shade(closestHit, lightVec, viewRay.direction);

						finalColor += Radiance * BRDF * ObservedArea;
					}

				}
			}

		}


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

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
