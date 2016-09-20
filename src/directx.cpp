#include "directx.h"
#include <sstream>
#include <string>

HRESULT DirectX::Init(HWND hWnd)
{
	if (pDxObject != NULL) 
		return S_OK;
	HRESULT res = E_FAIL;
	pDxObject = Direct3DCreate9(D3D_SDK_VERSION);
	if (pDxObject == NULL)
	{
		MessageBox(NULL, L"Call to Direct3DCreate9() failed", L"Error!", MB_OK | MB_ICONEXCLAMATION);
		return GetLastError();
	}
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;
	pDxObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.Windowed = true;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	res = pDxObject->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pDxDevice);
	if (FAILED(res))
	{
		res = pDxObject->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_SW, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDxDevice);
		if (FAILED(res)) {
			std::wostringstream stream;
			stream << L"Call to CreateDevice() failed. Error code " << GetLastError();
			std::wstring msg = stream.str();
			MessageBox(NULL, msg.c_str(), L"Error!", MB_OK | MB_ICONEXCLAMATION);
			return GetLastError();
		}
	}
	return res;
}

DirectX::~DirectX()
{
	if (pDxDevice)
		pDxDevice->Release();
	if (pDxObject)
		pDxObject->Release();
}
