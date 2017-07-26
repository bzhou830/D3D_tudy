#include "AppFrame.h"
#include <Windows.h>
#include <assert.h>
#include <WinUser.h>

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

D3DAppAbs* g_d3dApp = 0;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_d3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DAppAbs::D3DAppAbs(HINSTANCE hInstance)
	: m_hAppInst(hInstance),
	m_MainWndCaption(L"Binean D3D"),
	m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	m_ClientWidth(800),
	m_ClientHeight(600),
	m_hMainWnd(NULL),
	m_d3dDevice(NULL),
	m_d3dImmediateContext(NULL),
	m_SwapChain(NULL),
	m_DepthStencilBuffer(0),
	m_RenderTargetView(0),
	m_DepthStencilView(0)
{
	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
	g_d3dApp = this;
}

D3DAppAbs::~D3DAppAbs()
{
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_SwapChain);
	ReleaseCOM(m_DepthStencilBuffer);

	if (m_d3dImmediateContext)
		m_d3dImmediateContext->ClearState();

	ReleaseCOM(m_d3dImmediateContext);
	ReleaseCOM(m_d3dDevice);
}

int D3DAppAbs::Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			DrawScene();
	}
	return (int)msg.wParam;
}

bool D3DAppAbs::Init()
{
	if (!InitMainWindow())
		return false;
	if (!InitDirect3D())
		return false;
	return true;
}

bool D3DAppAbs::InitMainWindow()
{
	// 设计窗口类
	WNDCLASSEX wc = { sizeof(wc), CS_VREDRAW | CS_HREDRAW, MainWndProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"CLASS_NAME", NULL };
	// 注册窗口类
	RegisterClassEx(&wc);
	// 创建窗口
	m_hMainWnd = CreateWindow(L"CLASS_NAME", m_MainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, 300, 400,
		m_ClientWidth, m_ClientHeight, NULL, NULL, wc.hInstance, NULL);
	//显示和更新窗口
	ShowWindow(m_hMainWnd, SW_SHOWDEFAULT);							
	UpdateWindow(m_hMainWnd);
	CenterWindow(m_hMainWnd); //窗口居中显示
	return true;
}

bool D3DAppAbs::InitDirect3D()
{
	HRESULT hr;
	UINT createDeviceFlags = 0;
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = m_ClientWidth;
	sd.BufferDesc.Height = m_ClientHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hMainWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		D3D_DRIVER_TYPE	g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_SwapChain, &m_d3dDevice, &m_featureLevel, &m_d3dImmediateContext);
		if (SUCCEEDED(hr)) break;
	}
	if (FAILED(hr))
		return hr;
	//第二步，创建Render Target View.
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Swap Chain GetBuffer Error.", 0, 0);
		return hr;
	}

	hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_RenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create Target View Error.", 0, 0);
		return hr;
	}
	m_d3dImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL);
	return true;
}

LRESULT D3DAppAbs::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3DAppAbs::CenterWindow(HWND hWnd)
{
	RECT rect, screen;
	GetWindowRect(hWnd, &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	screen.right = GetSystemMetrics(SM_CXSCREEN);
	screen.bottom = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(hWnd, NULL, (screen.right - rect.right) / 2, (screen.bottom - rect.bottom) / 2,
		0, 0, SWP_NOSIZE);
	return true;
}


void D3DAppAbs::SetWindowTitle(std::wstring str)
{
	SetWindowText(m_hMainWnd, str.c_str());
}
