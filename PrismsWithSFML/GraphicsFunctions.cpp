#include "stdafx.h"
#include "GraphicsFunctions.h"

#include <cmath>

using glm::dot;
using glm::cross;
using glm::normalize;

namespace Graphics
{
    float degreeToRad(float degree)
    {
        return degree * PI / 180;
    }

    glm::mat3 rotationYMatrix(float yaw)
    {
        return glm::mat3(
            vec3(cos(degreeToRad(yaw)), 0, sin(degreeToRad(yaw))),
            vec3(0, 1, 0),
            vec3(-sin(degreeToRad(yaw)), 0, cos(degreeToRad(yaw))));
    }

    glm::mat3 rotationXMatrix(float yaw)
    {
        return glm::mat3(
            vec3(1, 0, 0),
            vec3(0, cos(degreeToRad(yaw)), -sin(degreeToRad(yaw))),
            vec3(0, sin(degreeToRad(yaw)), cos(degreeToRad(yaw))));
    }

    inline float positiveCos(const vec3& v1, const vec3& v2)
    {
        return std::max(glm::dot(v1, v2), 0.0f);
    }

    void Interpolate(const float a, const float b, vector<float>& result)
    {
        int N = result.size();
        float step = float(b - a) / float(std::max(N - 1, 1));

        float current = a;
        for (float& pixelResult : result)
        {
            pixelResult = current;
            current += step;
        }
        return;
    }

    namespace Raytracing
    {

        glm_color_t lambertianIllumination(const Intersection& intersection, const glm_color_t& ambiantLight, const glm_color_t& directLight, const vec3& lightDirection)
        {
            const Material* materialPtr = intersection.trianglePtr->material;
            const glm::vec3 n = intersection.trianglePtr->normal;
            const float cosAngle = positiveCos(lightDirection, n);

            return materialPtr->color * (materialPtr->ambiantCoeff * ambiantLight + materialPtr->diffuseCoeff * directLight * cosAngle);
        }


        glm_color_t phongIllumination(const Intersection& intersection, const glm_color_t& ambiantLight, const glm_color_t& directLight, const vec3& lightDirection)
        {
            const Material* materialPtr = intersection.trianglePtr->material;
            const glm::vec3 viewDirection = intersection.rayPtr->direction;
            const glm::vec3 n = intersection.trianglePtr->normal;
            const vec3 reflectedDirection = glm::reflect(lightDirection, n);

            const glm_color_t lambertianPart = lambertianIllumination(intersection, ambiantLight, directLight, lightDirection);
            const glm_color_t specularPart = materialPtr->color * materialPtr->specularCoeff * pow(positiveCos(reflectedDirection, viewDirection), materialPtr->shininess) * directLight;
            
            return lambertianPart + specularPart;
        }


        glm_color_t blinnPhongIllumination(const Intersection& intersection, const glm_color_t& ambiantLight, const glm_color_t& directLight, const vec3& lightDirection)
        {
            const Material* materialPtr = intersection.trianglePtr->material;
            const glm::vec3 viewDirection = intersection.rayPtr->direction;
            const glm::vec3 n = intersection.trianglePtr->normal;
            const vec3 halwayDirection = glm::normalize(lightDirection + viewDirection);

            const glm_color_t lambertianPart = lambertianIllumination(intersection, ambiantLight, directLight, lightDirection);
            const glm_color_t specularPart = materialPtr->color * materialPtr->specularCoeff * pow(positiveCos(halwayDirection, n), materialPtr->shininess) * directLight;

            return lambertianPart + specularPart;
        }

        glm_color_t refractedLight(const Scene& scene, const Intersection& intersection, const Ray& incidentRay, const int depthMax, const int depth)
        {
            auto normal = intersection.trianglePtr->normal;
            float refractiveRatio{};
            //exterior normals assumption
            if (glm::dot(incidentRay.direction, normal) <= 0)
            {
                refractiveRatio = 1 / intersection.trianglePtr->material->refractiveIndex;
            }
            else
            {
                refractiveRatio = intersection.trianglePtr->material->refractiveIndex;
            }
            const Ray refractedRay(intersection.position, glm::refract(incidentRay.direction, normal, refractiveRatio));
            auto refractedLightColor = raytrace_recursive_call(scene, refractedRay, depthMax, depth + 1);
            
            return refractedLightColor;
        }

        
        vector<Intersection> FindIntersections(const Ray& ray, const vector<Triangle>& triangles)
        {
            vector<Intersection> intersectionsList{};

            float distance{};
            vec3 pointIntersection{};

            //try to find an intersection for each triangle of the input vector
            for (const Triangle& triangle : triangles)
            {
                if (TryIntersection(ray, triangle, distance, pointIntersection))
                {
                    if (distance > EPSILON) {
                        auto intersection = Intersection{ pointIntersection, distance, &triangle, &ray };
                        intersectionsList.push_back(intersection);
                    }
                }
            }
            return intersectionsList;
        }


