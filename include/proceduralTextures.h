#ifndef PROCEDURAL_TEXTURES_H
#define PROCEDURAL_TEXTURES_H

#include "perlin.h"

class ProceduralTextures
{
private:
	static	Perlin				PerlinNoise;
public:
	static	D3DXCOLOR			Marble		(const D3DXVECTOR3 & point);
	static	D3DXCOLOR			Turbulence	(	const float & lambertCoeff, 
												const float & reflectCoeff, 
												const D3DXVECTOR3 & point,
												const D3DXCOLOR & color1,
												const D3DXCOLOR & color2,
												const float & lightIntensity);

	static	D3DXVECTOR3			SimpePerlin	(	const float & bump, 
												const D3DXVECTOR3 & point, 
												const D3DXVECTOR3 & curNormal);
								ProceduralTextures();
								~ProceduralTextures();
};

#endif  PROCEDURAL_TEXTURES_H