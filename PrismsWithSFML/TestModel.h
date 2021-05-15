#ifndef TEST_MODEL_H
#define TEST_MODEL_H

// Defines the test models

#include "stdafx.h"
#include "GraphicsModel.h"

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

	constexpr vec3 grey(0.5f, 0.5f, 0.5f);

	// Loads the Cornell Box. It is scaled to fill the volume:
	// -1 <= x <= +1
	// -1 <= y <= +1
	// -1 <= z <= +1
	void LoadTestModelCornellBox(std::vector<Graphics::Triangle>& triangles);

	// Loads a model with a prism on a plane surface. It is scaled to fill the volume:
	// -1 <= x <= +1
	// -1 <= y <= +1
	// -1 <= z <= +1
	// Changing the prism size will make it wider
	void LoadTestModelTriangularPrism(std::vector<Graphics::Triangle>& triangles, float prismSize = 2);
}

#endif
