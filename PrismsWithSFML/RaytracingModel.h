#pragma once
#include "stdafx.h"

using glm::vec3;

namespace Graphics
{
	constexpr vec3 COLOR_BLACK(0, 0, 0);

	// Represent a screen of pixels
	struct Screen
	{
		int width;
		int height;
	};

	// Represents a camera
	class Camera
	{
	public:
		vec3 position;
		glm::mat3 rotationMatrix;
		float focal;
		Screen screen;

		Camera(vec3 position, float focal, Screen screen) :
			position(position), focal(focal), screen(screen)
		{
			rotationMatrix = glm::mat3(1.0f);
		}

		vec3 right() const
		{
			return vec3(rotationMatrix[0]);
		}

		vec3 down() const
		{
			return vec3(rotationMatrix[1]);
		}

		vec3 forward() const
		{
			return vec3(rotationMatrix[2]);
		}

	};

	// Describes an omni-light source
	struct Light
	{
		vec3 pos;
		vec3 color;
	};


	// Used to describe a triangular surface:
	class Triangle
	{
	public:
		vec3 v0;
		vec3 v1;
		vec3 v2;
		vec3 normal;
		vec3 color;

		Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 color)
			: v0(v0), v1(v1), v2(v2), color(color)
		{
			ComputeNormal();
		}

		void ComputeNormal()
		{
			vec3 e1 = v1 - v0;
			vec3 e2 = v2 - v0;
			normal = glm::normalize(glm::cross(e2, e1));
		}
	};

	// Represents a scene with only one light source
	class Scene
	{
	public:
		vector<Triangle> polygons;
		Light lightSource;
		glm::vec3 ambiantLight;

		Scene(Light lightSource, glm::vec3 ambiantLight) :
			lightSource(lightSource), ambiantLight(ambiantLight)
		{
		}
	};

	namespace Raytracing
	{
		// Describes an intersection point between a Ray and a Triangle
		struct Intersection
		{
			vec3 position;
			float distance;
			int triangleIndex;
		};

		// Describes a ray with a normalized direction
		class Ray
		{
		public:
			vec3 start;
			vec3 direction;

			Ray(vec3 start, vec3 direction) : start(start)
			{
				this->direction = glm::normalize(direction);
			}

			vec3 pointOnRay(float lambda) const
			{
				return start + lambda * direction;
			}
		};


	}
}



