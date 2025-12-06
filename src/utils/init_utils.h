#ifndef INIT_H
#define INIT_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>


int initialize(GLFWwindow** windowPtr)
{
    // create GLFW window instance and set OpenGL context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "duGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window." << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    // load OpenGL function pointers with GLAD
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD." << std::endl;
        return -1;
    }

    (*windowPtr) = window;

    // set view port
    glViewport(0, 0, 800, 600);

    // enable depth testing with the z-buffer
    glEnable(GL_DEPTH_TEST);

    return 0;
}

#endif