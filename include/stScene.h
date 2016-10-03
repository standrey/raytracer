#pragma once
#include <vector>
#include <memory>
#include "material.h"
#include "proceduralTextures.h"
#include "config_loader.h"


class stScene
{
private:
	ProceduralTextures texture;
	std::unique_ptr<ConfigLoader> objConfig;
	bool specularEnabled;
	bool reflectionEnabled;
	std::vector<stLight> listPointLights;
	std::vector<stSphere> listSpheres;
	std::vector<stMaterial> listMaterials;
	int stSceneSizeY;
	int stSceneSizeX;
	stRay ** rayArray;
	GLuint screenTexture;
	glm::vec3 ambientColor;
	void calcObjects();
	//LPD3DXFONT font;
	void drawFPS();
	//void calcFPS(int oldTime);
	//glm::vec3 readCubemap(stRay viewRay);
	//glm::vec3 readTexture(int, float u, float v, const D3DLOCKED_RECT&);
	glm::vec3 addRay(stRay viewRay);
	float FPS;
	int FrameCnt;
	unsigned int TimeElapsed;
	float AutoExposure();
	ScenePersp persp;
public:
	stScene(int width, int height);
	~stScene();
	void UpdateObjects();
	void DrawObjects();
	void KeyPress(unsigned int key);
};