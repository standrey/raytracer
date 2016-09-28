#pragma once

#include <string>
#include <fstream>
#include <list>
#include <vector>#include <memory>
#include <utility>
		
		
#include "material.h"

using namespace std;

typedef std::pair<string, float> typeParamValue;
typedef std::pair<string, list<typeParamValue>> typeMaterial;

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
