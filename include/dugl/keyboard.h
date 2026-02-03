#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GLFW/glfw3.h>

#include <map>
#include <vector>
#include <set>
#include <functional>
#include <utility>
#include <memory>

#include "keyboard_observer.h"
#include "camera.h"


class KeyboardController
{
private:
    GLFWwindow* window;

    std::set<KeyboardObserver*> observers;

    std::set<int> registeredKeys;
    std::map<int, bool> registeredTogglableKeys;
    std::map<int, int> registeredSingleTriggerKeys;
    
public:
    KeyboardController();
    KeyboardController(GLFWwindow* window);

    void setWindow(GLFWwindow* window);

    void registerKey(int key);
    void unregisterKey(int key);

    void registerTogglableKey(int key);
    void unregisterTogglableKey(int key);

    void registerSingleTriggerKey(int key);
    void unregisterSingleTriggerKey(int key);

    void registerObserver(KeyboardObserver* observer);
    void unregisterObserver(KeyboardObserver* observer);

    void processKeyboardInput();
};


#endif