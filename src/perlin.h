#ifndef _PERLIN_H
#define _PERLIN_H

class Perlin
{
private:
	int p[512];
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
public:
	double noise(double x, double y, double z);
	Perlin();
};

#endif