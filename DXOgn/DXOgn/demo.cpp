
#include "stdafx.h"
#include "demo.h"

#pragma comment(lib, "d3d11.lib") 
#pragma comment(lib, "d3dx11.lib") 
#pragma comment(lib, "d3dcompiler.lib") 

CDemo::CDemo()
{

}

CDemo::~CDemo()
{

}

void CDemo::Initialize(HWND hwnd)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	RECT demensions;
	GetClientRect(hwnd, &demensions);
	unsigned int width = demensions.right - demensions.left;
	unsigned int height = demensions.bottom - demensions.top;

	float PI = 3.1415926f;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*PI,
		(float)width / height, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	XMMATRIX projMatrix_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)width / height, 0.01f, 100.0f);
	projMatrix_ = XMMatrixTranspose(projMatrix_);
	XMStoreFloat4x4(&mProj, projMatrix_);

	XMMATRIX viewMatrix_ = XMMatrixIdentity();
	viewMatrix_ = XMMatrixTranspose(viewMatrix_);
	XMStoreFloat4x4(&mView, viewMatrix_);

	D3D_DRIVER_TYPE drive_types[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE, 
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE, 
		D3D_DRIVER_TYPE_SOFTWARE
	};

	unsigned int totle_drive_types = ARRAYSIZE(drive_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	unsigned int totle_feature_levels = ARRAYSIZE(feature_levels);
	
	DXGI_SWAP_CHAIN_DESC chain_desc;
	ZeroMemory(&chain_desc, sizeof(chain_desc));

	chain_desc.BufferCount = 1;
	chain_desc.BufferDesc.Width = width;
	chain_desc.BufferDesc.Height = height;
	chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	chain_desc.OutputWindow = hwnd;
	chain_desc.Windowed = true;
	chain_desc.SampleDesc.Count = 1;
	chain_desc.SampleDesc.Quality = 0;

	unsigned int create_flag = 0;
#ifdef _DEBUG
	create_flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel_;
	D3D_DRIVER_TYPE  device_type_;
	HRESULT result;
	for (int i = 0; i < totle_drive_types; ++i)
	{
		result = D3D11CreateDeviceAndSwapChain(0, drive_types[i], 0, create_flag, feature_levels, totle_feature_levels, \
			D3D11_SDK_VERSION, &chain_desc, &swap_chain, &device, &featureLevel_, &device_context);
		if (SUCCEEDED(result))
		{
			device_type_ = drive_types[i];
			break;
		}
	}

	ID3D11Texture2D* back_texture;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_texture);

	device->CreateRenderTargetView(back_texture, 0, &render_target);
	back_texture->Release();

	device_context->OMSetRenderTargets(1, &render_target, nullptr);

	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 0;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	device_context->RSSetViewports(1, &viewport);

	//initialize shader
	ID3DBlob* vsbuffer = nullptr;
	bool compile_result =	CompileShader("res/shader/standard.hlsl", "VS_Main", "vs_4_0", &vsbuffer);
	if (!compile_result) {
		return;
	}

	HRESULT d3dresult = device->CreateVertexShader(vsbuffer->GetBufferPointer(), vsbuffer->GetBufferSize(), 0, &vertex_shader);
	if (FAILED(d3dresult))
	{
		return;
	}

	D3D11_INPUT_ELEMENT_DESC desc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int totle_layout = ARRAYSIZE(desc);

	d3dresult = device->CreateInputLayout(desc, totle_layout, vsbuffer->GetBufferPointer(), vsbuffer->GetBufferSize(), &input_layout);

	vsbuffer->Release();
	if (FAILED(d3dresult)) {
		return;
	}

	ID3DBlob* psbuffer = 0;
	compile_result = CompileShader("res/shader/standard.hlsl", "PS_Main", "ps_4_0", &psbuffer);
	if (!compile_result) {
		return;
	}
	d3dresult = device->CreatePixelShader(psbuffer->GetBufferPointer(), psbuffer->GetBufferSize(), 0, &pixel_shader);
	if (FAILED(d3dresult)) {
		return;
	}
	psbuffer->Release();

	//vertice buffer
	VertexPos vertices[] =
	{ { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },

	{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC vertex_desc;
	ZeroMemory(&vertex_desc, sizeof(vertex_desc));
	vertex_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_desc.ByteWidth = sizeof(VertexPos) * ARRAYSIZE(vertices);
	
	D3D11_SUBRESOURCE_DATA resouce_data;
	ZeroMemory(&resouce_data, sizeof(resouce_data));
	resouce_data.pSysMem = vertices;

	d3dresult = device->CreateBuffer(&vertex_desc, &resouce_data, &vertex_buffer);
	if (FAILED(d3dresult)) {
		return;
	}

	//indice buffer
	UINT indices[] = {
		3,   1,  0,  2,  1,  3,
		6,   4,  5,  7,  4,  6,
		11,  9,  8, 10,  9, 11,
		14, 12, 13, 15, 12, 14,
		19, 17, 16, 18, 17, 19,
		22, 20, 21, 23, 20, 22
	};
	D3D11_BUFFER_DESC index_desc;
	ZeroMemory(&index_desc, sizeof(index_desc));
	index_desc.Usage = D3D11_USAGE_DEFAULT;
	index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_desc.ByteWidth = sizeof(UINT) * 36;
	index_desc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA index_data;
	ZeroMemory(&index_data, sizeof(index_data));
	index_data.pSysMem = indices;
	d3dresult = device->CreateBuffer(&index_desc, &index_data, &index_buffer);
	if (FAILED(d3dresult)) {
		return;
	}

	//texture
	d3dresult = D3DX11CreateShaderResourceViewFromFile(device, "res/tex/decal.dds", 0, 0, &shader_resourceview, 0);
	if (FAILED(d3dresult)) {
		return;
	}

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	d3dresult = device->CreateSamplerState(&sampler_desc, &sampler_state);
	if (FAILED(d3dresult)) {
		return;
	}

	//cb
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	bd.ByteWidth = sizeof(CBMVP);
	device->CreateBuffer(&bd, nullptr, &mvp_buffer);


	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(XMMATRIX);
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	device->CreateBuffer(&constDesc, 0, &viewCB_);
	device->CreateBuffer(&constDesc, 0, &projCB_);
	device->CreateBuffer(&constDesc, 0, &worldCB_);
}

bool CDemo::CompileShader(char* file, char* entry, char* shader_model, ID3DBlob** buffer)
{
	DWORD shader_flag = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
	shader_flag |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* errorbuffer = nullptr;
	HRESULT result = D3DX11CompileFromFile(file, 0, 0, entry, shader_model, shader_flag, 0, 0, buffer, &errorbuffer, 0);
	if (FAILED(result)) {
		return false;
	}

	if (errorbuffer) {
		errorbuffer->Release();
	}
	return true;
}

void CDemo::Update()
{
// 	XMVECTOR pos = XMVectorSet(0, 0, -6, 1.0f);
// 	XMVECTOR target = XMVectorZero();
// 	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
// 	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
// 	XMStoreFloat4x4(&mView, V);
// 
// 	XMMATRIX world = XMLoadFloat4x4(&mWorld);
// 	XMMATRIX view = XMLoadFloat4x4(&mView);
// 	XMMATRIX proj = XMLoadFloat4x4(&mProj);
// 	XMMATRIX worldViewProj = world*view*proj;


	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(0.0f, 0.7f, 0.7f);
	XMMATRIX translationMat = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	XMMATRIX worldMat = rotationMat * translationMat;
	worldMat = XMMatrixTranspose(worldMat);
	XMStoreFloat4x4(&mView, worldMat);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;	
	
	CBMVP cb_mvp;
	cb_mvp.mvp = worldViewProj;
	device_context->UpdateSubresource(mvp_buffer, 0, nullptr, &cb_mvp, 0, 0);
}

void CDemo::Render()
{
	float color[4] = { 0.25f, 0.25f, 0.25f, 1 };
	device_context->ClearRenderTargetView(render_target, color);

	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;

	device_context->IASetInputLayout(input_layout);
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_context->VSSetShader(vertex_shader, 0, 0);
	device_context->PSSetShader(pixel_shader, 0, 0);
	device_context->PSSetShaderResources(0, 1, &shader_resourceview);
	device_context->PSSetSamplers(0, 1, &sampler_state);
	//device_context->Draw(6, 0);
	//device_context->VSSetConstantBuffers(0, 1, &mvp_buffer);

	device_context->UpdateSubresource(worldCB_, 0, 0, &mWorld, 0, 0);
	device_context->UpdateSubresource(viewCB_, 0, 0, &mView, 0, 0);
	device_context->UpdateSubresource(projCB_, 0, 0, &mProj, 0, 0);

	device_context->VSSetConstantBuffers(0, 1, &worldCB_);
	device_context->VSSetConstantBuffers(1, 1, &viewCB_);
	device_context->VSSetConstantBuffers(2, 1, &projCB_);

	device_context->DrawIndexed(36, 0, 0);

	swap_chain->Present(0, 0);
}

void CDemo::ShutDown()
{
	device_context->Release();
	device->Release();
	swap_chain->Release();
}
