#ifndef KEYBOARD_CONTROLLABLE_H
#define KEYBOARD_CONTROLLABLE_H

#include <map>
#include <set>

#define SINGLE_TRIGGER_KEY_NO_CHANGE 0
#define SINGLE_TRIGGER_KEY_RELEASE 1
#define SINGLE_TRIGGER_KEY_PRESS 2

struct KeyboardState
{
    std::map<int, int> keys;
    std::map<int, bool> togglableKeys;
    std::map<int, int> singleTriggerKeys;
};

class KeyboardControllable
{
public:
    KeyboardControllable() {}

    virtual std::set<int> getActiveKeys() = 0;
    virtual std::set<int> getActiveTogglableKeys() = 0;
    virtual std::set<int> getActiveSingleTriggerKeys() = 0;

    virtual void processKeyboardInput(KeyboardState& keyboardState) = 0;
    virtual ~KeyboardControllable() = default;
};

#endif