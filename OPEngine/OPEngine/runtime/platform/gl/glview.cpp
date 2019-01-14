
#include "glview.h"
#include "app.h"

void framebuffer_size_callback_(GLFWwindow* window, int width, int height);
void processInput_(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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
    glfwSetCursorPosCallback(m_Window, mouse_callback);
    glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
    glfwSetScrollCallback(m_Window, scroll_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return;
}

void processInput_(GLFWwindow *window)
{
    Camera& camera = *App::Inst()->m_Camera;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float deltaTime = 0.0026f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback_(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 768;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isRightButtonDown = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!isRightButtonDown)
    {
        lastX = xpos;
        lastY = ypos;
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float scale = 1;
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    xoffset /= scale;
    yoffset /= scale;

    lastX = xpos;
    lastY = ypos;

    Camera& camera = *App::Inst()->m_Camera;
    camera.ProcessMouseMovement(xoffset, yoffset);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    switch(button)
    {
        case GLFW_MOUSE_BUTTON_RIGHT:
            {
                isRightButtonDown = action == GLFW_PRESS;
            }
            break;
        default:
            return;
    }
    return;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera& camera = *App::Inst()->m_Camera;
    camera.ProcessMouseScroll(yoffset);
}