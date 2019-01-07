
#include "glview.h"

void framebuffer_size_callback_(GLFWwindow* window, int width, int height);
void processInput_(GLFWwindow *window);

void GLView::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int GLView::windowShouldClose()
{
    return glfwWindowShouldClose(m_Window);
}

void GLView::swapBuffers()
{
    glfwSwapBuffers(m_Window);
}

void GLView::pollEvents()
{
    glfwPollEvents();
    processInput_(m_Window);
}

void GLView::handleTouchBegin(float x, float y)
{

}

void GLView::handleTouchMove(float x, float y)
{

}

void GLView::handleTouchEnd(float x, float y)
{

}

void GLView::createWindow(int w, int h)
{
    m_Window = glfwCreateWindow(w, h, "OPEngine", NULL, NULL);
    if (m_Window == NULL)
    {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback_);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return;
}

void processInput_(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback_(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
