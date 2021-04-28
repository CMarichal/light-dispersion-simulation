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
		constexpr double EPSILON{ 0.000001 };

		// compute a color according to Lambertian Illumination Model
		glm_color_t lambertianIllumination(const Intersection& intersection, const glm_color_t ambiantLight, const glm_color_t& directLight, const vec3& lightDirection);

		// compute a color according to Phong Illumination Model
		glm_color_t phongIllumination(const Intersection& intersection, const glm_color_t ambiantLight, const glm_color_t& directLight, const vec3& lightDirection);

		// compute a color according to Blinn-Phong Illumination Model
		glm_color_t blinnPhongIllumination(const Intersection& intersection, const glm_color_t ambiantLight, const glm_color_t& directLight, const vec3& lightDirection);

		// Returns the list of the intersections along a ray
		vector<Intersection> FindIntersections(const Ray& ray, const vector<Triangle>& triangles);

		// Returns the closest intersection assuming that all intersection share the same ray
		Intersection ClosestIntersection(vector<Intersection>& intersectionsList);

		// Returns true if an intersection with a triangle is found along the ray
		// Fills out the distance as lambdaOut and the intersection point as pointOut
		bool TryIntersection(const Ray& ray, const Triangle& triangle, float& lambdaOut, glm::vec3& pointOut);

		// Compute the color of a point directly illuminated by a light source Light
		glm_color_t DirectLight(const Intersection& i, const vector<Triangle>& triangles, const Light& light);

		// Return the color of the pixel according to raytracing
		glm_color_t raytrace(const Camera& camera, const Scene& scene, int x, int y);

		// Return the color of the pixel according to recursive raytracing
		glm_color_t raytraceRecursive(const Camera& camera, const Scene& scene, int x, int y, const int depthMax);

		glm_color_t raytrace_recursive_call(const Scene& scene, const Ray& incomingRay, const int depthMax, const int depth, glm_color_t& color);
	}
}