        Intersection ClosestIntersection(vector<Intersection>& intersectionsList)
        {
            return *std::min_element(intersectionsList.begin(), intersectionsList.end(),
                [](Intersection& a, Intersection& b) {
                    return a.distance < b.distance;
                });
        }


        bool TryIntersection(const Ray& ray, const Triangle& triangle, float& lambdaOut, vec3& pointOut)
        {
            const vec3 e1{ triangle.v1 - triangle.v0 };
            const vec3 e2{ triangle.v2 - triangle.v0 };

            const vec3 normalVec{ normalize(cross(e1, e2)) };

            // computing the denominator prevently
            const float den{ dot(ray.direction, normalVec) };

            if (fabs(den) >= EPSILON) {
                const float lambda{ dot(triangle.v0 - ray.start, normalVec) / den };
                const vec3 intersectionPoint{ ray.pointOnRay(lambda) };

                if (0 <= lambda && lambda <= MAX_DISTANCE) {

                    // Solving the linear system
                    const float A{ dot(intersectionPoint - triangle.v0, normalize(e1) / glm::length(e1)) };
                    const float B{ dot(normalize(e1), e2) / glm::length(e1) };
                    const float C{
                        dot(intersectionPoint - triangle.v0, normalize(e2) / glm::length(e2)) };
                    const float D{ dot(normalize(e2), e1) / glm::length(e2) };

                    const float det{ 1 - B * D };

                    if (fabs(det) > EPSILON) {
                        // computing the linear combination coefficients of the intersection point
                        const float lambda1{ (1 / det) * (A - B * C) };
                        const float lambda2{ (1 / det) * (-A * D + C) };

                        if (0 <= lambda1 && lambda1 <= 1 && 0 <= lambda2 && lambda2 <= 1 &&
                            lambda1 + lambda2 <= 1) {
                            lambdaOut = lambda;
                            pointOut = intersectionPoint;
                            return true;
                        }
                    }
                }
            }
            return false;
        }


        glm_color_t DirectLight(const Intersection& i, const vector<Triangle>& triangles, const Light& light) 
        {
            const glm::vec3 l = light.getIncidentRayDirection(i.position);
            Ray ray(light.pos, l);
            
            float lightToPointDistance = light.getDistance(i.position);

            auto intersectionList = FindIntersections(ray, triangles);
            if (intersectionList.size() > 0)
            {
                //equation for illumination
                auto closestIntersec = ClosestIntersection(intersectionList);
                // Check if the intersection point is the closest one to the light along the ray
                // Otherwise, cast no light : Direct shadows
                if (fabs(closestIntersec.distance - lightToPointDistance) > EPSILON)
                {
                    return COLOR_BLACK;
                }
            }

            const glm::vec3 n{ i.trianglePtr->normal };
            const float cosAngle{ std::max(glm::dot(glm::normalize(l), n), 0.0f) };

            return light.color * light.falloff(i.position);
        }

        glm_color_t raytrace(const Camera& camera, const Scene& scene, int x, int y)
        {
            //default color
            glm_color_t color = Graphics::COLOR_BLACK;

            vec3 dirRayFromPixel(x - camera.screen.width / 2, y - camera.screen.height / 2, camera.focal);

            Graphics::Raytracing::Ray rayFromPixel(camera.position, camera.rotationMatrix * dirRayFromPixel);

            auto intersectionList = FindIntersections(rayFromPixel, scene.polygons);
            if (intersectionList.size() > 0)
            {
                auto closestIntersection = ClosestIntersection(intersectionList);
                //equation for illumination
                vec3 incidentRayDir = scene.lightSource.getIncidentRayDirection(closestIntersection.position);
                color = lambertianIllumination(closestIntersection, scene.ambiantLight, scene.lightSource.color, incidentRayDir);
            }
            return color;
        }


        glm_color_t raytraceRecursive(const Camera& camera, const Scene& scene, int x, int y, const int depthMax)
        {
            int depth = 0;
            glm_color_t color = Graphics::COLOR_BLACK;

            vec3 dirRayFromPixel(x - camera.screen.width / 2, y - camera.screen.height / 2, camera.focal);
            Graphics::Raytracing::Ray rayFromPixel(camera.position, camera.rotationMatrix * dirRayFromPixel);

            return raytrace_recursive_call(scene, rayFromPixel, depthMax, depth);
        }


