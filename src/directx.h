#include "global.h"

class DirectX
{
private:
	IDirect3D9			*pDxObject;
	IDirect3DDevice9	*pDxDevice;
	HWND				hWnd;
public:
	DirectX() : pDxObject(NULL), pDxDevice(NULL) {};
	HRESULT Init(HWND);
	~DirectX();
	operator IDirect3DDevice9*() {return pDxDevice;}
};