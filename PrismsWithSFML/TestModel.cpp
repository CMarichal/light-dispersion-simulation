#include "stdafx.h"
#include "TestModel.h"

using Graphics::Triangle;
using Graphics::Material;

namespace TestModel
{
	using glm::vec3;

	Material materialFloor{
	green,
	green,
	green,
	0
	};

	Material materialLeftWall{
	purple,
	purple,
	purple,
	0
	};

	Material materialRightWall{
	yellow,
	yellow,
	yellow,
	0
	};

	Material materialCeiling{
	cyan,
	cyan,
	cyan,
	0
	};

	Material materialBackWall{
	white,
	white,
	white,
	0
	};

	Material materialShortBlock{
	red,
	red,
	red,
	0
	};

	Material materialTallBlock{
	blue,
	blue,
	blue,
	0
	};

	void LoadTestModel(std::vector<Triangle>& triangles)
	{
		triangles.clear();
		triangles.reserve(5 * 2 * 3);

		// ---------------------------------------------------------------------------
		// Room

		float L = 555;			// Length of Cornell Box side.

		vec3 A(L, 0, 0);
		vec3 B(0, 0, 0);
		vec3 C(L, 0, L);
		vec3 D(0, 0, L);

		vec3 E(L, L, 0);
		vec3 F(0, L, 0);
		vec3 G(L, L, L);
		vec3 H(0, L, L);

		// Floor:
		triangles.push_back(Triangle(C, B, A, &materialFloor));
		triangles.push_back(Triangle(C, D, B, &materialFloor));

		// Left wall
		triangles.push_back(Triangle(A, E, C, &materialLeftWall));
		triangles.push_back(Triangle(C, E, G, &materialLeftWall));

		// Right wall
		triangles.push_back(Triangle(F, B, D, &materialRightWall));
		triangles.push_back(Triangle(H, F, D, &materialRightWall));

		// Ceiling
		triangles.push_back(Triangle(E, F, G, &materialCeiling));
		triangles.push_back(Triangle(F, H, G, &materialCeiling));

		// Back wall
		triangles.push_back(Triangle(G, D, C, &materialBackWall));
		triangles.push_back(Triangle(G, H, D, &materialBackWall));

		// ---------------------------------------------------------------------------
		// Short block

		A = vec3(290, 0, 114);
		B = vec3(130, 0, 65);
		C = vec3(240, 0, 272);
		D = vec3(82, 0, 225);

		E = vec3(290, 165, 114);
		F = vec3(130, 165, 65);
		G = vec3(240, 165, 272);
		H = vec3(82, 165, 225);

		// Front
		triangles.push_back(Triangle(E, B, A, &materialShortBlock));
		triangles.push_back(Triangle(E, F, B, &materialShortBlock));

		// Front
		triangles.push_back(Triangle(F, D, B, &materialShortBlock));
		triangles.push_back(Triangle(F, H, D, &materialShortBlock));

		// BACK
		triangles.push_back(Triangle(H, C, D, &materialShortBlock));
		triangles.push_back(Triangle(H, G, C, &materialShortBlock));

		// LEFT
		triangles.push_back(Triangle(G, E, C, &materialShortBlock));
		triangles.push_back(Triangle(E, A, C, &materialShortBlock));

		// TOP
		triangles.push_back(Triangle(G, F, E, &materialShortBlock));
		triangles.push_back(Triangle(G, H, F, &materialShortBlock));

		// ---------------------------------------------------------------------------
		// Tall block

		A = vec3(423, 0, 247);
		B = vec3(265, 0, 296);
		C = vec3(472, 0, 406);
		D = vec3(314, 0, 456);

		E = vec3(423, 330, 247);
		F = vec3(265, 330, 296);
		G = vec3(472, 330, 406);
		H = vec3(314, 330, 456);

		// Front
		triangles.push_back(Triangle(E, B, A, &materialTallBlock));
		triangles.push_back(Triangle(E, F, B, &materialTallBlock));

		// Front
		triangles.push_back(Triangle(F, D, B, &materialTallBlock));
		triangles.push_back(Triangle(F, H, D, &materialTallBlock));

		// BACK
		triangles.push_back(Triangle(H, C, D, &materialTallBlock));
		triangles.push_back(Triangle(H, G, C, &materialTallBlock));

		// LEFT
		triangles.push_back(Triangle(G, E, C, &materialTallBlock));
		triangles.push_back(Triangle(E, A, C, &materialTallBlock));

		// TOP
		triangles.push_back(Triangle(G, F, E, &materialTallBlock));
		triangles.push_back(Triangle(G, H, F, &materialTallBlock));


		// ----------------------------------------------
		// Scale to the volume [-1,1]^3

		for (size_t i = 0; i < triangles.size(); ++i)
		{
			triangles[i].v0 *= 2 / L;
			triangles[i].v1 *= 2 / L;
			triangles[i].v2 *= 2 / L;

			triangles[i].v0 -= vec3(1, 1, 1);
			triangles[i].v1 -= vec3(1, 1, 1);
			triangles[i].v2 -= vec3(1, 1, 1);

			triangles[i].v0.x *= -1;
			triangles[i].v1.x *= -1;
			triangles[i].v2.x *= -1;

			triangles[i].v0.y *= -1;
			triangles[i].v1.y *= -1;
			triangles[i].v2.y *= -1;

			triangles[i].ComputeNormal();
		}
	}
}

