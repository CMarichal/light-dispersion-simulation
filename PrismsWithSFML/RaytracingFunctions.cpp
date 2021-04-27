#include "stdafx.h"
#include "RaytracingFunctions.h"

using glm::dot;
using glm::cross;
using glm::normalize;

namespace Graphics
{
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

        vec3 DirectLight(const Intersection& i, const vector<Triangle>& triangles, const Light& light) {

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

        glm::vec3 raytrace(const Camera& camera, const Scene& scene, int x, int y)
        {
            //default color
            vec3 color = Graphics::COLOR_BLACK;

            vec3 dirRayFromPixel(x - camera.screen.width / 2, y - camera.screen.height / 2, camera.focal);
            Graphics::Raytracing::Intersection intersection{};
            Graphics::Raytracing::Ray rayFromPixel(camera.position, camera.rotationMatrix * dirRayFromPixel);

            if (Graphics::Raytracing::ClosestIntersection(rayFromPixel, scene.polygons, intersection))
            {
                //equation for illumination
                const vec3 diffuseColor = scene.polygons[intersection.triangleIndex].color;
                color = diffuseColor * (scene.ambiantLight + Graphics::Raytracing::DirectLight(intersection, scene.polygons, scene.lightSource));
            }
        }
    }
}