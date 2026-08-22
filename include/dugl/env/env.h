#pragma once

#include "dugl/controllers/mouse.h"
#include "dugl/controllers/keyboard.h"
#include "dugl/interfaces/player_interface.h"
#include "dugl/view/camera.h"
#include "dugl/utils/glfw_include.h"
#include "dugl/utils/glad_include.h"

#include <memory>
#include <vector>


class Environment : public PlayerInterface
{
    template<typename T> using vector = std::vector<T>;
    template<typename T> using unique_ptr = std::unique_ptr<T>;
    template<typename T, typename U> using pair = std::pair<T, U>;

protected:
    GLFWwindow* window;
    int viewportWidth;
    int viewportHeight;

    vector<unique_ptr<Camera>> cameras;
    Camera* activeCamera;

    bool freeCursor;

    KeyboardController keyboardController;
    MouseController mouseController;

public:
    Environment();
    virtual ~Environment() = default;

    /// Starts the main loop of the environment.
    virtual void start() = 0;
    void stop();

    GLFWwindow* getWindow() override;
    pair<int, int> getViewportSize() override;
    Camera* getActiveCamera() override;

    bool getFreeCursor() override;
    void setFreeCursor(bool freeCursor) override;
    
protected:

    virtual void frameBufferResizeCallback(int width, int height) = 0;

private:
    /** Called by GLFW on window resize. */
    static void glfwFrameBufferResizeCallback(GLFWwindow* window, int width, int height);
};