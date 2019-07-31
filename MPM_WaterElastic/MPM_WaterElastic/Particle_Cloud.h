#pragma once
#include "Particle.h"
#include <vector>
#include <glut.h>

using namespace std;
class Particle_Cloud
{
public:
	Particle_Cloud();
	~Particle_Cloud();

	void Compute_Energy_Derivative();
	void draw();

public:
	vector<Particle> points;
};

