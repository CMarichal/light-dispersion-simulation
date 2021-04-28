#pragma once
#include "stdafx.h"
#include "GraphicsModel.h"

using std::vector;

namespace Graphics
{
	const float PI{ static_cast<float>(atan(1) * 4) };

	// Convert degrees to radians
	float degreeToRad(float degree);

	// Return the rotation matrix around the Y-axis of a yaw Yaw
	glm::mat3 rotationYMatrix(float yaw);

	namespace Raytracing
	{

		const float MAX_DISTANCE{ std::numeric_limits<float>::max() };
		constexpr double EPSILON{ 0.00001 };


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
		glm_color_t DirectLight(const Intersection& i, const vector<Triangle>& triangles, const Light& light);

		// Return the color of the pixel according to raytracing
		glm_color_t raytrace(const Camera& camera, const Scene& scene, int x, int y);

		glm_color_t raytrace_recursive(const Scene& scene, const Ray& incomingRay, const int depthMax, const int depth, glm_color_t& color);
	}
}