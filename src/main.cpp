#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "init_utils.h"
#include "camera.h"
#include "keyboard.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


int main() {
    
    GLFWwindow* window;
    initialize(&window);

    // update view port on window resize using callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    glm::vec3 cameraStart(0.0f, 0.0f, 0.0f);
    Camera* camera = new Camera(cameraStart, 1.0f);

    KeyboardController* keyboardController = new KeyboardController(window);
    keyboardController->registerKeyboardObserver(camera);

    keyboardController->registerKey(GLFW_KEY_W);
    keyboardController->registerKey(GLFW_KEY_A);
    keyboardController->registerKey(GLFW_KEY_S);
    keyboardController->registerKey(GLFW_KEY_D);


    glfwSetWindowUserPointer(window, keyboardController);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        KeyboardController* controller = static_cast<KeyboardController*>(glfwGetWindowUserPointer(window));
    });

    // create buffers and VAO, multiple VBOs/EBOs can exist, but only one can be bound at a time
    
    // bind vertex array object, which stores vertex attribute configuration
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    
    // vertex buffer object ref, manages vertices in GPU memory, 
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // element buffer object ref, manages indices
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    
    // data to render

    float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    
    // bind VAO, subsequent VBO calls are stored in VAO
    glBindVertexArray(VAO); 

    // bind VBO and transfer data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    // bind EBO and transfer data to GPU
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes pointers, this gets stored in VAO. First sets attr 0 to the position
    // which is three values, then enables attribute 0 to signal that it is being used.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader.use(); // use shader program
    // shader.setMatrix("model", model);
    shader.setMatrix("projection", projection);
    
    
    
    while (!glfwWindowShouldClose(window)) {
        keyboardController->processKeyboardInput();
        
        // clear color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);  // bind VAO

        shader.setMatrix("view", camera->getView());

        int i = 0;
        for (auto pos : cubePositions)
        {
            auto cubeModel = glm::translate(model, pos);
            shader.setMatrix(
                "model", 
                glm::rotate(
                    cubeModel, 
                    glm::radians(20.0f * i), 
                    glm::vec3(1.0f, 0.3f, 0.5f)));
            // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // draw rectangle
            glDrawArrays(GL_TRIANGLES, 0, 36); 
            i++;
        }
        
        glBindVertexArray(0); // unbind VAO (not necessary, but prevents incidental recording to VAO config)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}