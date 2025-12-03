#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(window, true);
    }
}


int main() {
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

    // set view port
    glViewport(0, 0, 800, 600);
    // update view port on window resize using callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shader("shaders/basic.vert", "shaders/basic.frag");

    // create buffers and VAO, multiple VBOs/EBOs can exist, but only one can be bound at a time
    
    // bind vertex array object, which stores vertex attribute configuration
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    
    // vertex buffer object, manages vertices GPU memory, 
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // element buffer object, manages
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    
    // data to render

    // // vertices for rectangle to render, to be stored in VBO
    // float vertices[] = {
    //     0.5f,  0.5f, 0.0f,   // top right
    //     0.5f, -0.5f, 0.0f,   // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f   // top left
    // };
    // // order in which vertices will be rendered, to be stored in EBO
    // unsigned int indices[] = {
    //     0, 1, 3,  // first triangle
    //     1, 2, 3   // second triangle
    // };

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  
    
    // bind VAO, subsequent VBO calls are stored in VAO
    glBindVertexArray(VAO); 

    // bind VBO and transfer data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // bind EBO and transfer data to GPU
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes pointers, this gets stored in VAO. First sets attr 0 to the position
    // which is three values, then enables attribute 0 to signal that it is being used.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // clear color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use(); // use shader program

        // set uniforms here if relevant

        glBindVertexArray(VAO);  // bind VAO
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // draw rectangle
        glDrawArrays(GL_TRIANGLES, 0, 3); // draw triangle
        glBindVertexArray(0); // unbind VAO (not necessary, but prevents incidental recording to VAO config)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}