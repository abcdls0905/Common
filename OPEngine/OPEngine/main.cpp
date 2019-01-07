
#include <iostream>
#include "app.h"
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 768;

int main()
{
    App app;
    app.Init(SCR_WIDTH, SCR_HEIGHT);
    while(!app.OnShouldClose())
    {
        app.Update();
    }
    return 0;
}