        glm_color_t raytrace_recursive_call(const Scene& scene, const Ray& incomingRay, const int depthMax, const int depth)
        {
            // compute the direct light color at the end
            if (depth >= depthMax)
            {
                return Graphics::COLOR_BLACK;
            }

            //looking for intersection
            auto intersectionList = FindIntersections(incomingRay, scene.polygons);
            if (intersectionList.size() > 0)
            {
                auto closestIntersection = ClosestIntersection(intersectionList);
                auto normal = closestIntersection.trianglePtr->normal;


                // REFLECTION
                glm_color_t reflectedLightColor = Graphics::COLOR_BLACK;
                if (closestIntersection.trianglePtr->material->reflectionCoeff > 0)
                {
                    const Ray reflectedRay(closestIntersection.position, glm::reflect(incomingRay.direction, normal));
                    auto reflectedLightColor = raytrace_recursive_call(scene, reflectedRay, depthMax, depth + 1);
                }

                // REFRACTION
                glm_color_t refractedLightColor = Graphics::COLOR_BLACK;
                if (closestIntersection.trianglePtr->material->refractionCoeff > 0)
                {
                    refractedLightColor = refractedLight(scene, closestIntersection, incomingRay, depthMax, depth);
                }

                // DIRECT ILLUMINATION
                auto originLightColor = DirectLight(closestIntersection, scene.polygons, scene.lightSource);
                vec3 lightDir = -scene.lightSource.getIncidentRayDirection(closestIntersection.position);
                auto illuminationColor = phongIllumination(closestIntersection, scene.ambiantLight, originLightColor, lightDir);

                // ADDING TOGETHER
                auto color = illuminationColor
                    + closestIntersection.trianglePtr->material->reflectionCoeff * reflectedLightColor
                    + closestIntersection.trianglePtr->material->refractionCoeff * refractedLightColor;
                return color;
            }
            // no object found
            return Graphics::COLOR_BLACK;
        }


        namespace Dispersion
        {
            glm_color_t raytraceRecursiveWithDispersion(const Camera& camera, const Scene& scene, int x, int y, const int depthMax)
            {
                int depth = 0;
                glm_color_t color = Graphics::COLOR_BLACK;

                vec3 dirRayFromPixel(x - camera.screen.width / 2, y - camera.screen.height / 2, camera.focal);
                RayWave rayFromPixel(camera.position, camera.rotationMatrix * dirRayFromPixel);
                rayFromPixel.isMonochromatic = false;
                return recursive_raytracing_with_dispersion_call(scene, rayFromPixel, depthMax, depth);
            }

            glm_color_t refractedLightWithDispersion(const Scene& scene, const Intersection& intersection, const RayWave& incidentRayWave, const int depthMax, const int depth)
            {
                auto normal = intersection.trianglePtr->normal;

                float refractiveRatio{};
                //exterior normals assumption
                if (glm::dot(incidentRayWave.direction, normal) <= 0)
                {
                    refractiveRatio = 1 / intersection.trianglePtr->material->cauchyRefractiveIndex(incidentRayWave.wavelength);
                }
                else
                {
                    refractiveRatio = intersection.trianglePtr->material->cauchyRefractiveIndex(incidentRayWave.wavelength);
                }
                RayWave refractedRay(intersection.position, glm::refract(incidentRayWave.direction, normal, refractiveRatio));
                refractedRay.isMonochromatic = true;
                refractedRay.wavelength = incidentRayWave.wavelength;
                auto refractedLightColor = recursive_raytracing_with_dispersion_call(scene, refractedRay, depthMax, depth + 1);

                return refractedLightColor;
            }

