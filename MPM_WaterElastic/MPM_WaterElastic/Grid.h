#pragma once
#include "Grid_point.h"
#include "Particle_cloud.h"
#include <vector>
#include <Dense>
using namespace Eigen;
using namespace std;
class Grid
{
public:
	Grid();
	~Grid();

public:
	vector<vector<Grid_Point>> Map;
	Vector2i size;
	Vector2d start, end;
	Vector2d cellsize;
	Particle_Cloud * Material;

public:
	Grid(Vector2d Start, Vector2d End, Vector2i Size, Particle_Cloud * Object);
	double N(double x);
	double dN(double x);	//the derivative of N(x)

	void Init_Grid();
	void Update_From_Material();
	void Init_Volume();

	void Update_Grid();
	void Compute_Weight_Gradient();
	void Set_Force_Zero();
	void Update_Force();
	void Update_From_Force();
	void Determine_Collision();


	void Update_Particle_Cloud();
	void Update_Particle_Position();
	void Update_Deformation_Gradient();
};

