#ifndef PERSPECTIVE_INTERFACE
#define PERSPECTIVE_INTERFACE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "view/camera.h"


class PerspectiveInterface
{
public:
    virtual Camera* getActiveCamera() = 0;

    virtual ~PerspectiveInterface() = default;
};

#endif