            glm_color_t recursive_raytracing_with_dispersion_call(const Scene& scene, const RayWave& incidentRayWave, const int depthMax, const int depth)
            {
                // compute the direct light color at the end
                if (depth >= depthMax)
                {
                    return Graphics::COLOR_BLACK;
                }

                //looking for intersection
                auto intersectionList = FindIntersections(incidentRayWave, scene.polygons);
                if (intersectionList.size() > 0)
                {
                    auto closestIntersection = ClosestIntersection(intersectionList);
                    auto normal = closestIntersection.trianglePtr->normal;


                    // REFLECTION
                    glm_color_t reflectedLightColor = Graphics::COLOR_BLACK;
                    if (closestIntersection.trianglePtr->material->reflectionCoeff > 0)
                    {
                        RayWave reflectedRay(closestIntersection.position, glm::reflect(incidentRayWave.direction, normal));
                        if (incidentRayWave.isMonochromatic)
                        {
                            reflectedRay.isMonochromatic = true;
                            reflectedRay.wavelength = incidentRayWave.wavelength;
                        }
                        auto reflectedLightColor = recursive_raytracing_with_dispersion_call(scene, reflectedRay, depthMax, depth + 1);
                    }

                    // REFRACTION
                    glm_color_t refractedLightColor = Graphics::COLOR_BLACK;
                    if (closestIntersection.trianglePtr->material->refractionCoeff > 0)
                    {
                        // check out if the ray is already monochromatic or if the material is non-dispersive
                        if (incidentRayWave.isMonochromatic || (closestIntersection.trianglePtr->material->refractiveIndex == 1))
                        {
                            refractedLightColor = refractedLight(scene, closestIntersection, incidentRayWave, depthMax, depth);
                        }
                        else
                        {
                            // Interpolation of wavelengths
                            const int nbInterpolation = 10;
                            vector<float> wavelengths{};
                            wavelengths.resize(nbInterpolation);
                            Interpolate(VISIBLE_SPECTRUM_START, VISIBLE_SPECTRUM_END, wavelengths);

                            for (float wavelength : wavelengths)
                            {
                                //additive color mixing
                                auto monochromaticIncidentRay = RayWave(incidentRayWave, wavelength);
                                refractedLightColor += refractedLightWithDispersion(scene, closestIntersection, monochromaticIncidentRay, depthMax, depth);
                            }
                            refractedLightColor /= nbInterpolation; //energy preservation
                        }
                    }

                    // DIRECT ILLUMINATION
                    auto originLightColor = DirectLight(closestIntersection, scene.polygons, scene.lightSource);
                    vec3 lightDir = -scene.lightSource.getIncidentRayDirection(closestIntersection.position);
                    auto illuminationColor = phongIllumination(closestIntersection, scene.ambiantLight, originLightColor, lightDir);

                    // ADDING TOGETHER
                    auto color = illuminationColor
                        + closestIntersection.trianglePtr->material->reflectionCoeff * reflectedLightColor
                        + closestIntersection.trianglePtr->material->refractionCoeff * refractedLightColor;
                    if (incidentRayWave.isMonochromatic)
                    {
                        auto wavelengthColor = WavelengthRGBFilter(incidentRayWave.wavelength);
                        color *= wavelengthColor;
                    }
                    return color;
                }
                // no object found
                return Graphics::COLOR_BLACK;
            }

            glm_color_t WavelengthRGBFilter(float wavelength)
            {
                if (380 <= wavelength && wavelength < 410)
                {
                    float R = 0.6f - 0.41f * (410 - wavelength) / 30;
                    float G = 0;
                    float B = 0.39f + 0.6f * (410 - wavelength) / 30;
                    return glm_color_t(R, G, B);
                }
                else if (410 <= wavelength && wavelength < 440)
                {
                    float R = 0.19f - 0.19f * (440-wavelength) / 30;
                    float G = 0;
                    float B = 1;
                    return glm_color_t(R, G, B);
                }
                else if (440 <= wavelength && wavelength < 490)
                {
                    float R = 0;
                    float G = 1 - (490 - wavelength) / 50;
                    float B = 1;
                    return glm_color_t(R, G, B);
                }
                else if (490 <= wavelength && wavelength < 510)
                {
                    float R = 0;
                    float G = 1;
                    float B = (510 - wavelength) / 20;
                    return glm_color_t(R, G, B);
                }
                else if (510 <= wavelength && wavelength < 580)
                {
                    float R = 1 - (580 - wavelength) / 70;
                    float G = 1;
                    float B = 0;
                    return glm_color_t(R, G, B);
                }
                else if (580 <= wavelength && wavelength < 640)
                {
                    float R = 1;
                    float G = (640 - wavelength) / 60;
                    float B = 0;
                    return glm_color_t(R, G, B);
                }
                else if (640 <= wavelength && wavelength < 700)
                {
                    float R = 1;
                    float G = 0;
                    float B = 0;
                    return glm_color_t(R, G, B);
                }
                else if (700 <= wavelength && wavelength < 780)
                {
                    float R = 0.35f - 0.65f * (780 - wavelength) / 80;
                    float G = 0;
                    float B = 0;
                    return glm_color_t(R, G, B);
                }
                else
                {
                    return COLOR_BLACK;
                }
            }

        }
    }
}