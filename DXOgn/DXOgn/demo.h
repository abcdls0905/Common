#pragma once


#include<d3d11.h>
#include<d3dx11.h>
#include<DxErr.h>
#include <d3dcompiler.h>
#include<xnamath.h>

class CDemo
{
	struct VertexPos
	{
		XMFLOAT3 position;
	};

public:
	CDemo();
	~CDemo();
	void Initialize(HWND hwnd);
	bool CompileShader(char* file, char* entry, char* shader_model, ID3DBlob** buffer);
	void Update();
	void Render();
	void ShutDown();
private:
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_target;
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;
	ID3D11Buffer* vertex_buffer;
};