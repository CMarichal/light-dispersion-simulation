#pragma once
#include "stdafx.h"

#include "RaytracingModel.h"
// Defines a simple test model: The Cornell Box

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel(std::vector<Graphics::Triangle> & triangles);
