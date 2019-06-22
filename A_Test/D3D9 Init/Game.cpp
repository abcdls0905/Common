#include "Game.h"
#include "d3dUtility.h"

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
	//m_VB = new IDirect3DVertexBuffer9();
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
