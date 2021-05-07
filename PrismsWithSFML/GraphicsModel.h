#pragma once
#include "stdafx.h"

using glm::vec3;

namespace Graphics
{
	typedef glm::vec3 glm_color_t;
	constexpr glm_color_t COLOR_BLACK(0, 0, 0);

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

	// Describes an omni-directional light source
	struct LightPoint
	{
		vec3 pos;
		vec3 color;
	};

	// Describes an directional source
	struct LightDirectional
	{
		vec3 pos;
		vec3 direction;
		vec3 color;
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

		Material(vec3 color, float specularCoeff, float ambiantCoeff, float diffuseCoeff,
			float shininess,
			float reflectionCoeff,
			float refractionCoeff,
			float refractiveIndex) :
			color(color),
			specularCoeff(specularCoeff),
			diffuseCoeff(diffuseCoeff),
			ambiantCoeff(ambiantCoeff),
			shininess(shininess),
			reflectionCoeff(reflectionCoeff),
			refractionCoeff(refractionCoeff),
			refractiveIndex(refractiveIndex)

		{}

		Material(vec3 color):
			color(color),
			specularCoeff(1.f),
			diffuseCoeff(1.f),
			ambiantCoeff(1.f),
			shininess(1),
			reflectionCoeff(0.02f),
			refractionCoeff(0.f),
			refractiveIndex(2.417f) //Diamond
		{}
		
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
		LightPoint lightSource;
		glm_color_t ambiantLight;

		Scene(LightPoint lightSource, glm::vec3 ambiantLight) :
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

	}

	namespace Dispersion
	{

		// Describes a color using a simplified wave model
		class ColorWave
		{
		public:
			float wavelenght; //in nanometer

			ColorWave(float wavelenght):
				wavelenght{wavelenght}
			{ }

			// compute an approximation of the RGB color from the wavelength using
			// the method from Mihai and Strajescu, FROM WAVELENGTH TO RGB FILTER, 2007
			glm_color_t toRGBcolor()
			{
				if (380 <= wavelenght && wavelenght < 410)
				{
					float R = 0.6f - 0.41f * (410 - wavelenght) / 30;
					float G = 0;
					float B = 0.39f + 0.6f * (410 - wavelenght) / 30;
					return glm_color_t(R, G, B);
				}
				else if (410 <= wavelenght && wavelenght < 440)
				{
					float R = 0;
					float G = 1-(490-wavelenght) / 50;
					float B = 1;
					return glm_color_t(R, G, B);
				}
				else if (490 <= wavelenght && wavelenght < 510)
				{
					float R = 0;
					float G = 1;
					float B = (510-wavelenght)/20;
					return glm_color_t(R, G, B);
				}
				else if (510 <= wavelenght && wavelenght < 580)
				{
					float R = 1-(580-wavelenght)/70;
					float G = 1;
					float B = 0;
					return glm_color_t(R, G, B);
				}
				else if (580 <= wavelenght && wavelenght < 640)
				{
					float R = 1;
					float G = (640 - wavelenght)/60;
					float B = 0;
					return glm_color_t(R, G, B);
				}
				else if (640 <= wavelenght && wavelenght < 700)
				{
					float R = 1;
					float G = 0;
					float B = 0;
					return glm_color_t(R, G, B);
				}
				else if (700 <= wavelenght && wavelenght < 780)
				{
					float R = 0.35f-0.65f*(780-wavelenght)/80;
					float G = 0;
					float B = 0;
					return glm_color_t(R, G, B);
				}
				else
				{
					return COLOR_BLACK;
				}
			}
		};


	}

}



