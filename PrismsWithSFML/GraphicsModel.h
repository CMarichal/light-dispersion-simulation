#ifndef GRAPHICS_MODEL_H
#define GRAPHICS_MODEL_H

#include "stdafx.h"

using glm::vec3;

namespace Graphics
{

	typedef glm::vec3 glm_color_t;
	constexpr glm_color_t COLOR_BLACK(0, 0, 0);
	constexpr glm_color_t COLOR_WHITE(1, 1, 1);
	
	const float PI{ static_cast<float>(atan(1) * 4) };

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

	// Describes a light source
	class Light
	{
	public:
		vec3 pos;
		glm_color_t color;

	protected:
		Light(vec3 position, glm_color_t color):
			pos(position),
			color(color)
		{ }

	public:

		virtual vec3 getIncidentRayDirection(vec3 hitPoint) const = 0;

		virtual float getDistance(vec3 hitPoint) const = 0;

		virtual float falloff(vec3 hitPoint) const = 0;

	};

	// Describes an omni-directional light source
	class LightPoint : public virtual Light
	{
	public:
		LightPoint(vec3 pos, glm_color_t color) :
			Light(pos, color)
		{};

		vec3 getIncidentRayDirection(vec3 hitPoint) const override
		{
			return glm::normalize(hitPoint - pos);
		}

		float getDistance(vec3 hitPoint) const override
		{
			return glm::length(hitPoint - pos);
		}

		float falloff(vec3 hitPoint) const override
		{
			float d = getDistance(hitPoint);
			return 1 / (4 * PI * d*d);
		}

	};

	// Describes an directional source
	class LightDirectional : public virtual Light
	{
	public:
		vec3 direction;
	
		LightDirectional(vec3 pos, glm_color_t color, vec3 dir) :
			Light(pos, color)
		{
			direction = glm::normalize(dir);
		};

		vec3 getIncidentRayDirection(vec3 hitPoint) const override
		{
			return direction;
		}

		float getDistance(vec3 hitPoint) const override
		{
			auto hypothenus = hitPoint - pos;
			return fabs(glm::dot(hypothenus, direction));
		}

		float falloff(vec3 hitPoint) const override
		{
			float d = getDistance(hitPoint);
			return 1/ (d);
		}

	};

	// Describes a material and how it reflects light
	class Material
	{

	public:
		const glm_color_t color;
		const float specularCoeff;
		const float diffuseCoeff;
		const float ambiantCoeff;
		const float shininess;
		const float reflectionCoeff;
		const float refractionCoeff;
		const float refractiveIndex;
		float cauchyCoeff_A; // no unit
		float cauchyCoeff_B; //nanometer squared

		Material(vec3 color, float specularCoeff, float ambiantCoeff, float diffuseCoeff,
			float shininess,
			float reflectionCoeff,
			float refractionCoeff,
			float A,
			float B) :
			color(color),
			specularCoeff(specularCoeff),
			diffuseCoeff(diffuseCoeff),
			ambiantCoeff(ambiantCoeff),
			shininess(shininess),
			reflectionCoeff(reflectionCoeff),
			refractionCoeff(refractionCoeff),
			refractiveIndex(A),
			cauchyCoeff_A(A),
			cauchyCoeff_B(B)
		{}

		Material(vec3 color):
			color(color),
			specularCoeff(1.f),
			diffuseCoeff(1.f),
			ambiantCoeff(1.f),
			shininess(1),
			reflectionCoeff(0.02f),
			refractionCoeff(0.f),
			refractiveIndex(1.f),
			cauchyCoeff_A(1.f),
			cauchyCoeff_B(0.0f)//Glass
		{}

		// Return the refractive index according to Cauchy's formula
		// wavelength is in nanometer
		float cauchyRefractiveIndex(float wavelength)
		{
			return cauchyCoeff_A + cauchyCoeff_B / (wavelength * wavelength);
		}
		
	};


	// Used to describe a triangular surface:
	class Triangle
	{
	public:
		vec3 v0;
		vec3 v1;
		vec3 v2;
		vec3 normal;
		Material* material;


		Triangle(vec3 v0, vec3 v1, vec3 v2, Material* material)
			: v0(v0), v1(v1), v2(v2), material(material)
		{
			this->ComputeNormal();
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
		std::vector<Triangle> polygons;
		Light& lightSource;
		glm_color_t ambiantLight;

		Scene(Light& lightSource, const glm::vec3& ambiantLight) :
			lightSource(lightSource), ambiantLight(ambiantLight)
		{
		}
	};


	// Namespace containing raytracing models and functions
	namespace Raytracing
	{
		// Describes a ray with a normalized direction
		class Ray
		{
		public:
			vec3 start;
			vec3 direction;

			Ray(vec3 start, vec3 dir) : start(start)
			{
				direction = glm::normalize(dir);
			}

			vec3 pointOnRay(float lambda) const
			{
				return start + lambda * direction;
			}
		};

		// Describes an intersection point between a Ray and a Triangle
		struct Intersection
		{
			vec3 position;
			float distance;
			const Triangle* trianglePtr;
			const Ray* rayPtr;
		};



		namespace Dispersion
		{

			class RayWave : public Ray
			{
			public:
				bool isMonochromatic;
				float wavelength;

				RayWave(vec3 start, vec3 dir) :
					Ray(start, dir)
				{
					wavelength = 0;
					isMonochromatic = false;
				}

				RayWave(const RayWave& upgradedRay) :
					Ray(upgradedRay.start, upgradedRay.direction),
					wavelength(0)
				{
					isMonochromatic = false;
				}

				RayWave(const RayWave& upgradedRay, float wavelength) :
					Ray(upgradedRay.start, upgradedRay.direction),
					wavelength(wavelength)
				{
					isMonochromatic = true;
				}


			};
		}
	}

}

#endif



