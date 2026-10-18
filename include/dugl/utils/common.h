#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <span>


struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

// Computes a vector in world space from a point in screen space.
inline glm::vec3 castScreenRay(float screenX, float screenY, int viewportWidth, int viewportHeight, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    glm::vec4 viewport(0.0f, 0.0f, (float)viewportWidth, (float)viewportHeight);

    screenY = viewportHeight - screenY;
    
    glm::vec3 nearPlanePoint = glm::unProject(
        glm::vec3(screenX, screenY, 0.0f),
        viewMatrix,
        projectionMatrix,
        viewport);
    
    glm::vec3 farPlanePoint = glm::unProject(
        glm::vec3(screenX, screenY, 1.0f),
        viewMatrix,
        projectionMatrix,
        viewport);

    glm::vec3 direction = glm::normalize(farPlanePoint - nearPlanePoint);

    return direction;
}

// True if a given line (defined by an origin and direction) comes within a specified distance of a target point.
inline bool checkRayIntersection(glm::vec3 target, float distance, glm::vec3 origin, glm::vec3 direction)
{
    glm::vec3 toTarget = target - origin;

    float t = glm::dot(toTarget, direction);

    // ray is cast in wrong direction
    if (t < 0.0f) {
        return false;
    }

    glm::vec3 closestPoint = origin + t * direction;

    return glm::length2(closestPoint - target) <= (distance * distance);
}

// Get the distance to the nearest point on a sphere from an origin in a given direction, or nullopt if no such point exists.
inline std::optional<float> intersectRaySphere(glm::vec3 center, float radius, glm::vec3 origin, glm::vec3 direction)
{
    glm::vec3 toCenter = center - origin;

    float t = glm::dot(toCenter, direction);
    float halfChord2 = radius * radius - glm::length2(toCenter - t * direction);

    if (halfChord2 < 0.0f) {
        return std::nullopt;
    }

    float halfChord = std::sqrt(halfChord2);
    float nearest = t - halfChord >= 0.0f ? t - halfChord : t + halfChord;

    if (nearest < 0.0f) {
        return std::nullopt;
    }

    return nearest;
}

// Get the distance to the nearest point on an ellipsoid from an origin in a given direction, or nullopt if no such point exists.
inline std::optional<float> intersectRayEllipsoid(glm::vec3 radii, glm::vec3 origin, glm::vec3 direction)
{
    // Solve in the space where the ellipsoid is a unit sphere.
    glm::vec3 scaledOrigin = origin / radii;
    glm::vec3 scaledDirection = glm::normalize(direction / radii);

    std::optional<float> scaledDistance = intersectRaySphere(glm::vec3(0.0f), 1.0f, scaledOrigin, scaledDirection);
    if (!scaledDistance) {
        return std::nullopt;
    }

    glm::vec3 point = (scaledOrigin + *scaledDistance * scaledDirection) * radii;
    return glm::dot(point - origin, direction);
}


// Get the distance to the nearest point on a rectangle from an origin in a given direction, or nullopt if no such point exists.
inline std::optional<float> intersectRayRectangle(glm::vec2 halfExtents, glm::vec3 origin, glm::vec3 direction)
{
    if (std::abs(direction.y) < 1e-6f) {
        return std::nullopt;
    }

    float t = -origin.y / direction.y;
    if (t < 0.0f) {
        return std::nullopt;
    }

    glm::vec3 point = origin + t * direction;
    if (std::abs(point.x) > halfExtents.x || std::abs(point.z) > halfExtents.y) {
        return std::nullopt;
    }

    return t;
}

// Get the distance to the nearest point on a convex geometry from an origin in a given direction, or nullopt if no such point exists.
inline std::optional<float> intersectRayConvex(std::span<const glm::vec4> planes, glm::vec3 origin, glm::vec3 direction)
{
    float entryDistance = -std::numeric_limits<float>::infinity();
    float exitDistance = std::numeric_limits<float>::infinity();

    for (const glm::vec4& plane : planes)
    {
        glm::vec3 normal = glm::vec3(plane);
        float alignment = glm::dot(normal, direction);
        float planeDistance = plane.w - glm::dot(normal, origin);

        if (std::abs(alignment) < 1e-6f)
        {
            if (planeDistance < 0.0f) {
                return std::nullopt;
            }
            continue;
        }

        float t = planeDistance / alignment;
        if (alignment < 0.0f) {
            entryDistance = std::max(entryDistance, t);
        } else {
            exitDistance = std::min(exitDistance, t);
        }

        if (entryDistance > exitDistance) {
            return std::nullopt;
        }
    }

    if (exitDistance < 0.0f) {
        return std::nullopt;
    }

    return entryDistance >= 0.0f ? entryDistance : exitDistance;
}

// Get the distance to the nearest point on a cone from an origin in a given direction, or nullopt if no such point exists.
inline std::optional<float> intersectRayCone(float radius, float height, glm::vec3 origin, glm::vec3 direction)
{
    float nearest = std::numeric_limits<float>::infinity();

    auto considerSide = [&](float t) {
        float y = origin.y + t * direction.y;
        if (t >= 0.0f && y >= 0.0f && y <= height) {
            nearest = std::min(nearest, t);
        }
    };

    // The side surface satisfies x^2 + z^2 = (radius / height)^2 * (height - y)^2.
    float slope2 = (radius * radius) / (height * height);
    float toApex = height - origin.y;
    float a = direction.x * direction.x + direction.z * direction.z - slope2 * direction.y * direction.y;
    float b = 2.0f * (origin.x * direction.x + origin.z * direction.z + slope2 * toApex * direction.y);
    float c = origin.x * origin.x + origin.z * origin.z - slope2 * toApex * toApex;

    if (std::abs(a) > 1e-6f)
    {
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant >= 0.0f) {
            float root = std::sqrt(discriminant);
            considerSide((-b - root) / (2.0f * a));
            considerSide((-b + root) / (2.0f * a));
        }
    }
    else if (std::abs(b) > 1e-6f)
    {
        considerSide(-c / b);
    }

    if (std::abs(direction.y) > 1e-6f)
    {
        float t = -origin.y / direction.y;
        glm::vec3 point = origin + t * direction;
        if (t >= 0.0f && point.x * point.x + point.z * point.z <= radius * radius) {
            nearest = std::min(nearest, t);
        }
    }

    if (nearest == std::numeric_limits<float>::infinity()) {
        return std::nullopt;
    }

    return nearest;
}

// Get the distance to the nearest point on an infinite plane from an origin in a given direction, or nullopt if no such point exists.
inline std::optional<glm::vec3> intersectRayPlane(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 origin, glm::vec3 direction)
{
    float alignment = glm::dot(direction, planeNormal);

    if (std::abs(alignment) < 1e-6f) {
        return std::nullopt;
    }

    float t = glm::dot(planePoint - origin, planeNormal) / alignment;

    if (t < 0.0f) {
        return std::nullopt;
    }

    return origin + t * direction;
}