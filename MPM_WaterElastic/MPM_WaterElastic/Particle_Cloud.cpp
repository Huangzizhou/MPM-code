#include "Particle_Cloud.h"

Particle_Cloud::Particle_Cloud()
{
}


Particle_Cloud::~Particle_Cloud()
{
}

void Particle_Cloud::draw()
{
	glColor3f(0.1f, 0.1f, 1.0f);
	glPointSize(4);
	glBegin(GL_POINTS);

	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].Tag == Water)
		{
			glColor3f(0.051, 0.749, 0.984);
		}
		else
		{
			glColor3f(1,0,0);
		}
		glVertex2d(points[i].position[0], points[i].position[1]);
	}
	glEnd();
}

void Particle_Cloud::Compute_Energy_Derivative()
{
	for (int p = 0; p < points.size(); p++)
	{
		points[p].Compute_Energy_Derivative();
	}
}