#include "global.h"
#include "material.h"
#include "proceduralTextures.h"
#include "config_loader.h"
#include <vector>

class stScene
{
private:
	ProceduralTextures texture;
	unique_ptr<ConfigLoader> objConfig;
	bool specularEnabled;
	bool reflectionEnabled;
	std::vector<stLight> listPointLights;
	std::vector<stSphere> listSpheres;
	std::vector<stMaterial> listMaterials;
	INT stSceneSizeY;
	INT stSceneSizeX;
	stRay ** rayArray;
	IDirect3DTexture9 * screenTexture;
	IDirect3DDevice9  * d3ddev;
	IDirect3DSurface9 * d3dRenderSurface;
	D3DCOLORVALUE ambientColor;
	void calcObjects();
	LPD3DXFONT font;
	void drawFPS();
	void calcFPS(DWORD oldTime);
	D3DCOLORVALUE readCubemap(stRay viewRay);
	D3DCOLORVALUE readTexture(int, float u, float v, const D3DLOCKED_RECT&);
	D3DXCOLOR addRay(stRay viewRay);
	float FPS;
	int FrameCnt;
	DWORD TimeElapsed;
	float AutoExposure();
	ScenePersp persp;
public:
	stScene(IDirect3DDevice9 * dev, INT width, INT height);
	~stScene();
	void UpdateObjects();
	void DrawObjects();
	void KeyPress(UINT key);
};