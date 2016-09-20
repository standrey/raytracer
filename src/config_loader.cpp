#include "config_loader.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace std;

ConfigLoader::ConfigLoader(string configfile)
{
	configFilename = configfile;
	list<typeMaterial> rawMaterialsList = readSection(string("materials"));
	list<typeMaterial> rawLightsList = readSection(string("lights"));
	list<typeMaterial> rawSpheresList = readSection(string("spheres"));
	list<typeMaterial> rawCubemap = readSection(string("cubemap_tex"));

	if (rawMaterialsList.empty() || rawLightsList.empty() || rawSpheresList.empty())
	{
		MessageBox(NULL, L"Error while reading config.txt. Empty one of section material list", L"Error!", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	readRawMaterialsToStruct(rawMaterialsList);
	readRawSpheresToStruct(rawSpheresList);
	readRawLightsToStruct(rawLightsList);
	readRawCubemapToClass(rawCubemap);
}

void ConfigLoader::readRawSpheresToStruct(list<typeMaterial> & rawMaterialsList)
{
	for (list<typeMaterial>::iterator rawMaterial = rawMaterialsList.begin();
		rawMaterial != rawMaterialsList.end();
		rawMaterial++)
	{
		string name = (*rawMaterial).first;
		//values of structure
		stSphere oneSphere;
		for (list<typeParamValue>::iterator rawValues = (*rawMaterial).second.begin();
			rawValues != (*rawMaterial).second.end();
			rawValues++)
		{
			string nameOfValue = (*rawValues).first;
			if (nameOfValue == "positionx")
				oneSphere.position.x = (*rawValues).second;
			if (nameOfValue == "positiony")
				oneSphere.position.y = (*rawValues).second;
			if (nameOfValue == "positionz")
				oneSphere.position.z = (*rawValues).second;
			if (nameOfValue == "radius")
				oneSphere.radius = (*rawValues).second;
			if (nameOfValue == "materialid")
			{
				int materialId = (int)(*rawValues).second;
				if (materialId > 0 && materialId <= materialList.size())
				{
					oneSphere.MaterialId =  (int)(*rawValues).second - 1;
				}
				else
				{
					continue;
				}
			}
		}
		sphereList.push_back(oneSphere);
	}
}

void ConfigLoader::readRawLightsToStruct(list<typeMaterial> & rawMaterialsList)
{
	for (list<typeMaterial>::iterator rawMaterial = rawMaterialsList.begin();
		rawMaterial != rawMaterialsList.end();
		rawMaterial++)
	{
		string name = (*rawMaterial).first;
		//values of structure
		stLight oneLight;
		for (list<typeParamValue>::iterator rawValues = (*rawMaterial).second.begin();
			rawValues != (*rawMaterial).second.end();
			rawValues++)
		{
			string nameOfValue = (*rawValues).first;
			if (nameOfValue == "positionx")
				oneLight.position.x = (*rawValues).second;
			if (nameOfValue == "positiony")
				oneLight.position.y = (*rawValues).second;
			if (nameOfValue == "positionz")
				oneLight.position.z = (*rawValues).second;
			if (nameOfValue == "intensity")
				oneLight.intensity = (*rawValues).second;
		}
		this->lightList.push_back(oneLight);
	}
}

void ConfigLoader::readRawMaterialsToStruct(list<typeMaterial> & rawMaterialsList)
{
	for (list<typeMaterial>::iterator rawMaterial = rawMaterialsList.begin();
		rawMaterial != rawMaterialsList.end();
		rawMaterial++)
	{
		string name = (*rawMaterial).first;
		//values of structure
		stMaterial oneMaterial;
		for (list<typeParamValue>::iterator rawValues = (*rawMaterial).second.begin();
			rawValues != (*rawMaterial).second.end();
			rawValues++)
		{
			string nameOfValue = (*rawValues).first;
			if (nameOfValue == "colorr")
				oneMaterial.color.r = (*rawValues).second;
			if (nameOfValue == "colorg")
				oneMaterial.color.g = (*rawValues).second;
			if (nameOfValue == "colorb")
				oneMaterial.color.b = (*rawValues).second;
			oneMaterial.color.a = 1.0f;

			if (nameOfValue == "color2r")
				oneMaterial.color2.r = (*rawValues).second;
			if (nameOfValue == "color2g")
				oneMaterial.color2.g = (*rawValues).second;
			if (nameOfValue == "color2b")
				oneMaterial.color2.b = (*rawValues).second;
			oneMaterial.color2.a = 1.0f;

			if (nameOfValue == "specularpower")
				oneMaterial.specPow = (*rawValues).second;
			if (nameOfValue == "specularvalue")
				oneMaterial.specVal = (*rawValues).second;
			if (nameOfValue == "reflectioncoef")
				oneMaterial.reflectionCoef = (*rawValues).second;
			if (nameOfValue == "bumping")
				oneMaterial.bump = (*rawValues).second;
			if (nameOfValue == "turbulence")
				oneMaterial.turbulence = (bool)(*rawValues).second;
		}
		materialList.push_back(oneMaterial);
	}
}

void ConfigLoader::readRawCubemapToClass(list<typeMaterial> & rawMaterialsList)
{
	m_cubemap = std::unique_ptr<Cubemap>(new Cubemap());
	for (auto rawMaterial = rawMaterialsList.begin();
		rawMaterial != rawMaterialsList.end();
		rawMaterial++)
	{
		string name = (*rawMaterial).first;
		//values of structure
		stMaterial oneMaterial;
		for (list<typeParamValue>::iterator rawValues = (*rawMaterial).second.begin();
			rawValues != (*rawMaterial).second.end();
			rawValues++)
		{
			string nameOfValue = (*rawValues).first;
			if (nameOfValue == "bexposure")
				m_cubemap->set_bExposed((bool)(*rawValues).second);
			if (nameOfValue == "bsRGB")
				m_cubemap->set_bsRGB((bool)(*rawValues).second);
			if (nameOfValue == "exposure")
				m_cubemap->set_Exposure((*rawValues).second);
		}
	}
}

list<typeMaterial> ConfigLoader::readSection(string & sectionName)
{
	list<typeParamValue> listOfValues;
	list<typeMaterial> materials;

	ifstream config(configFilename);
	if (config.bad() || config.fail())
	{
		MessageBox(NULL, L"Failed to read config file config.txt", L"Error!", MB_OK | MB_ICONEXCLAMATION);
		return materials;
	}

	list<string> rawMaterials;

	string strFindSectionName = "[" + sectionName + "]";
	string sourceLine;
	string currentSection;

	while (!config.eof())
	{
		getline(config, sourceLine);

		if (sourceLine == "")
			continue;

		if (sourceLine[0] == '[' && sourceLine[sourceLine.length() - 1] == ']')
		{
			if (!currentSection.empty())
				break;

			if (sourceLine == strFindSectionName)
			{
				currentSection = strFindSectionName;
				continue;
			}
		}

		if (currentSection == strFindSectionName)
			rawMaterials.push_back(sourceLine);
	}

	config.close();

	if (!rawMaterials.empty())
	{

		int count_lines;
		for (	list<string>::iterator strMat = rawMaterials.begin();
				strMat != rawMaterials.end();)
		{
			list<typeParamValue> listOfValues;
			string nameOfMaterial = *strMat;
			strMat++;
			count_lines = 0;
			for (	list<string>::iterator line_nameval = strMat;
					line_nameval != rawMaterials.end();
					line_nameval++, strMat++)
			{
				int posEqual = (*line_nameval).find("=");
				if (posEqual <= 0)
					break;

				string name = (*line_nameval).substr(0, posEqual);
				string value = (*line_nameval).substr(posEqual+1);
				float val = stof(value);
				typeParamValue tmpval(name,val);
				listOfValues.push_back(tmpval);
			}

			typeMaterial tmp(nameOfMaterial, listOfValues);
			materials.push_back(tmp);
		}
	}

	return materials;
}

ConfigLoader::~ConfigLoader()
{
}

vector<stSphere> ConfigLoader::GetSpheres()
{
	return sphereList;
}

vector<stMaterial> ConfigLoader::GetMaterials()
{
	return materialList;
}

vector<stLight> ConfigLoader::GetLights()
{
	return lightList;
}