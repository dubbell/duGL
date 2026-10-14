#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <cmath>
#include <optional>


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

inline std::optional<glm::vec3> intersectRaySphere(glm::vec3 center, float radius, glm::vec3 origin, glm::vec3 direction)
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

    return origin + nearest * direction;
}

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