
#pragma once

#include "../view.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLView : public View
{
private:
    GLFWwindow* m_Window;
public:
    virtual int windowShouldClose();
    virtual void init();
    virtual void createWindow(int w, int h);
    virtual void swapBuffers();
    virtual void pollEvents();
    virtual void handleTouchBegin(float x, float y);
    virtual void handleTouchMove(float x, float y);
    virtual void handleTouchEnd(float x, float y);
};