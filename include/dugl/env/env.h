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
public:
    Environment();
    virtual ~Environment() = default;

    virtual void start() = 0;
    void stop();

    GLFWwindow* getWindow() override;
    std::pair<int, int> getViewportSize() override;
    Camera* getActiveCamera() override;

    bool getFreeCursor() override;
    void setFreeCursor(bool freeCursor) override;
    
protected:
    GLFWwindow* window;
    int viewportWidth;
    int viewportHeight;

    std::vector<std::unique_ptr<Camera>> cameras;
    Camera* activeCamera;

    bool freeCursor;
    
    KeyboardController keyboardController;
    MouseController mouseController;

    virtual void frameBufferResizeCallback(int width, int height) = 0;

private:
    /** Called by GLFW on window resize. */
    static void glfwFrameBufferResizeCallback(GLFWwindow* window, int width, int height);
};