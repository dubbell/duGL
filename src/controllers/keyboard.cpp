#include "keyboard.h"


KeyboardController::KeyboardController()
    : window(nullptr),
      observers(),
      registeredKeys()
{}

KeyboardController::KeyboardController(GLFWwindow* window) 
    : window(window),
      observers(),
      registeredKeys()
{}

void KeyboardController::setWindow(GLFWwindow* window)
{
    this->window = window;
}

void KeyboardController::registerKey(int key)
{
    registeredKeys.insert(key);
}

void KeyboardController::unregisterKey(int key)
{
    registeredKeys.erase(key);
}

void KeyboardController::registerTogglableKey(int key)
{
    registeredTogglableKeys.insert({ key, false });
}

void KeyboardController::unregisterTogglableKey(int key)
{
    registeredTogglableKeys.erase(key);
}

void KeyboardController::registerSingleTriggerKey(int key)
{
    registeredSingleTriggerKeys.insert({ key, GLFW_RELEASE });

}

void KeyboardController::unregisterSingleTriggerKey(int key)
{
    registeredSingleTriggerKeys.erase(key);
}

void KeyboardController::registerObserver(std::shared_ptr<KeyboardControllable> observer)
{
    observers.insert(observer);

    for (int key : observer->getActiveKeys()) 
        registeredKeys.insert(key);
    
    for (int key : observer->getActiveTogglableKeys()) 
    {
        if (registeredTogglableKeys.find(key) == registeredTogglableKeys.end())
            registeredTogglableKeys.insert({ key, false });
    }

    for (int key : observer->getActiveSingleTriggerKeys()) 
    {
        if (registeredSingleTriggerKeys.find(key) == registeredSingleTriggerKeys.end())
            registeredSingleTriggerKeys.insert({ key, GLFW_RELEASE });
    }
}

void KeyboardController::unregisterObserver(std::shared_ptr<KeyboardControllable> observer)
{
    observers.erase(observer);
}

void KeyboardController::processKeyboardInput()
{
    KeyboardState keyboardState;

    // get key press input
    for (int key : registeredKeys)
        keyboardState.keys[key] = glfwGetKey(window, key);

    for (auto it = registeredTogglableKeys.begin(); it !=registeredTogglableKeys.end(); it++)
    {
        int key = it->first;
        int prevState = it->second;
        int nextState = glfwGetKey(window, key);

        if (prevState != nextState && nextState == GLFW_PRESS) 
            registeredTogglableKeys[key] = !registeredTogglableKeys[key];
        
        keyboardState.togglableKeys[key] = registeredTogglableKeys[key];
    }

    // get single trigger key input
    for (auto it = registeredSingleTriggerKeys.begin(); it != registeredSingleTriggerKeys.end(); it++)
    {
        int key = it->first;
        int prevState = it->second;
        int nextState = glfwGetKey(window, key);

        if (prevState == nextState) keyboardState.singleTriggerKeys[key] = SINGLE_TRIGGER_KEY_NO_CHANGE;
        else if (nextState == GLFW_RELEASE) keyboardState.singleTriggerKeys[key] = SINGLE_TRIGGER_KEY_RELEASE;
        else keyboardState.singleTriggerKeys[key] = SINGLE_TRIGGER_KEY_PRESS;

        registeredSingleTriggerKeys[key] = nextState;
    }

    // process input in observers
    for (auto& observer : observers)
        observer->processKeyboardInput(keyboardState);
}