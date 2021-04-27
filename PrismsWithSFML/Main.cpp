#include "stdafx.h"
#include "Main.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "IDrawingManager.h"
#include <iostream>
#include "RaytracingFunctions.h"
#include "RaytracingModel.h"
#include "TestModel.h"
#include "SFMLhelper.h"
#include "Utilities.h"

// ----------------------------------------------------------------------------
// USING STATEMENTS

using glm::vec3;
using glm::mat3;
using utilities::Chrono;

// ----------------------------------------------------------------------------
// FUNCTIONS

void Draw(const Graphics::Scene& scene, const Graphics::Camera& camera, IDrawingManager& manager);
void Update();

int main(int argc, char* argv[])
{
	const Graphics::Screen SCREEN{
	50 * 2 * 2, //width
	50 * 2 * 2  // height
	};

	//camera 
	const float FOCAL{ static_cast<float>(SCREEN.height) };
	Graphics::Camera camera(
		vec3(0, 0, (-2 * FOCAL / SCREEN.height) - 0.9f),
		FOCAL,
		SCREEN);

	//lighting
	constexpr vec3 INDIRECT_LIGHT = 0.5f * vec3(1, 1, 1);
	Graphics::Light light{
		glm::vec3(0.0f, -0.5f, -0.7f),
		14.f * glm::vec3(1.0f, 1.0f, 1.0f)
	};

	//3D models
	Graphics::Scene scene(light, INDIRECT_LIGHT);

	//SFML SCREEN
	auto drawingManager = SFML_Manager(camera.screen.width, camera.screen.height);

	LoadTestModel(scene.polygons);

	auto chrono = Chrono();
	chrono.startChrono();
	while (!drawingManager.closedWindowEventHandler())
	{
		drawingManager.cleanWindow();

		Update();
		Draw(scene, camera, drawingManager);

		drawingManager.display();
		
		std::cout << "Render time: " << chrono.getChronoElapsedTime() << " ms." << std::endl;
	}

	return EXIT_SUCCESS;
}

void Update()
{
	//step for translations
	float step{ 0.1f };

	//in degrees
	float yaw{ 10.0f };


	//const Uint8* keystate = SDL_GetKeyboardState(NULL);

	////moving camera
	//if (keystate[SDLK_UP])
	//{
	//	camera.position.y -= step;
	//}
	//if (keystate[SDLK_DOWN])
	//{
	//	camera.position.y += step;
	//}
	//if (keystate[SDLK_LEFT])
	//{
	//	//camera.x -= step;

	//	camera.rotationMatrix = Raytracing::rotationYMatrix(yaw) * camera.rotationMatrix;
	//}
	//if (keystate[SDLK_RIGHT])
	//{
	//	//camera.x += step;

	//	camera.rotationMatrix = Raytracing::rotationYMatrix(-yaw) * camera.rotationMatrix;
	//}

	////moving light
	//if (keystate[SDLK_w])
	//{
	//	light.pos += step * camera.forward();
	//}
	//if (keystate[SDLK_s])
	//{
	//	light.pos -= step * camera.forward();
	//}
	//if (keystate[SDLK_a])
	//{
	//	light.pos -= step * camera.right();
	//}
	//if (keystate[SDLK_d])
	//{
	//	light.pos += step * camera.right();
	//}
	//if (keystate[SDLK_q])
	//{
	//	light.pos -= step * camera.down();
	//}
	//if (keystate[SDLK_e])
	//{
	//	light.pos += step * camera.down();
	//}
}



void Draw(const Graphics::Scene& scene, const Graphics::Camera& camera, IDrawingManager& drawingManager)
{
	for (int y = 0; y < camera.screen.height; ++y)
	{
		for (int x = 0; x < camera.screen.width; ++x)
		{
			auto color = Graphics::Raytracing::raytrace(camera, scene, x, y);
			drawingManager.drawPixel(x, y, color);
		}
	}
}

