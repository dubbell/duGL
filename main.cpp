#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"



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

    Shader shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

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

    // vertices for rectangle to render, to be stored in VBO

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    // order in which vertices will be rendered, to be stored in EBO
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };

    // set interpolation type
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load texture image
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/textures/container.jpg", &width, &height, &nrChannels, 0);

    // generates texture reference in OpenGL
    unsigned int texture1;
    glGenTextures(1, &texture1);

    // bind texture so that subsequent calls apply changes to `texture`
    glActiveTexture(GL_TEXTURE0); // set texture unit to bind to
    glBindTexture(GL_TEXTURE_2D, texture1);

    // targets texture bound to GL_TEXTURE_2D
    // then mipmap level, image format, width/height, legacy setting, format and datatype of source image, actual image data
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture." << std::endl;
    }
    
    // once texture generated and data set, free image memory
    stbi_image_free(data);

    
    data = stbi_load("assets/textures/wall.jpg", &width, &height, &nrChannels, 0);
    
    unsigned int texture2;
    glGenTextures(1, &texture2);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture." << std::endl;
    }

    stbi_image_free(data);


    // rotation/scaling transform
    glm::mat4 trans1 = glm::mat4(1.0f), trans2;
    trans1 = glm::translate(trans1, glm::vec3(0.5f, -0.5f, 0.0f));
    // assign uniform location
    unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");

    
    // bind VAO, subsequent VBO calls are stored in VAO
    glBindVertexArray(VAO); 

    // bind VBO and transfer data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // bind EBO and transfer data to GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes pointers, this gets stored in VAO. First sets attr 0 to the position
    // which is three values, then enables attribute 0 to signal that it is being used.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    shader.use(); // use shader program
    shader.setInt("texture1", 0);  // set texture units
    shader.setInt("texture2", 1);

    
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        // clear color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // rotate and transfer matrix data
        trans2 = glm::rotate(trans1, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
        
        glBindVertexArray(VAO);  // bind VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // draw rectangle
        // glDrawArrays(GL_TRIANGLES, 0, 3); // draw triangle
        glBindVertexArray(0); // unbind VAO (not necessary, but prevents incidental recording to VAO config)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}