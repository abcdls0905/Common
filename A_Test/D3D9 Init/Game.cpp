#include "Game.h"
#include "d3dUtility.h"

Game* Game::m_GameInstance = nullptr;

namespace d3d
{
    const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
    const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
    const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
    const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
    const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
    const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
    const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
    const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

}

Game::Game()
{

}

Game::~Game()
{

}

bool Game::GameLoop(float deltaTime)
{
	m_GameInstance->Update(deltaTime);
	m_GameInstance->Render(deltaTime);
	return true;
}

void Game::Update(float deltaTime)
{

}

bool Game::Initialize(IDirect3DDevice9* device)
{
	this->m_Device = device;
	InitRenderData();
	return true;
}

D3DLIGHT9 Game::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = d3d::WHITE;
	light.Specular = d3d::WHITE * 0.5f;
	light.Ambient = d3d::WHITE * 0.6f;
	light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    return light;
}

void Game::InitRenderData()
{
	m_Device->CreateVertexBuffer(sizeof(SVertex) * 12, D3DUSAGE_WRITEONLY, SVertex::FVF, D3DPOOL_MANAGED, &m_VB, 0);
	SVertex* vertices;
	m_VB->Lock(0, sizeof(SVertex) * 12, (void**)&vertices, D3DLOCK_READONLY);

    vertices[0] = SVertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f, 0, 0, d3d::BLUE);
    vertices[1] = SVertex( 0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f,  1, 0, d3d::BLUE);
    vertices[2] = SVertex( 1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f, 0, 1, d3d::BLUE);
    // left face
    vertices[3] = SVertex(-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f,  0, 0, d3d::BLUE);
    vertices[4] = SVertex( 0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f,  1, 0, d3d::BLUE);
    vertices[5] = SVertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f, 0, 1, d3d::BLUE);
    // right face
    vertices[6] = SVertex( 1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f,  0, 0, d3d::BLUE);
    vertices[7] = SVertex( 0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f,   1, 0, d3d::BLUE);
    vertices[8] = SVertex( 1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f,   0, 1, d3d::BLUE);
    // back face
    vertices[9] = SVertex( 1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f,   0, 0, d3d::BLUE);
    vertices[10] = SVertex( 0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f,  1, 0, d3d::BLUE);
    vertices[11] = SVertex(-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f,  0, 1, d3d::BLUE);

    m_VB->Unlock();

	D3DXCreateTextureFromFile(m_Device, "res/tex/dx5_logo.bmp", &m_Tex);
	m_Device->SetTexture(0, m_Tex);
	m_Device->SetTexture(1, m_Tex);
	m_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    D3DXMATRIX proj;
    float Width = 1360;
    float Height = 768;

    D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    m_Device->SetTransform(D3DTS_VIEW, &V);

    D3DXMatrixPerspectiveFovLH(
        &proj,
        D3DX_PI * 0.5f,
        (float)Width / (float)Height,
        1.0f,
        1000.0f);
    m_Device->SetTransform(D3DTS_PROJECTION, &proj);
    m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

    m_Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_Device->SetRenderState(D3DRS_SPECULARENABLE, true);
	m_Device->SetRenderState(D3DRS_LIGHTING, true);

    D3DMATERIAL9 mtrl;
    mtrl.Ambient = d3d::WHITE;
    mtrl.Diffuse = d3d::WHITE;
    mtrl.Specular = d3d::WHITE;
    mtrl.Emissive = d3d::WHITE;
    mtrl.Power = 5.0f;

    m_Device->SetMaterial(&mtrl);


	D3DXVECTOR3 dir(0, 1, 0);
	D3DXCOLOR c = d3d::RED;
	D3DLIGHT9 dirLight = InitDirectionalLight(&dir, &c);

    m_Device->SetLight(0, &dirLight);
    m_Device->LightEnable(0, true);
}

void Game::Render(float deltaTime)
{
    D3DXMATRIX yRot;

    static float y = 0.0f;

    D3DXMatrixRotationY(&yRot, y);
    y += deltaTime;

    if( y >= 6.28f )
        y = 0.0f;

    m_Device->SetTransform(D3DTS_WORLD, &yRot);

    m_Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1, 0);
    m_Device->BeginScene();
    m_Device->SetStreamSource(0, m_VB, 0, sizeof(SVertex));
    m_Device->SetFVF(SVertex::FVF);
    m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);
    m_Device->EndScene();
    m_Device->Present(0, 0, 0, 0);
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
