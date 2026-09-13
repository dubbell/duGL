#pragma once

#include "dugl/controllers/mouse.h"
#include "dugl/controllers/keyboard.h"
#include "dugl/interfaces/perspective_interface.h"
#include "dugl/view/camera.h"
#include "dugl/modelling/scene.h"
#include "dugl/utils/time.h"
#include "dugl/shading/uniform_buffer.h"
#include "dugl/async/dispatcher.h"

#include "dugl/utils/glfw_include.h"
#include "dugl/utils/glad_include.h"

#include <memory>
#include <vector>
#include <concepts>


// Base class for starting a duGL session. Initializes the GLFW window.
class Environment : public PerspectiveInterface
{
protected:
    GLFWwindow* window;
    int viewportWidth;
    int viewportHeight;

    dugl::AsyncDispatcher dispatcher;  // for concurrent work
    dugl::uint32 reservedJobs;         // number of jobs reserved for general async work

    std::vector<std::unique_ptr<Camera>> cameras;
    Camera* activeCamera;
    UniformBuffer perspectiveUbo;

    bool freeCursor;
    KeyboardController keyboardController;
    MouseController mouseController;

    // declared after the controllers so its entities (which may reference
    // them, e.g. HoverableEntity unregistering itself on destruction) are
    // destroyed first
    Scene scene;
    glm::vec4 clearColor;

    Stopwatch stopwatch;

public:
    Environment();
    virtual ~Environment();

    /* Starts the main environment loop. */
    void start();
    /* Terminates GLFW. */
    void stop();

    /* Executes once for every environment loop iteration, with time dt since last update. */
    virtual void update(float dt) {}

    GLFWwindow* getWindow() override;
    std::pair<int, int> getViewportSize() override;
    Camera* getActiveCamera() override;

    bool getFreeCursor() override;
    void setFreeCursor(bool freeCursor) override;
    
protected:
    /* Create a shader program with the provided vertex and fragment shaders. */
    Shader* createShader(const char* vertexShader, const char* fragmentShader);
    /* Create a renderable. */
    Renderable* createRenderable(RenderableBuilder& builder);

    /* Create an entity group. */
    template<typename T, typename... Args> requires std::is_base_of_v<EntityGroup, T>
    T* createEntityGroup(dugl::uint groupId, Args&&... args) {
        return static_cast<T*>(scene.addGroup(groupId, std::make_unique<T>(std::forward<Args>(args)...)));
    }

    /* Create a standard entity group. */
    StandardEntityGroup* createEntityGroup(dugl::uint groupId, Shader* shader) { 
        return createEntityGroup<StandardEntityGroup>(groupId, shader); 
    }

    /* Create an entity of the given type to be rendered in the environment scene. */
    template<typename T, typename... Args> requires std::is_base_of_v<Entity, T>
    T* createEntity(Renderable* renderable, Args&&... args) {
        return static_cast<T*>(scene.addEntity(std::make_unique<T>(renderable, std::forward<Args>(args)...)));
    }

    /* Create a standard entity to be rendered in the environment scene. */
    Entity* createEntity(Renderable* renderable) { return createEntity<Entity>(renderable); }
    
    /* Invoked before the main loop starts iterating. */
    virtual void init() {}

    /* Invoked after the scene is rendered. */
    virtual void postRender() {}

    /* Invoked when the viewport is resized. */
    virtual void frameBufferResizeCallback(int width, int height) {}

private:
    void updateEntities(float dt);

    void clearBuffers();
    void writeData();

    /* Called by GLFW on window resize. */
    static void glfwFrameBufferResizeCallback(GLFWwindow* window, int width, int height);
};