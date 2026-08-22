#pragma once
#include "dugl/utils/glfw_include.h"

#include <utility>


class WindowInterface
{
public:
    virtual ~WindowInterface() = default;
    virtual GLFWwindow* getWindow() = 0;
    virtual bool getFreeCursor() = 0;
    virtual void setFreeCursor(bool freeCursor) = 0;
    virtual std::pair<int, int> getViewportSize() = 0;
};