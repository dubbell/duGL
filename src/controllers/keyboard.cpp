#include "keyboard.h"



KeyboardController::KeyboardController(GLFWwindow* window) 
    : window(window),
      observers(), 
      registeredKeys(), 
      keyboardState()
{}


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
        keyboardState[key] = glfwGetKey(window.get(), key);

    // process input in observers
    for (auto& observer : observers)
        observer->processKeyboardInput(keyboardState);
}