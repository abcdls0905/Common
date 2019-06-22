#include "Game.h"

Game* Game::m_GameInstance = nullptr;

Game::Game()
{

}

Game::~Game()
{

}

bool Game::GameLoop(float deltaTime)
{
	m_GameInstance->Update();
	m_GameInstance->Render();
	return true;
}

void Game::Update()
{

}

void Game::Render()
{
	m_Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1, 0);
	m_Device->Present(0, 0, 0, 0);
}

bool Game::Initialize(IDirect3DDevice9* device)
{
	this->m_Device = device;
	InitRenderData();
	return true;
}

void Game::InitRenderData()
{
	m_Device->CreateVertexBuffer(sizeof(SVertex) * 3, D3DUSAGE_WRITEONLY, SVertex::FVF, D3DPOOL_MANAGED, &m_VB, 0);
	SVertex* vertices;
	m_VB->Lock(0, sizeof(SVertex) * 3, (void**)&vertices, D3DLOCK_READONLY);
	vertices[0] = SVertex(-1.0f, 0.0f, 2.0f);
	vertices[1] = SVertex(0.0f, 1.0f, 2.0f);
	vertices[2] = SVertex(1.0f, 0.0f, 2.0f);
	m_VB->Unlock();
}

void Game::End()
{
	if (m_Device)
	{
		m_Device->Release();
		m_Device = nullptr;
	}
}

Game* Game::GetInstance()
{
	if (!m_GameInstance)
		m_GameInstance = new Game();
	return m_GameInstance;
}
