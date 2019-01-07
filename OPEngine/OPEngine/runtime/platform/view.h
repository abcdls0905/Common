
#pragma once

class View
{
public:
    virtual int windowShouldClose();
    virtual void swapBuffers();
    virtual void pollEvents();
    virtual void handleTouchBegin(float x, float y);
    virtual void handleTouchMove(float x, float y);
    virtual void handleTouchEnd(float x, float y);
    virtual void init();

    virtual void createWindow(int w, int h);
};