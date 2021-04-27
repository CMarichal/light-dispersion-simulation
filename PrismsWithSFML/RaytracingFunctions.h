#pragma once
#include "stdafx.h"
#include "RaytracingModel.h"

using std::vector;

namespace Graphics
{
	namespace Raytracing
	{

		const float MAX_DISTANCE{ std::numeric_limits<float>::max() };
		constexpr double EPSILON{ 0.00001 };
		const float PI{ static_cast<float>(atan(1) * 4) };


		// Returns true if an intersection with a triangle is found along the ray
		// Files out the given intersection
		bool ClosestIntersection(
			const Ray& ray,
			const vector<Triangle>& triangles,
			Intersection& closestIntersection
		);

		// Returns true if an intersection with a triangle is found along the ray
		// Fills out the distance as lambdaOut and the intersection point as pointOut
		bool TryIntersection(const Ray& ray, const Triangle& triangle, float& lambdaOut, glm::vec3& pointOut);

		// Compute the color of a point directly illuminated by a light source Light
		vec3 DirectLight(const Intersection& i, const vector<Triangle>& triangles, const Light& light);

		// Convert degrees to radians
		float degreeToRad(float degree);

		// Return the rotation matrix around the Y-axis of a yaw Yaw
		glm::mat3 rotationYMatrix(float yaw);

		// Return the color of the pixel according to raytracing
		glm::vec3 raytrace(const Camera& camera, const Scene& scene, int x, int y);
	}
}