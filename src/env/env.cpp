#include "dugl/env/env.h"
#include "dugl/utils/glfw_include.h"
#include "dugl/shading/ubo_templates.h"
#include "dugl/async/job.h"

#include <algorithm>
#include <stdexcept>


static constexpr dugl::uint32 MAX_JOBS = 1024;
static constexpr dugl::uint32 MAX_BARRIERS = 8;

Environment::Environment() 
    : viewportWidth(1920), viewportHeight(1080), dispatcher(MAX_JOBS, MAX_BARRIERS), reservedJobs(MAX_JOBS),
      freeCursor(false), mouseController(this), clearColor(0.7f, 0.8f, 1.0f, 1.0f)
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

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

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
    init();

    while (!glfwWindowShouldClose(window))
    {
        clearBuffers();

        // Process inputs.
        glfwPollEvents();                   // viewport resizing and GUI interaction
        mouseController.processInput();     // user mouse input
        keyboardController.processInput();  // user keyboard input

        // Process environment logic.
        float dt = stopwatch.tick();
        update(dt);
        updateEntities(dt);

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

void Environment::updateEntities(float dt)
{
    std::vector<Entity*> entities = scene.getEntities();
    if (entities.empty()) {
        return;
    }

    dugl::uint32 numJobs = std::min<dugl::uint32>(reservedJobs, dispatcher.getNumThreads() * 4);
    numJobs = std::max<dugl::uint32>(1, std::min<dugl::uint32>(numJobs, (dugl::uint32)entities.size()));

    // Add largest possible remainder numJobs-1 so that last batch of entities are not discarded.
    size_t entitiesPerJob = (entities.size() + numJobs - 1) / numJobs;

    std::vector<std::unique_ptr<dugl::EntityUpdateJob>> jobs;
    dugl::JobBarrier* barrier = dispatcher.createBarrier();

    for (dugl::uint32 job_i = 0; job_i < numJobs; job_i++)
    {
        size_t begin = job_i * entitiesPerJob;
        size_t end = std::min(begin + entitiesPerJob, entities.size());
        if (begin >= end) {
            break;
        }

        auto& job = jobs.emplace_back(std::make_unique<dugl::EntityUpdateJob>(
            dt, std::vector<Entity*>(entities.begin() + begin, entities.begin() + end)));
        dispatcher.submitJob("EntityUpdate", job.get(), barrier);
    }

    dispatcher.waitAndDestroyBarrier(barrier);
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