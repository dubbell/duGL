#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <app.h>


int main()
{
    Application* application = new Application(1920, 1080);

    application->startMainLoop();
    application->stop();

    return 0;
}