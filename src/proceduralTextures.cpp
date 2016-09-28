#include "proceduralTextures.h"
#include <math.h>

Perlin ProceduralTextures::PerlinNoise = Perlin();

ProceduralTextures::ProceduralTextures()
{
	float ttt = 0;
}

ProceduralTextures::~ProceduralTextures()
{
	float ttt = 0;
}

glm::vec3 ProceduralTextures::Marble(const glm::vec3 & point)
{
	return glm::vec3();
}

glm::vec3 ProceduralTextures::Turbulence(	const float & lambertCoeff, 
												const float & reflectCoeff, 
												const glm::vec3 & point,
												const glm::vec3 & color1,
												const glm::vec3 & color2,
												const float & lightIntensity)
{
	glm::vec3 resColorToAdd;
	float noiseCoef = 0;

	for (int levelTurb = 1; levelTurb < 10; levelTurb ++)
		noiseCoef += (1.0f / levelTurb ) * fabsf(float(PerlinNoise.noise(levelTurb * 0.05 * point.x, levelTurb * 0.05 * point.y, levelTurb * 0.05 * point.z)));

	resColorToAdd.r =	reflectCoeff * lambertCoeff * lightIntensity * 
						(noiseCoef * color1.r + (1.0f - noiseCoef) * color2.r);
	resColorToAdd.g =	reflectCoeff * lambertCoeff * lightIntensity * 
						(noiseCoef * color1.g + (1.0f - noiseCoef) * color2.g);
	resColorToAdd.b =	reflectCoeff * lambertCoeff * lightIntensity * 
						(noiseCoef * color1.b + (1.0f - noiseCoef) * color2.b);
	return resColorToAdd;
}

glm::vec3	ProceduralTextures::SimpePerlin	(	const float & bump, 
												const glm::vec3 & point, 
												const glm::vec3 & curNormal)
{
	glm::vec3 resNormal;
	float noiseCoefx = float(PerlinNoise.noise(0.2 * double(point.x), 0.2 * double(point.y), 0.2 * double(point.z)));
	float noiseCoefy = float(PerlinNoise.noise(0.2 * double(point.y), 0.2 * double(point.z), 0.2 * double(point.x)));
	float noiseCoefz = float(PerlinNoise.noise(0.2 * double(point.z), 0.2 * double(point.x), 0.2 * double(point.y)));
	resNormal.x = (1.0f - bump ) * curNormal.x + bump * noiseCoefx;
	resNormal.y = (1.0f - bump ) * curNormal.y + bump * noiseCoefy; 
	resNormal.z = (1.0f - bump ) * curNormal.z + bump * noiseCoefz; 
	return resNormal;
}
