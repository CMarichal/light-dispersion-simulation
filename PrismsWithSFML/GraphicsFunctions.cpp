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

    inline float positiveCos(const vec3& v1, const vec3& v2)
    {
        return std::max(glm::dot(v1, v2), 0.0f);
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


        float quadraticFalloff(const Intersection& i, const LightPoint& light)
        {
            const glm::vec3 l = light.pos - i.position;
            return 1 / (4 * PI * glm::dot(l, l));
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


        glm_color_t DirectLight(const Intersection& i, const vector<Triangle>& triangles, const LightPoint& light) 
        {
            Ray ray(light.pos, normalize(i.position - light.pos));

            const glm::vec3 l{ light.pos - i.position };
            float lightToPointDistance{ glm::length(l) };

            auto intersectionList = FindIntersections(ray, triangles);
            if (intersectionList.size() > 0)
            {
                //equation for illumination
                auto closestIntersec = ClosestIntersection(intersectionList);
                // Check if the intersection point is the closest one to the light along the ray
                // Otherwise, cast no light
                if (fabs(closestIntersec.distance - lightToPointDistance) > EPSILON)
                {
                    return COLOR_BLACK;
                }
            }

            const glm::vec3 n{ i.trianglePtr->normal };
            const float cosAngle{ std::max(glm::dot(glm::normalize(l), n), 0.0f) };

            return light.color * quadraticFalloff(i, light);
        }

        glm_color_t computeColorLight(const Intersection& intersection, const Scene& scene, const LightPoint& lightSource)
        {
            auto directLightColor = DirectLight(intersection, scene.polygons, lightSource);
            auto lightDirection = glm::normalize(lightSource.pos - intersection.position);
            return lambertianIllumination(intersection, scene.ambiantLight, directLightColor, lightDirection);
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
                color = computeColorLight(closestIntersection, scene, scene.lightSource);
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
                vec3 lightDir = glm::normalize(scene.lightSource.pos - closestIntersection.position);
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
 
    }
}