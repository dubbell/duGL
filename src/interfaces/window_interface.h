#ifndef WINDOW_INTERFACE
#define WINDOW_INTERFACE

#include <GLFW/glfw3.h>

#include <utility>


class WindowInterface
{
public:
    virtual GLFWwindow* getWindow() = 0;
    virtual bool getFreeCursor() = 0;
    virtual void setFreeCursor(bool freeCursor) = 0;
    virtual std::pair<int, int> getViewportSize() = 0;

    virtual ~WindowInterface() = default;
};

#endif