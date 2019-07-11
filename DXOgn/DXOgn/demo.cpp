
#include "stdafx.h"
#include "demo.h"

CDemo::CDemo()
{

}

CDemo::~CDemo()
{

}

void CDemo::Initialize(HWND hwnd)
{
	RECT demensions;
	GetClientRect(hwnd, &demensions);
	unsigned int width = demensions.right - demensions.left;
	unsigned int height = demensions.bottom - demensions.top;

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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	//initialize vertice
	VertexPos vertices[] =
	{
		XMFLOAT3(0.5f, 0.5f, 0.5f),
		XMFLOAT3(0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, -0.5f, 0.5f)
	};
	D3D11_BUFFER_DESC vertex_desc;
	ZeroMemory(&vertex_desc, sizeof(vertex_desc));
	vertex_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_desc.ByteWidth = sizeof(VertexPos) * 3;
	
	D3D11_SUBRESOURCE_DATA resouce_data;
	ZeroMemory(&resouce_data, sizeof(resouce_data));
	resouce_data.pSysMem = vertices;

	d3dresult = device->CreateBuffer(&vertex_desc, &resouce_data, &vertex_buffer);
	if (FAILED(d3dresult)) {
		return;
	}
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

}

void CDemo::Render()
{
	float color[4] = { 0.25f, 0, 0.25f, 1 };
	device_context->ClearRenderTargetView(render_target, color);

	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;

	device_context->IASetInputLayout(input_layout);
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_context->VSSetShader(vertex_shader, 0, 0);
	device_context->PSSetShader(pixel_shader, 0, 0);
	device_context->Draw(3, 0);

	swap_chain->Present(0, 0);
}

void CDemo::ShutDown()
{
	device_context->Release();
	device->Release();
	swap_chain->Release();
}
