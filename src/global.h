#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <windows.h>

#include <d3dx9.h>
#include <dxerr.h>
#include <d3dx9core.h>

#include <conio.h> 

#ifdef _DEBUG
	#undef _DEBUG
		#include <omp.h>
	#define _DEBUG
#else
	#include <omp.h>
#endif


#if defined(DEBUG)|defined(_DEBUG)
	#ifndef D3D_DEBUG_INFO
	#define D3D_DEBUG_INFO
	#endif
#endif

#pragma comment(lib,"DxErr.lib")
#pragma comment(lib,"d3d9.lib")

#if defined(DEBUG)|defined(_DEBUG)
#pragma comment(lib,"d3dx9d.lib")
#endif

#if defined(RELEASE)|defined(_RELEASE)
#pragma comment(lib,"d3dx9.lib")
#endif

static HWND hWnd = NULL;

#endif