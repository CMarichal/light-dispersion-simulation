#include "stdafx.h"
#include "GraphicsFunctions.h"

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

    glm_color_t lambertianIllumination(const Material& material,  const glm_color_t ambiantLight, const glm_color_t& directLight)
    {
        return material.diffuseColor * (directLight + ambiantLight);
    }

    glm_color_t phongIllumination()
    {

    }

    namespace Raytracing
    {
        bool ClosestIntersection(
            const Ray& ray,
            const vector<Triangle>& triangles,
            Intersection& closestIntersection)
        {
            vector<Intersection> intersectionsList{};

            float distance{};
            vec3 pointIntersection{};

            //try to find an intersection for each triangle of the input vector
            int index{ 0 };
            for (const Triangle& triangle : triangles)
            {
                if (TryIntersection(ray, triangle, distance, pointIntersection))
                {
                    auto intersection = Intersection{ pointIntersection, distance, index };
                    intersectionsList.push_back(intersection);
                }
                ++index;
            }


            if (intersectionsList.size() > 0)
            {
                //Looking for the closest intersection
                closestIntersection = *std::min_element(intersectionsList.begin(), intersectionsList.end(),
                    [](Intersection a, Intersection b) {
                        return a.distance < b.distance;
                    });
                return true;
            }
            else
            {
                return false;
            }
        }

        bool TryIntersection(const Ray& ray, const Triangle& triangle, float& lambdaOut, vec3& pointOut)
        {
            const vec3 e1{ triangle.v1 - triangle.v0 };
            const vec3 e2{ triangle.v2 - triangle.v0 };

            const vec3 normalVec{ normalize(cross(e1, e2)) };

            // computing the denominator prevently
            const double den{ dot(ray.direction, normalVec) };

            if (fabs(den) >= EPSILON) {
                const double lambda{ dot(triangle.v0 - ray.start, normalVec) / den };
                const vec3 intersectionPoint{ ray.pointOnRay(static_cast<float>(lambda)) };

                if (0 <= lambda && lambda <= MAX_DISTANCE) {

                    // Solving the linear system
                    const double A{ dot(intersectionPoint - triangle.v0, normalize(e1) / glm::length(e1)) };
                    const double B{ dot(normalize(e1), e2) / glm::length(e1) };
                    const double C{
                        dot(intersectionPoint - triangle.v0, normalize(e2) / glm::length(e2)) };
                    const double D{ dot(normalize(e2), e1) / glm::length(e2) };

                    const double det{ 1 - B * D };

                    if (det != 0) {
                        // computing the linear combination coefficients of the intersection point
                        const double lambda1{ (1 / det) * (A - B * C) };
                        const double lambda2{ (1 / det) * (-A * D + C) };

                        if (0 <= lambda1 && lambda1 <= 1 && 0 <= lambda2 && lambda2 <= 1 &&
                            lambda1 + lambda2 <= 1) {
                            lambdaOut = static_cast<float>(lambda);
                            pointOut = intersectionPoint;
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        glm_color_t DirectLight(const Intersection& i, const vector<Triangle>& triangles, const Light& light) {

            Intersection closestIntersec{};
            Ray ray(light.pos, normalize(i.position - light.pos));

            const glm::vec3 r{ light.pos - i.position };
            float lightToPointDistance{ glm::length(r) };

            ClosestIntersection(ray, triangles, closestIntersec);

            // Check if the intersection point is the closest one to the light along the ray
            // Otherwise, cast no light
            if (fabs(closestIntersec.distance - lightToPointDistance) > EPSILON)
            {
                return COLOR_BLACK;
            }

            const glm::vec3 n{ triangles[i.triangleIndex].normal };
            const float cosAngle{ std::max(glm::dot(glm::normalize(r), n), 0.0f) };

            return light.color * cosAngle / (4 * PI * glm::dot(r, r));
        }

        glm_color_t raytrace(const Camera& camera, const Scene& scene, int x, int y)
        {
            //default color
            glm_color_t color = Graphics::COLOR_BLACK;

            vec3 dirRayFromPixel(x - camera.screen.width / 2, y - camera.screen.height / 2, camera.focal);
            Graphics::Raytracing::Intersection intersection{};
            Graphics::Raytracing::Ray rayFromPixel(camera.position, camera.rotationMatrix * dirRayFromPixel);

            if (Graphics::Raytracing::ClosestIntersection(rayFromPixel, scene.polygons, intersection))
            {
                //equation for illumination
                color = lambertianIllumination(*scene.polygons[intersection.triangleIndex].material, scene.ambiantLight, DirectLight(intersection, scene.polygons, scene.lightSource));
            }

            //color = raytrace_recursive(scene, rayFromPixel, 3, 0, color);

            return color;
        }

        glm_color_t raytrace_recursive(const Scene& scene, const Ray& incomingRay, const int depthMax, const int depth, glm_color_t& color)
        {
            Graphics::Raytracing::Intersection intersection{};

            if (depth > depthMax)
            {
                
                return color;
            }

            if (Graphics::Raytracing::ClosestIntersection(incomingRay, scene.polygons, intersection))
            {

                //equation for illumination
                const glm_color_t diffuseColor = scene.polygons[intersection.triangleIndex].material->diffuseColor;
                
                if (depth < depthMax)
                {
                    auto normal = scene.polygons[intersection.triangleIndex].normal;
                    Ray outcomingRay(intersection.position, glm::reflect(incomingRay.direction, normal));;
                    color = diffuseColor * (scene.ambiantLight + color);
                    return raytrace_recursive(scene, outcomingRay, depthMax, depth+1, color);
                }
                else
                {
                    auto directLightColor = Graphics::Raytracing::DirectLight(intersection, scene.polygons, scene.lightSource);
                    return diffuseColor * directLightColor;
                }
            }
            else
            {
                return color;
            }

        }

        
    }
}