#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>// glm::cross, glm::normalize
#include <GL/glut.h>

typedef struct {
	glm::vec3 position;
	float intensity;
} stLight;

typedef struct stRay {
	stRay() {};
	stRay(glm::vec3 pos, glm::vec3 dir) : startPoint(pos), direction(dir) {};

	glm::vec3 startPoint;
	glm::vec3 direction;
	glm::vec3 color;
} stRay;

typedef struct {
	float specPow;
	float specVal;
	float reflectionCoef;

	//it's diffuse color
	glm::vec4 color;
	//color2 use only for turbulence texture
	glm::vec4 color2;

	float bump;
	bool turbulence;
} stMaterial;

typedef struct {
	int MaterialId;
	glm::vec3 position;
	float radius;

	bool RayCollision(stRay r, float & distance)
	{
		const static float t = 2000;
		//first of all we must transform ray from world space to sphere space
		glm::vec3 rayPoint = r.startPoint - position;

		//calc codfficients
		float A = glm::dot(r.direction, r.direction);
		float B = 2 * glm::dot(rayPoint, r.direction);
		float C = glm::dot(rayPoint, rayPoint) - radius * radius;
		float discriminant = B * B - 4 * A * C;
		if (discriminant < 0)
			return false;
		float t0 = (-B - sqrtf(discriminant))/(2*A);
		float t1 = (-B + sqrtf(discriminant))/(2*A);

		if (t0 > 0.1 && t0 < t)
		{
			distance = t0;
			return true;
		}
		if (t1 > 0.1 && t1 < t)
		{
			distance = t1;
			return true;
		}
		return false;
	}
} stSphere;

class Cubemap{
public:
	Cubemap() : bExposed(false), bsRGB(false), exposure(1.0), texture(0) {};
	~Cubemap() {}
	GLuint texture;
	unsigned int *				bits;
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