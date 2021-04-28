#pragma once
#include "stdafx.h"

#include "GraphicsModel.h"
// Defines a simple test model: The Cornell Box

namespace TestModel
{
	using glm::vec3;
	using Graphics::Material;

	// Defines colors:
	constexpr vec3 red(0.75f, 0.15f, 0.15f);
	constexpr vec3 yellow(0.75f, 0.75f, 0.15f);
	constexpr vec3 green(0.15f, 0.75f, 0.15f);
	constexpr vec3 cyan(0.15f, 0.75f, 0.75f);
	constexpr vec3 blue(0.15f, 0.15f, 0.75f);
	constexpr vec3 purple(0.75f, 0.15f, 0.75f);
	constexpr vec3 white(0.75f, 0.75f, 0.75f);

	// Loads the Cornell Box. It is scaled to fill the volume:
	// -1 <= x <= +1
	// -1 <= y <= +1
	// -1 <= z <= +1
	void LoadTestModel(std::vector<Graphics::Triangle>& triangles);
}

