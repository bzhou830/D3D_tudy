#include "AppFrame.h"
#include <assert.h>

//顶点的数据结构
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

//常量缓存的数据结构
struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

class DemoApp : public D3DAppAbs
{
public:
	DemoApp(HINSTANCE hInstance);
	~DemoApp();
	bool Init();
	void DrawScene();
};

DemoApp::DemoApp(HINSTANCE hInstance)
	: D3DAppAbs(hInstance)
{

}

DemoApp::~DemoApp()
{

}

bool DemoApp::Init()
{
	//在父类中执行窗口的创建和设备的创建
	if (!D3DAppAbs::Init())
		return false;
	// 第三步，创建一个视口
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)500;
	vp.Height = (FLOAT)500;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_d3dImmediateContext->RSSetViewports(1, &vp);

	return true;
}

void DemoApp::DrawScene()
{
	float ClearColor[4] = { 0.0f, 0.3f, 0.13f, 1.0f };//red, green, blue, alpha
	m_d3dImmediateContext->ClearRenderTargetView(m_RenderTargetView, ClearColor);

	m_SwapChain->Present(0, 0);
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	DemoApp theApp(hInstance);
	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

