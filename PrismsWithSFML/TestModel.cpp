#include "stdafx.h"
#include "TestModel.h"

using Graphics::Triangle;
using Graphics::Material;

namespace TestModel
{
	using glm::vec3;

	Material materialFloor(green);
	Material materialLeftWall(purple);
	Material materialRightWall(yellow);
	Material materialCeiling(cyan);
	Material materialBackWall(white);
	Material materialShortBlock(red);
	Material materialTallBlock(blue);

	Material materialPrism(
		grey, 
		1, //specular
		0, //diffuse
		0.1, //ambiant
		2, //shininess
		0.f, //reflection
		1, //refraction
		1.5046f,
		420'000.f);
	//BK7 borosilicate glass
	// A =1.5046
	// B =420 000nm²


	void LoadTestModelCornellBox(std::vector<Triangle>& triangles)
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
		triangles.push_back(Triangle(E, B, A, &materialPrism));
		triangles.push_back(Triangle(E, F, B, &materialPrism));

		// Front
		triangles.push_back(Triangle(F, D, B, &materialPrism));
		triangles.push_back(Triangle(F, H, D, &materialPrism));

		// BACK
		triangles.push_back(Triangle(H, C, D, &materialPrism));
		triangles.push_back(Triangle(H, G, C, &materialPrism));

		// LEFT
		triangles.push_back(Triangle(G, E, C, &materialPrism));
		triangles.push_back(Triangle(E, A, C, &materialPrism));

		// TOP
		triangles.push_back(Triangle(G, F, E, &materialPrism));
		triangles.push_back(Triangle(G, H, F, &materialPrism));


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

	void LoadTestModelTriangularPrism(std::vector<Graphics::Triangle>& triangles)
	{
		triangles.clear();
		triangles.reserve(2 + 2 + 3 * 2);

		// ---------------------------------------------------------------------------
		// Room

		float L = 20;			// Length of the scene side.

		//floor
		vec3 A(0, 0, 0);
		vec3 B(0, 0, L);
		vec3 C(L, 0, L);
		vec3 D(L, 0, 0);

		// Floor:
		triangles.push_back(Triangle(C, B, A, &materialFloor));
		triangles.push_back(Triangle(C, A, D, &materialFloor));

		// ---------------------------------------------------------------------------
		// Prism

		float half_L = L/2;
		float sizePrism = 6;
		//prism base
		vec3 E(half_L - sizePrism, 0, half_L + sizePrism);
		vec3 F(half_L + sizePrism, 0, half_L + sizePrism);
		vec3 G(half_L, 0, half_L - sizePrism);

		float height = 8;
		//prism top
		vec3 H = E + vec3(0, height, 0);
		vec3 I = F + vec3(0, height, 0);
		vec3 J = G + vec3(0, height, 0);
		

		// Base
		triangles.push_back(Triangle(E, G, F, &materialPrism));

		// Top
		triangles.push_back(Triangle(H, J, I, &materialPrism));

		// BACK
		triangles.push_back(Triangle(E, H, F, &materialPrism));
		triangles.push_back(Triangle(H, I, F, &materialPrism));

		// LEFT
		triangles.push_back(Triangle(F, I, G, &materialPrism));
		triangles.push_back(Triangle(I, J, G, &materialPrism));

		// RIGHT
		triangles.push_back(Triangle(J, E, G, &materialPrism));
		triangles.push_back(Triangle(E, J, H, &materialPrism));


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

