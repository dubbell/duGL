#include "keyboard.h"


KeyboardController::KeyboardController()
    : _window(nullptr),
      observers(), 
      registeredKeys(), 
      keyboardState()
{}

KeyboardController::KeyboardController(GLFWwindow* window) 
    : _window(window),
      observers(), 
      registeredKeys(), 
      keyboardState()
{}

void KeyboardController::setWindow(GLFWwindow* window)
{
    _window = window;
}

void KeyboardController::registerKey(int key)
{
    registeredKeys.insert(key);
}

void KeyboardController::unregisterKey(int key)
{
    registeredKeys.erase(key);
}

void KeyboardController::registerObserver(std::shared_ptr<KeyboardControllable> observer)
{
    observers.insert(observer);
}

void KeyboardController::unregisterObserver(std::shared_ptr<KeyboardControllable> observer)
{
    observers.erase(observer);
}

void KeyboardController::processKeyboardInput()
{
    // get input
    for (int key : registeredKeys)
        keyboardState[key] = glfwGetKey(_window, key);

    // process input in observers
    for (auto& observer : observers)
        observer->processKeyboardInput(keyboardState);
}