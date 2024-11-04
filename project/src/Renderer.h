#pragma once

#include <cstdint>
#include <vector>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;
		void CycleLightingMode()
		{
			// Cast the current mode to int to check if it's the last mode
			if (static_cast<int>(m_CurrentLightingMode) == 3)
			{
				// Set the mode back to the first one
				m_CurrentLightingMode = LightingMode::ObservedArea;
			}

			else
			{
				// Move to the next mode by incrementing the current one
				m_CurrentLightingMode = static_cast<LightingMode>(static_cast<int>(m_CurrentLightingMode) + 1);
			}
			
		};
		void ToggleShadows() { m_ShadowsEnabled = !m_ShadowsEnabled; }
		void ToggleMultiThreading() { m_IsMultiThreadingEnabled = !m_IsMultiThreadingEnabled; }


	private:

		enum class LightingMode
		{
			ObservedArea,
			Radiance,
			BRDF,
			Combined //ObservedArea * Radiance * BRDF
		};

		LightingMode m_CurrentLightingMode{LightingMode::Combined};
		bool m_ShadowsEnabled{true};
		bool m_IsMultiThreadingEnabled{ true };
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
		float m_AspectRatio{};

		std::vector<uint32_t> m_HorizontalIterator, m_VerticalIterator;
	};
}
