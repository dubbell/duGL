#pragma once

class MouseOffsetObserver
{
public:
    virtual void cursorOffsetCallback(float xOffset, float yOffset) = 0;
};