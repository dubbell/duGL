#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <map>
#include <vector>
#include <set>
#include <functional>
#include <utility>
#include <GLFW/glfw3.h>

#include <keyboard_controllable.h>
#include "camera.h"


class KeyboardController
{
private:
    GLFWwindow* _window;

    std::set<std::shared_ptr<KeyboardControllable>> observers;
    std::set<int> registeredKeys;
    std::map<int, int> keyboardState;
    
public:
    KeyboardController();
    KeyboardController(GLFWwindow* window);

    void setWindow(GLFWwindow* window);

    void registerKey(int key);
    void unregisterKey(int key);

    void registerObserver(std::shared_ptr<KeyboardControllable> observer);
    void unregisterObserver(std::shared_ptr<KeyboardControllable> observer);

    void processKeyboardInput();
};


#endif