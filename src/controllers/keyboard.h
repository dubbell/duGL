#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <map>
#include <vector>
#include <set>
#include <functional>
#include <utility>
#include <GLFW/glfw3.h>

#include "keyboard_controllable.h"
#include "camera.h"


class KeyboardController
{
private:
    GLFWwindow* window;

    std::set<KeyboardControllable*> observers;
    std::set<int> registeredKeys;
    std::map<int, int> keyboardState;
    
public:
    KeyboardController(GLFWwindow* window);

    void registerKey(int key);
    void unregisterKey(int key);

    void registerKeyboardObserver(KeyboardControllable* observer);
    void unregisterKeyboardObserver(KeyboardControllable* observer);

    void processKeyboardInput();
};


#endif