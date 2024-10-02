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
		float fovAngle{ 90.f };
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

			Vector4 Right{ Vector3{}.Cross(Vector3::UnitY,forward).Normalized() ,0 };
			Vector4 Up{ Vector3{}.Cross(forward,Right).Normalized(),0 };
			Vector4 TVec{ origin ,1 };
			Matrix Onb{ Right,Up,forward, TVec };

			return Onb;
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

			//if (SDL_PollEvent(&event))
			//{
			//	if (SDL_MOUSEMOTION == event.type)
			//	{
			//		int x, y;

			//		SDL_GetMouseState(&x, &y);

			//		forward = Matrix::CreateRotation(x * rotSpeed * pTimer->GetElapsed(), y * rotSpeed * pTimer->GetElapsed(), 1).TransformVector(forward);
			//	}
			//}
			//


			

			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);


			// Move Forward
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				Matrix forwardMovement = Matrix::CreateTranslation(0, 0, movSpeed);
				origin = forwardMovement.TransformPoint(origin);
			}

			// Move Backward
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				Matrix backwardMovement = Matrix::CreateTranslation(0, 0, -movSpeed);
				origin = backwardMovement.TransformPoint(origin);
			}

			// Strafe Left
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				Matrix leftMovement = Matrix::CreateTranslation(-movSpeed, 0, 0);
				origin = leftMovement.TransformPoint(origin);
			}

			// Strafe Right
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				Matrix rightMovement = Matrix::CreateTranslation(movSpeed, 0, 0);
				origin = rightMovement.TransformPoint(origin);
			}

			
			// Process mouse movements (relative to the last frame)
			if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				
				if (mouseX != 0 || mouseY != 0)
				{
					if (forward.z < 0)
					{
						forward = Matrix::CreateRotation(-(mouseY / rotSpeed) * deltaTime, (mouseX / rotSpeed) * deltaTime, 0).TransformVector(forward);
					}

					else
					{
						forward = Matrix::CreateRotation((mouseY / rotSpeed) * deltaTime, (mouseX / rotSpeed) * deltaTime, 0).TransformVector(forward);
					}
					
				}
			}



			/*		Vector3 moveVector = forward.Normalized() * movSpeed * deltaTime;;

					Vector3 rightVector = Vector3{}.Cross(up , forward).Normalized() * movSpeed * deltaTime;

					if (pKeyboardState[SDL_SCANCODE_W])
					{
						origin += moveVector;
					}

					if (pKeyboardState[SDL_SCANCODE_S])
					{
						origin -= moveVector;
					}

					if (pKeyboardState[SDL_SCANCODE_D])
					{
						origin += rightVector;
					}

					if (pKeyboardState[SDL_SCANCODE_A])
					{
						origin -= rightVector;
					}*/


		}
	};
}
