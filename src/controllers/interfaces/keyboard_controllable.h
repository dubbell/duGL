#ifndef KEYBOARD_CONTROLLABLE_H
#define KEYBOARD_CONTROLLABLE_H

#include <map>

class KeyboardControllable
{
public:
    KeyboardControllable() {}

    virtual void processKeyboardInput(std::map<int, int> &keyboardState) = 0;
    virtual ~KeyboardControllable() = default;
};

#endif