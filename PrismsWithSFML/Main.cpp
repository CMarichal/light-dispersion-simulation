#include "stdafx.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "IDrawingManager.h"
#include "IInputManager.h"
#include <iostream>
#include "GraphicsModel.h"
#include "GraphicsFunctions.h"
#include "TestModel.h"
#include "SFMLhelper.h"
#include "Utilities.h"


// ----------------------------------------------------------------------------
// USING STATEMENTS

using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// FUNCTIONS

void Draw(const Graphics::Scene& scene, const Graphics::Camera& camera, IDrawingManager& manager);
void Update(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager);

int main(int argc, char* argv[])
{
	const Graphics::Screen SCREEN{
	50 * 2 * 2, //width
	50 * 2 * 2  // height
	};

	//camera 
	const float FOCAL{ static_cast<float>(SCREEN.height)};
	Graphics::Camera camera(
		vec3(0, 0, (-2 * FOCAL / SCREEN.height) - 0.9f),
		FOCAL,
		SCREEN);

	//lighting
	constexpr vec3 INDIRECT_LIGHT = 0.5f * vec3(1, 1, 1);
	Graphics::Light light{
		glm::vec3(0.0f, -0.5f, -0.7f),
		20.f * glm::vec3(1.0f, 1.0f, 1.0f)
	};

	//3D models
	Graphics::Scene scene(light, INDIRECT_LIGHT);

	//SFML SCREEN
	auto drawingManager = SFML_Manager(camera.screen.width, camera.screen.height);

	TestModel::LoadTestModel(scene.polygons);

	auto chrono = utilities::Chrono();
	chrono.startChrono();
	while (!drawingManager.closedWindowEventHandler())
	{
		drawingManager.cleanWindow();

		Update(scene, camera, drawingManager);
		Draw(scene, camera, drawingManager);

		drawingManager.display();
		
		std::cout << "Render time: " << chrono.getChronoElapsedTime() << " ms." << std::endl;
	}

	return EXIT_SUCCESS;
}

void Update(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager)
{
	//step for translations
	float step{ 0.1f };

	//in degrees
	float yaw{ 10.0f };

	//moving camera
	if (manager.isKeyPressed(IInputManager::Key::UP_ARROW))
	{
		camera.position.y -= step;
	}
	if (manager.isKeyPressed(IInputManager::Key::DOWN_ARROW))
	{
		camera.position.y += step;
	}
	if (manager.isKeyPressed(IInputManager::Key::R))
	{
		camera.position.z += step;
	}
	if (manager.isKeyPressed(IInputManager::Key::F))
	{
		camera.position.z -= step;
	}
	if (manager.isKeyPressed(IInputManager::Key::LEFT_ARROW))
	{
		camera.rotationMatrix = Graphics::rotationYMatrix(yaw) * camera.rotationMatrix;
	}
	if (manager.isKeyPressed(IInputManager::Key::RIGHT_ARROW))
	{
		camera.rotationMatrix = Graphics::rotationYMatrix(-yaw) * camera.rotationMatrix;
	}

	//moving light
	if (manager.isKeyPressed(IInputManager::Key::W))
	{
		scene.lightSource.pos += step * camera.forward();
	}
	if (manager.isKeyPressed(IInputManager::Key::S))
	{
		scene.lightSource.pos -= step * camera.forward();
	}
	if (manager.isKeyPressed(IInputManager::Key::A))
	{
		scene.lightSource.pos -= step * camera.right();
	}
	if (manager.isKeyPressed(IInputManager::Key::D))
	{
		scene.lightSource.pos += step * camera.right();
	}
	if (manager.isKeyPressed(IInputManager::Key::Q))
	{
		scene.lightSource.pos -= step * camera.down();
	}
	if (manager.isKeyPressed(IInputManager::Key::E))
	{
		scene.lightSource.pos += step * camera.down();
	}
}



void Draw(const Graphics::Scene& scene, const Graphics::Camera& camera, IDrawingManager& drawingManager)
{
	for (int y = 0; y < camera.screen.height; ++y)
	{
		for (int x = 0; x < camera.screen.width; ++x)
		{
			auto color = Graphics::Raytracing::raytraceRecursive(camera, scene, x, y, 5);
			drawingManager.drawPixel(x, y, color);
		}
	}
}

