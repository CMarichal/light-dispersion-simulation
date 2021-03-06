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
// FUNCTIONS DECLARATIONS

// Draw the scene at a current instant
void Draw(const Graphics::Scene& scene, const Graphics::Camera& camera, IDrawingManager& manager);
// Update objects positions according to inputs
void Update(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager);
// Handle the camera movements
void ControlCamera(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager);
// Handle the camera displacements
void ControlLight(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager);
// Welcome the user and provides commands
void printWelcomeMessage();

// ----------------------------------------------------------------------------
// MAIN PROGRAMM

int main(int argc, char* argv[])
{
	printWelcomeMessage();

	// Screen where rays are projected to pixels
	const Graphics::Screen SCREEN{
	50 * 2 *2 *2, //width
	50 * 2 *2 *2  //height
	};

	//camera 
	const float FOCAL{ static_cast<float>(SCREEN.height)};
	Graphics::Camera camera(
		vec3(1.f, 0.5f, -0.5f), //position
		FOCAL,
		SCREEN);
	camera.rotationMatrix = Graphics::rotationYMatrix(45) * camera.rotationMatrix;

	//lighting
	constexpr vec3 INDIRECT_LIGHT = 0.5f * Graphics::COLOR_WHITE;

	Graphics::LightDirectional light(
		glm::vec3(0.6f, 0, 0), //position
		Graphics::COLOR_WHITE, //color
		glm::vec3(1, 1, 0) //direction
	);

	//3D models
	Graphics::Scene scene(light, INDIRECT_LIGHT);

	//SFML SCREEN - change those two lines to change of display and input handling libraries
	auto drawingManager = SFML_Manager(camera.screen.width, camera.screen.height);
	auto inputManager = drawingManager;

	//Load a test model
	TestModel::LoadTestModelTriangularPrism(scene.polygons, 6.0f);

	auto chrono = utilities::Chrono();
	chrono.startChrono();
	while (!drawingManager.closedWindowEventHandler())
	{
		drawingManager.cleanWindow();

		Update(scene, camera, inputManager);
		Draw(scene, camera, drawingManager);

		drawingManager.display();
		
		std::cout << "Render time: " << chrono.getChronoElapsedTime() << " ms." << std::endl;
	}
	drawingManager.saveToFile("Screenshot.png");
	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
// MAIN FUNCTIONS DEFINITIONS

void Update(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager)
{
	//step for translations
	float step{ 0.1f };

	//in degrees
	float yaw{ 10.0f };

	//moving camera
	ControlCamera(scene, camera, manager);

	//moving light
	ControlLight(scene, camera, manager);
}

void Draw(const Graphics::Scene& scene, const Graphics::Camera& camera, IDrawingManager& drawingManager)
{
	for (int y = 0; y < camera.screen.height; ++y)
	{
		for (int x = 0; x < camera.screen.width; ++x)
		{
			auto color = Graphics::Raytracing::Dispersion::raytraceRecursiveWithDispersion(camera, scene, x, y, 5);
			drawingManager.drawPixel(x, y, color);
		}
	}
}

void ControlCamera(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager)
{
	//step for translations
	float step{ 0.1f };

	//in degrees
	float yaw{ 10.0f };

	if (manager.isKeyPressed(IInputManager::Key::LEFT_ARROW))
	{
		camera.position.x -= step;
	}
	if (manager.isKeyPressed(IInputManager::Key::RIGHT_ARROW))
	{
		camera.position.x += step;
	}
	if (manager.isKeyPressed(IInputManager::Key::UP_ARROW))
	{
		camera.position.z += step;
	}
	if (manager.isKeyPressed(IInputManager::Key::DOWN_ARROW))
	{
		camera.position.z -= step;
	}
	if (manager.isKeyPressed(IInputManager::Key::R))
	{
		camera.position.y -= step;
	}
	if (manager.isKeyPressed(IInputManager::Key::F))
	{
		camera.position.y += step;
	}
	if (manager.isKeyPressed(IInputManager::Key::T))
	{
		camera.rotationMatrix = Graphics::rotationYMatrix(yaw) * camera.rotationMatrix;
	}
	if (manager.isKeyPressed(IInputManager::Key::Y))
	{
		camera.rotationMatrix = Graphics::rotationYMatrix(-yaw) * camera.rotationMatrix;
	}
	if (manager.isKeyPressed(IInputManager::Key::G))
	{
		camera.rotationMatrix = Graphics::rotationXMatrix(yaw) * camera.rotationMatrix;
	}
	if (manager.isKeyPressed(IInputManager::Key::H))
	{
		camera.rotationMatrix = Graphics::rotationXMatrix(-yaw) * camera.rotationMatrix;
	}
}

void ControlLight(Graphics::Scene& scene, Graphics::Camera& camera, IInputManager& manager)
{
	//step for translations
	float step{ 0.1f };

	//in degrees
	float yaw{ 10.0f };

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


void printWelcomeMessage()
{
	std::cout << "Welcome to Prisms with SMFL" << std::endl;
	std::cout << "This build displays light dispersion on a prism using the SFML display library" << std::endl;
	std::cout << std::endl;

	std::cout << "Camera commands:" << std::endl;
	std::cout << "- Up-down arrows: to move along the Z-axis" << std::endl;
	std::cout << "- Left-right arrows: to move along the X-axis" << std::endl;
	std::cout << "- R, F: to move along the Y-axis" << std::endl;
	std::cout << "- T, Y: to turn around the Y-axis" << std::endl;
	std::cout << std::endl;

	std::cout << "Light commands:" << std::endl;
	std::cout << "- W, S: to move along the Z-axis" << std::endl;
	std::cout << "- A, D: to move along the X-axis" << std::endl;
	std::cout << "- Q, E: to move along the Y-axis" << std::endl;
	std::cout << std::endl;

	std::cout << "Please be sure to hold the button down during the rendering process" << std::endl;
	std::cout << "It usually takes a few seconds to display a 500x500 window" << std::endl;
	std::cout << std::endl;

	std::cout << "This project was a part of the DH2323-DGI21 Computer Graphics and Interaction course at KTH" << std::endl;
	std::cout << "Developer : Charles-Eole Marichal - marichal@kth.se" << std::endl;
	std::cout << std::endl;

	std::cout << "----------------" << std::endl;
}