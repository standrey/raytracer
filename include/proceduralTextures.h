#ifndef PROCEDURAL_TEXTURES_H
#define PROCEDURAL_TEXTURES_H

#include "perlin.h"
#include <glm/vec3.hpp>

class ProceduralTextures
{
private:
	static	Perlin				PerlinNoise;
public:
	static	glm::vec3			Marble		(const glm::vec3 & point);
	static	glm::vec3			Turbulence	(	const float & lambertCoeff, 
												const float & reflectCoeff, 
												const glm::vec3 & point,
												const glm::vec3 & color1,
												const glm::vec3 & color2,
												const float & lightIntensity);

	static	glm::vec3			SimpePerlin	(	const float & bump, 
												const glm::vec3 & point, 
												const glm::vec3 & curNormal);
								ProceduralTextures();
								~ProceduralTextures();
};

#endif  PROCEDURAL_TEXTURES_H
