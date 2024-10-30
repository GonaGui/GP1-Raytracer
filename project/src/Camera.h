#pragma once
#include <iostream>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "SDL_events.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}


		Vector3 origin{};
		float fovAngle{ 45.f };
		float previousfovAngle{ fovAngle };
		float FOV{ tan((fovAngle * (PI / 180)) / 2) };
		float rotSpeed{10};
		float movSpeed{ 11 };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			right = Vector3::Cross(Vector3::UnitY,forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			return Matrix(right, up, forward, origin);
		}

		void Update(Timer* pTimer)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);

			if (fovAngle != previousfovAngle)
			{
				FOV = tan ((fovAngle * (PI / 180)) / 2);

				previousfovAngle = fovAngle;
			}

			SDL_Event event;

			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			// Process mouse movements (relative to the last frame)
			if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				
				if (mouseX != 0 || mouseY != 0)
				{
					totalPitch = mouseY / rotSpeed * deltaTime;
					totalYaw = mouseX / rotSpeed * deltaTime;

					if (forward.z < 0)
					{
						forward = Matrix::CreateRotation(-totalPitch, totalYaw, 0).TransformVector(forward).Normalized();
					}

					else
					{
						forward = Matrix::CreateRotation(totalPitch, totalYaw, 0).TransformVector(forward).Normalized();
					}
					
				}
			}



			Vector3 forwardVec = forward.Normalized() * movSpeed * deltaTime;

			Vector3 rightVector = Vector3{}.Cross(up , forward).Normalized() * movSpeed * deltaTime;

			Vector3 upVector = Vector3{}.Cross(forward,right).Normalized() * movSpeed * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forwardVec;
			}

			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forwardVec;
			}

			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += rightVector;
			}

			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= rightVector;
			}

			if (pKeyboardState[SDL_SCANCODE_SPACE])
			{
				if (forward.z < 0)
				{
					origin -= upVector;
				}

				else
				{
					origin += upVector;
				}
				
			}

			if (pKeyboardState[SDL_SCANCODE_LSHIFT])
			{
				if (forward.z < 0)
				{
					origin += upVector;
				}

				else
				{
					origin -= upVector;
				}
			}

		}
	};
}
