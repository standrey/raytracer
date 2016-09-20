#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "global.h"

typedef struct {
	D3DXVECTOR3 position;
	float intensity;
} stLight;

typedef struct stRay {
	stRay() {};
	stRay(D3DXVECTOR3 pos, D3DXVECTOR3 dir) : startPoint(pos), direction(dir) {};

	D3DXVECTOR3 startPoint;
	D3DXVECTOR3 direction;
	D3DCOLORVALUE color;
} stRay;

typedef struct {
	FLOAT specPow;
	FLOAT specVal;
	FLOAT reflectionCoef;

	//it's diffuse color
	D3DXCOLOR color;
	//color2 use only for turbulence texture
	D3DXCOLOR color2;

	float bump;
	bool turbulence;
} stMaterial;

typedef struct {
	int MaterialId;
	D3DXVECTOR3 position;
	float radius;

	bool RayCollision(stRay r, float & distance)
	{
		const static float t = 2000;
		//first of all we must transform ray from world space to sphere space
		D3DXVECTOR3 rayPoint = r.startPoint - position;

		//calc codfficients
		float A = D3DXVec3Dot(&(r.direction), &(r.direction));
		float B = 2 * D3DXVec3Dot(&rayPoint, &(r.direction));
		float C = D3DXVec3Dot(&rayPoint, &rayPoint) - radius * radius;
		float discriminant = B * B - 4 * A * C;
		if (discriminant < 0)
			return FALSE;
		float t0 = (-B - sqrtf(discriminant))/(2*A);
		float t1 = (-B + sqrtf(discriminant))/(2*A);

		if (t0 > 0.1 && t0 < t)
		{
			distance = t0;
			return TRUE;
		}
		if (t1 > 0.1 && t1 < t)
		{
			distance = t1;
			return TRUE;
		}
		return FALSE;
	}
} stSphere;

class Cubemap{
public:
	Cubemap() : bExposed(false), bsRGB(false), exposure(1.0), texture(nullptr) {};
	~Cubemap() {if (texture!=nullptr) texture->Release();}
	IDirect3DTexture9 * texture;
	DWORD *				bits;
	int					h;
	int					w;
	enum {up = 0, down, right, left, forward, backward};
	//enum {right = 0, left, down, up, backward, forward};
	bool				get_bsRGB() {return bsRGB;};
	bool				get_bExposed() {return bExposed;};
	float				get_exposure() {return exposure;};
	void				set_bsRGB(bool v) {bsRGB = v;}
	void				set_bExposed(bool v) {bExposed = v;}
	void				set_Exposure(float v) {exposure = v;}
private:
	bool				bExposed;
	bool				bsRGB;
	float				exposure;
};

struct ScenePersp {
	float clearPoint;
	float dispersion;
	float FOV;
	enum typeDef {cubic = 0, conic};
	int typePersp;
};

#endif