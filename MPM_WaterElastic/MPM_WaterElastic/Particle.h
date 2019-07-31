#pragma once
#include <Dense>
#include <vector>
#include "Parameters.h"
using namespace std;
using namespace Eigen;
enum MaterialType { Water, Elastic };
class Particle
{
public:
	Particle();
	~Particle();
	Particle(Vector2d Pos, Vector2d Vel, double Mass, MaterialType tag, double Miu);
	void Compute_Energy_Derivative();
	void Update_Position();

public:
	double mass;
	double volume;
	double J;
	double miu;
	MaterialType Tag;
	Vector2d pseudo_position;
	Vector2i int_position;
	Vector2d position;
	Vector2d velocity;
	Vector2d momentum;
	Matrix2d Deformation_Gradient;
	Matrix4d weight;
	Matrix2d Energy_Derivative;
	Matrix2d Velocity_Gradient;
	Vector2d Weight_Gradient[4][4];
	Matrix2d B;
};

