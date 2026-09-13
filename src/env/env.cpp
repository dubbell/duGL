#include "dugl/env/env.h"
#include "dugl/utils/glfw_include.h"
#include "dugl/shading/ubo_templates.h"

#include <stdexcept>


Environment::Environment() 
    : viewportWidth(1920), viewportHeight(1080), freeCursor(false), mouseController(this), clearColor(0.7f, 0.8f, 1.0f, 1.0f)
{
    // window initialization
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // anti-aliasing MSAA (tell GLFW to create larger buffer)
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(viewportWidth, viewportHeight, "duGL", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD.");
    }

    glViewport(0, 0, viewportWidth, viewportHeight);

    keyboardController.setWindow(window);

    glfwSetFramebufferSizeCallback(window, Environment::glfwFrameBufferResizeCallback);
    glfwSetWindowUserPointer(window, this);

    // enable MSAA in OpenGL
    glEnable(GL_MULTISAMPLE);

    activeCamera = cameras.emplace_back(std::make_unique<Camera>()).get();
    activeCamera->setAspectRatio((float)16 / 9);

    // create uniform buffer object for perspective transforms
    perspectiveUbo.create("Perspective", scene.getShaders(), sizeof(PerspectiveData), GL_DYNAMIC_DRAW);
}

void Environment::stop()
{
    if (window != nullptr)
    {
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
    }
}

Environment::~Environment()
{
    // Deriving classes own GL resources (shaders, meshes, textures, ...) as members, which are
    // destroyed before this destructor body runs. The GL context must outlive that teardown, so
    // it's released here rather than earlier.
    stop();
}

void Environment::start()
{
    while (!glfwWindowShouldClose(window))
    {
        clearBuffers();

        // Process inputs.
        glfwPollEvents();                   // viewport resizing and GUI interaction
        mouseController.processInput();     // user mouse input
        keyboardController.processInput();  // user keyboard input

        // Process environment logic.
        float dt = stopwatch.tick();        // time since last frame
        update(dt);

        // Write data to the GPU (transforms, lighting, UBOs, ...)
        writeData();

        // Render the scene.
        scene.render();
        postRender();

        glfwSwapBuffers(window);
    }
}

GLFWwindow* Environment::getWindow()
{
    return window;
}

std::pair<int, int> Environment::getViewportSize()
{
    return { viewportWidth, viewportHeight };
}

Camera* Environment::getActiveCamera()
{
    return activeCamera;
}

bool Environment::getFreeCursor()
{
    return freeCursor;
}

void Environment::setFreeCursor(bool freeCursor)
{
    this->freeCursor = freeCursor;
}

Shader* Environment::createShader(const char* vertexShader, const char* fragmentShader)
{
    return scene.addShader(std::make_unique<Shader>(
        ("assets/shaders/" + std::string(vertexShader)).c_str(),
        ("assets/shaders/" + std::string(fragmentShader)).c_str()));
}

Renderable* Environment::createRenderable(RenderableBuilder& builder)
{
    return scene.addRenderable(std::make_unique<Renderable>(builder.build()));
}

void Environment::clearBuffers()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Environment::writeData()
{
    PerspectiveData perspectiveData = { activeCamera->getViewMatrix(), activeCamera->getProjectionMatrix() };
    perspectiveUbo.writeData(perspectiveData);
    for (auto& shader : scene.getShaders()) {
        if (shader->hasActiveUniform("viewPos")) {
            shader->setVec3("viewPos", activeCamera->getPosition());
        }
    }
}

void Environment::glfwFrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Environment* env = static_cast<Environment*>(glfwGetWindowUserPointer(window));
    env->viewportWidth = width;
    env->viewportHeight = height;
    env->frameBufferResizeCallback(width, height);
}