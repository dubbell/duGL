#ifndef COMMON_H
#define COMMON_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>



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

inline bool checkRayIntersection(glm::vec3 target, float distance, glm::vec3 origin, glm::vec3 direction)
{
    glm::vec3 toTarget = target - origin;

    float t = glm::dot(toTarget, direction);

    // ray is cast in wrong direction
    if (t < 0.0f) return false;

    glm::vec3 closestPoint = origin + t * direction;

    return glm::length2(closestPoint - target) <= (distance * distance);
}




#endif