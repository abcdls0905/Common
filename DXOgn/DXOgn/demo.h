#pragma once


#include<d3d11.h>
#include<d3dx11.h>
#include<DxErr.h>

class CDemo
{
public:
	CDemo();
	~CDemo();
	void Initialize(HWND hwnd);
	void Update();
	void Render();
	void ShutDown();
private:
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_target;
};