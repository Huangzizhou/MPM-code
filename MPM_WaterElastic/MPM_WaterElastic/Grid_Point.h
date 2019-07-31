#pragma once
#include <Dense>
using namespace Eigen;
class Grid_Point
{
public:
	Grid_Point();
	~Grid_Point();

public:

	double mass;					//mass
	bool active;
	Vector2d velocity;		//velocity
	Vector2d force;			//force
	Vector2d g;				//gravity
};

