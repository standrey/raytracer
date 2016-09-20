#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "global.h"
#include "material.h"

#include <string>
#include <fstream>
#include <list>
#include <vector>

using namespace std;

typedef pair<string, float> typeParamValue;
typedef pair<string, list<typeParamValue>> typeMaterial;

class ConfigLoader
{
public:
	ConfigLoader(string sourcefile);
	~ConfigLoader();
	vector<stSphere> GetSpheres();
	vector<stMaterial> GetMaterials();
	vector<stLight> GetLights();
	std::unique_ptr<Cubemap> m_cubemap;

private:
	ConfigLoader() {};

	list<typeMaterial> readSection(string & sectionName);
	string configFilename;

	vector<stMaterial> materialList;
	vector<stLight> lightList;
	vector<stSphere> sphereList;

	void readRawCubemapToClass(list<typeMaterial> & rawMaterialsList);
	void readRawLightsToStruct(list<typeMaterial> & rawMaterialsList);
	void readRawMaterialsToStruct(list<typeMaterial> & rawMaterialsList);
	void readRawSpheresToStruct(list<typeMaterial> & rawMaterialsList);
};

#endif