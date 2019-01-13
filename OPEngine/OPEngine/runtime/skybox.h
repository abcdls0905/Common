
#pragma once

#include "glad/glad.h"
#include <stb_image.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class SkyBox
{
public:
public:
    SkyBox();
    ~SkyBox();
    void Initialize();
    void Render();
};