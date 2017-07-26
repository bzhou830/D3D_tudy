#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <d3dcompiler.h>
#include <string>

class D3DAppAbs
{
public:
	D3DAppAbs(HINSTANCE hInstance);
	virtual ~D3DAppAbs();
	int Run();
	void SetWindowTitle(std::wstring str);

	virtual bool Init();
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	bool InitMainWindow();
	bool InitDirect3D();

private:
	bool CenterWindow(HWND hWnd);

protected:
	HINSTANCE				m_hAppInst;
	HWND					m_hMainWnd;
	int						m_ClientWidth;
	int						m_ClientHeight;

	ID3D11Device*			m_d3dDevice;
	ID3D11DeviceContext*	m_d3dImmediateContext;
	IDXGISwapChain*			m_SwapChain;
	ID3D11Texture2D*		m_DepthStencilBuffer;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;
	D3D11_VIEWPORT			m_ScreenViewport;
	std::wstring			m_MainWndCaption;
	D3D_DRIVER_TYPE			m_d3dDriverType;
	D3D_FEATURE_LEVEL		m_featureLevel = D3D_FEATURE_LEVEL_11_0;
};
