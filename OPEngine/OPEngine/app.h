
#pragma once

class View;
class Camera;

class App
{
private:
    View* m_View;
    Camera* m_Camera;
public:
    App();
    ~App();
    void Init(int screen_width, int screen_height);
    void Update();
    void OnFrame();
    bool OnShouldClose();
    void BeginRender();
    void OnRender();
    void EndRender();
    void Shutdown();
    bool OnMsg(unsigned int msg, size_t param1, size_t param2, int& result);
    void OnInput(const char* utf8);
};