#ifndef MOUSE_CONTROLLABLE_H
#define MOUSE_CONTROLLABLE_H


class MouseControllable
{
public:
    virtual void cursorPosCallback(float xOffset, float yOffset) = 0;
};

#endif