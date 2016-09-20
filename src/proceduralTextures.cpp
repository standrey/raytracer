#include "proceduralTextures.h"

Perlin ProceduralTextures::PerlinNoise = Perlin();

ProceduralTextures::ProceduralTextures()
{
	float ttt = 0;
}

ProceduralTextures::~ProceduralTextures()
{
	float ttt = 0;
}

D3DXCOLOR ProceduralTextures::Marble(const D3DXVECTOR3 & point)
{
	return D3DXCOLOR();
}

D3DXCOLOR ProceduralTextures::Turbulence(	const float & lambertCoeff, 
												const float & reflectCoeff, 
												const D3DXVECTOR3 & point,
												const D3DXCOLOR & color1,
												const D3DXCOLOR & color2,
												const float & lightIntensity)
{
	D3DCOLORVALUE resColorToAdd;
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

D3DXVECTOR3	ProceduralTextures::SimpePerlin	(	const float & bump, 
												const D3DXVECTOR3 & point, 
												const D3DXVECTOR3 & curNormal)
{
	D3DXVECTOR3 resNormal;
	float noiseCoefx = float(PerlinNoise.noise(0.2 * double(point.x), 0.2 * double(point.y), 0.2 * double(point.z)));
	float noiseCoefy = float(PerlinNoise.noise(0.2 * double(point.y), 0.2 * double(point.z), 0.2 * double(point.x)));
	float noiseCoefz = float(PerlinNoise.noise(0.2 * double(point.z), 0.2 * double(point.x), 0.2 * double(point.y)));
	resNormal.x = (1.0f - bump ) * curNormal.x + bump * noiseCoefx;
	resNormal.y = (1.0f - bump ) * curNormal.y + bump * noiseCoefy; 
	resNormal.z = (1.0f - bump ) * curNormal.z + bump * noiseCoefz; 
	return resNormal;
}
