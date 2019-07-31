#include "Grid_Point.h"



Grid_Point::Grid_Point()
{
	mass = 0;
	force = velocity = Vector2d::Zero();
	g = Vector2d(0.0, -1.0);
	active = 0;
}


Grid_Point::~Grid_Point()
{
}
