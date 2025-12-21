#ifndef UBO_TEMPLATES_H
#define UBO_TEMPLATES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct PerspectiveData
{
    glm::mat4 view;
    glm::mat4 projection;
};

#endif