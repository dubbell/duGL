#include <app.h>


Application::Application(int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "duGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window." << std::endl;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD." << std::endl;
    }
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    camera.setAspectRatio(16.0f / 9.0f);

    keyboardController.setWindow(window);

    auto flightController = std::make_shared<FlightController>(window, &camera);

    mouseController.registerObserver(flightController);
    keyboardController.registerObserver(flightController);
    for (int key : flightController->getActiveKeys())
        keyboardController.registerKey(key);

    glfwSetFramebufferSizeCallback(window, Application::frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, Application::cursorPosCallback);
    glfwSetWindowUserPointer(window, this);

    Renderable model("assets/models/backpack/backpack.obj");
    renderables.push_back(model);
}

void Application::stop()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

void Application::startMainLoop()
{
    Shader shader = Shader("assets/shaders/basic_texture.vert", "assets/shaders/basic_texture.frag");

    DirectionalLight directionalLight = {
        {-0.2f, -1.0f, -0.3f},
        { 0.2f,  0.2f,  0.2f},
        { 0.5f,  0.5f,  0.5f},
        { 1.0f,  1.0f,  1.0f}
    };

    while (!glfwWindowShouldClose(window))
    {
        keyboardController.processKeyboardInput();

        glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 projectionMatrix = camera.getProjectionMatrix();

        shader.use();

        shader.setMat4("view", viewMatrix);
        shader.setMat4("projection", projectionMatrix);
        shader.setMat4("model", glm::mat4(1.0f));

        shader.setVec3("viewPos", camera.getPosition());
        
        shader.setVec3("directionalLight.direction", directionalLight.direction);
        shader.setVec3("directionalLight.ambient", directionalLight.ambient);
        shader.setVec3("directionalLight.diffuse", directionalLight.diffuse);
        shader.setVec3("directionalLight.specular", directionalLight.specular);
        
        for (auto& renderable : renderables)
        {
            renderable.draw(shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// void Application::startMainLoop()
// {
//     std::vector<unsigned int> activeTextures(32);

//     DirectionalLight directionalLight = {
//         {-0.2f, -1.0f, -0.3f},
//         { 0.2f,  0.2f,  0.2f},
//         { 0.5f,  0.5f,  0.5f},
//         { 1.0f,  1.0f,  1.0f}
//     };

//     std::vector<PointLight> pointLights = {{
//         {4.5f, 4.0f, 5.0f},
//         {0.6f, 0.6f, 0.3f},
//         {0.7f, 0.7f, 0.4f},
//         {1.0f, 1.0f, 1.0f},
//         1.0f, 0.09f, 0.032f}};

//     while (!glfwWindowShouldClose(window))
//     {
//         // process input
//         keyboardController.processKeyboardInput();

//         // clear buffers
//         glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         // perspective transformations
//         glm::mat4 viewMatrix = camera.getViewMatrix();
//         glm::mat4 projectionMatrix = camera.getProjectionMatrix();

//         unsigned int prevVAO = 0, prevVBO = 0;
//         Shader* prevShader = nullptr;

//         // loop through each VAO group
//         for (const auto& [VAO, VBOmap] : entityMap)
//         {
//             if (VAO != prevVAO) 
//             {
//                 glBindVertexArray(VAO);
//                 prevVAO = VAO;
//             }

//             // loop through each VBO group
//             for (const auto& [VBO, entities] : VBOmap)
//             {
//                 if (VBO != prevVBO) 
//                 {
//                     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//                     prevVBO = VBO;
//                 }

//                 // loop through and render all entities in group
//                 for (const auto entity : entities)
//                 {
//                     // set shader
//                     Shader* shader = entity.getShader();
//                     if (shader != prevShader)
//                     {
//                         // use new shader program
//                         shader->use();
//                         prevShader = shader;

//                         // set perspective transforms
//                         shader->setMat4("view", viewMatrix);
//                         shader->setMat4("projection", projectionMatrix);

//                         // set single directional light
//                         shader->setVec3("directionalLight.direction", directionalLight.direction);
//                         shader->setVec3("directionalLight.ambient", directionalLight.ambient);
//                         shader->setVec3("directionalLight.diffuse", directionalLight.diffuse);
//                         shader->setVec3("directionalLight.specular", directionalLight.specular);

//                         // number of point lights
//                         int pointLightCount = static_cast<int>(pointLights.size());
//                         shader->setInt("pointLightCount", pointLightCount);

//                         // loop through all point lights (max 8 for now)
//                         for (int pl_i = 0; pl_i < std::min(pointLightCount, 8); pl_i++)
//                         {
//                             auto& pointLight = pointLights[pl_i];
//                             shader->setVec3(std::format("pointLights[{}].position", pl_i), pointLight.position);
//                             shader->setVec3(std::format("pointLights[{}].ambient", pl_i), pointLight.ambient);
//                             shader->setVec3(std::format("pointLights[{}].diffuse", pl_i), pointLight.diffuse);
//                             shader->setVec3(std::format("pointLights[{}].specular", pl_i), pointLight.specular);
//                             shader->setFloat(std::format("pointLights[{}].constant", pl_i), pointLight.constant);
//                             shader->setFloat(std::format("pointLights[{}].linear", pl_i), pointLight.linear);
//                             shader->setFloat(std::format("pointLights[{}].quadratic", pl_i), pointLight.quadratic);
//                         }
                        
//                         // camera position
//                         shader->setVec3("viewPos", camera.getPosition());
//                     }

//                     // surface material (it is here instead because entities can have the same shader but might not share material)
//                     auto material = entity.getMaterial();
                    
//                     // set diffuse map
//                     shader->setInt("material.diffuse", 0);               // tell it to look for the diffuse map in GL_TEXTURE0
//                     glActiveTexture(GL_TEXTURE0);                        // make GL_TEXTURE0 the active texture unit
//                     glBindTexture(GL_TEXTURE_2D, material.diffuseMap);   // bind the unsigned int texture ID to the GL_TEXTURE0 2D target
//                     shader->setInt("material.diffuseFlag", 
//                                     material.diffuseMap == 0 ? 0 : 1);   // flag for whether the diffuse map should be rendered or not

//                     // set specular map
//                     shader->setInt("material.specular", 1);              // same as above but for GL_TEXTURE1
//                     glActiveTexture(GL_TEXTURE1);
//                     glBindTexture(GL_TEXTURE_2D, material.specularMap);
//                     shader->setInt("material.specularFlag", material.specularMap == 0 ? 0 : 1);
                    
//                     // set shininess
//                     shader->setFloat("material.shininess", material.shininess);

//                     // entity draw call
//                     entity.render();
//                 }
//             }
//         }

//         glBindVertexArray(0);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
        
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }
// }


void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->camera.setAspectRatio(float(width) / float(height));
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->mouseController.cursorPosCallback((float)xpos, (float)ypos);
}