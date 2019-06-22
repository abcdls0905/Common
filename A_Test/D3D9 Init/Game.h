#pragma once

struct IDirect3DDevice9;

struct SVertex
{
	SVertex(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	float x;
	float y;
	float z;
};

class Game
{
public:
	Game();
	~Game();
	static bool GameLoop(float deltaTime);
	void Update();
	void Render();
	bool Initialize(IDirect3DDevice9* device);
	void InitRenderData();
	void End();
	static Game* GetInstance();
private:
	IDirect3DVertexBuffer9* m_VB;
	IDirect3DIndexBuffer9* m_IB;
	IDirect3DDevice9* m_Device;
	static Game* m_GameInstance;
};
