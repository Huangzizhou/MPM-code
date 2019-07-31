#include "Grid.h"
#include <math.h>
#include <iostream>
#include <stdlib.h>
const double BSPLINE_EPSILON = 1e-12;

Grid::Grid()
{
}

Grid::~Grid()
{
	delete Material;
}

Grid::Grid(Vector2d Start, Vector2d End, Vector2i Size, Particle_Cloud * Object)
{
	start = Start;
	end = End;
	size = Size;
	cellsize[0] = (end[0] - start[0]) / (double)(size[0] - 1);
	cellsize[1] = (end[1] - start[1]) / (double)(size[1] - 1);
	Map.resize(Size[0]);
	for (int i = 0; i < Size[0]; i++)
	{
		Map[i].resize(Size[1]);
	}
	Material = Object;
}

void Grid::Set_Force_Zero()
{
	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			Map[i][j].force = Vector2d::Zero();
		}
	}
}

double Grid::N(double x)
{
	double N_x;
	x = fabs(x);
	if (x < 1)
		N_x = x * x * (x / 2 - 1) + 2 / 3.0;
	else if (x < 2)
		N_x = x * (x * (-x / 6 + 1) - 2) + 4 / 3.0;
	else
		N_x = 0.0;

	return N_x;
}

double Grid::dN(double x)
{
	double abs_x = fabs(x);
	if (abs_x < 1)
		return 1.5*x*abs_x - 2 * x;
	else if (x < 2)
		return -x * abs_x / 2 + 2 * x - 2 * x / abs_x;
	else return 0;
}

void Grid::Update_From_Material()
{
	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			Map[i][j].mass = 0;
			Map[i][j].velocity = Vector2d::Zero();
			Map[i][j].active = false;
		}
	}
	for (int p = 0; p < Material->points.size(); p++)
	{
		Particle& P = Material->points[p];
		double x = P.position[0], y = P.position[1];
		x = (x - start[0]) / cellsize[0], y = (y - start[1]) / cellsize[1];
		P.pseudo_position = Vector2d(x, y);
		int u = (int)floor(x), v = (int)floor(y);
		P.int_position = Vector2i(u, v);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				P.weight(i, j) = N(i + u - x - 1) * N(j + v - y - 1);
				if (P.weight(i, j) > BSPLINE_EPSILON)
					Map[i + u - 1][j + v - 1].active = true;
				Map[i + u - 1][j + v - 1].mass += P.weight(i, j) * P.mass;
				Eigen::Vector2d position = Eigen::Vector2d(start[0] + cellsize[0] * (i + u - 1), start[1] + cellsize[1] * (j + v - 1));
				Map[i + u - 1][j + v - 1].velocity += P.weight(i, j) * P.mass*(P.velocity + 3.0 / cellsize[0] / cellsize[1] * P.B*(position - P.position));
				//Map[i + u - 1][j + v - 1].velocity += P.weight(i, j) * P.velocity * P.mass;
			}
		}
	}
	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			if (Map[i][j].active)
				Map[i][j].velocity /= Map[i][j].mass;

			else Map[i][j].velocity = Vector2d(0, 0);
		}
	}
}

void Grid::Init_Volume()
{
	for (int p = 0; p < Material->points.size(); p++)
	{
		Particle& P = Material->points[p];
		double x = P.pseudo_position[0], y = P.pseudo_position[1];
		int u = P.int_position[0], v = P.int_position[1];
		double density = 0;
		P.volume = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				density += Map[u + i - 1][j + v - 1].mass*P.weight(i, j);
			}
		}
		density = density / (cellsize[0] * cellsize[1]);
		P.volume = P.mass / density;
	}
}

void Grid::Init_Grid()
{
	Update_From_Material();
	Init_Volume();
}

void Grid::Compute_Weight_Gradient()
{
	for (int p = 0; p < Material->points.size(); p++)
	{
		Particle& P = Material->points[p];
		double x = P.pseudo_position[0], y = P.pseudo_position[1];
		int u = P.int_position[0], v = P.int_position[1];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				P.Weight_Gradient[i][j] << dN(x - u - i + 1) * N(y - v - j + 1) / cellsize[0],
					N(x - u - i + 1) * dN(y - v - j + 1) / cellsize[1];
			}
		}
	}
}

void Grid::Update_Force()
{
	Set_Force_Zero();
	for (int p = 0; p < Material->points.size(); p++)
	{
		Particle& P = Material->points[p];
		double x = P.pseudo_position[0], y = P.pseudo_position[1];
		int u = P.int_position[0], v = P.int_position[1];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Map[u + i - 1][v + j - 1].force = Map[u + i - 1][v + j - 1].force - P.volume * P.Energy_Derivative * P.Weight_Gradient[i][j];
			}
		}
	}
}

void Grid::Update_From_Force()
{
	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			if (Map[i][j].active)
			{
				Map[i][j].velocity = Map[i][j].velocity * Map[i][j].mass + (Map[i][j].force  + Map[i][j].g * Map[i][j].mass) * DT;
				Map[i][j].velocity /= Map[i][j].mass;
			}
			else Map[i][j].velocity = Vector2d(0, 0);
		}
	}
}

void Grid::Determine_Collision()
{
	for (int i = 0; i < size[0]; i++)
	{
		for (int j = 0; j < size[1]; j++)
		{
			double y = j + Map[i][j].velocity[1] * DT / size[1];
			double x = i + Map[i][j].velocity[0] * DT / size[0];
			if (y < 3 || y > 98)
				Map[i][j].velocity[1] = 0;
			if (x > 98 || x < 3)
				Map[i][j].velocity[0] = 0;
		}
	}
}

void Grid::Update_Particle_Position()
{
	for (int p = 0; p < Material->points.size(); p++)
	{
		Particle& P = Material->points[p];
		double x = P.pseudo_position[0], y = P.pseudo_position[1];
		Matrix2d affine = Matrix2d::Zero();
		int u = P.int_position[0], v = P.int_position[1];
		P.velocity = Vector2d::Zero();
		double density = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (Map[i + u - 1][j + v - 1].active)
				{
					density += Map[i + u - 1][j + v - 1].mass*P.weight(i, j);
					P.velocity += Map[i + u - 1][j + v - 1].velocity * P.weight(i, j);
					Eigen::Vector2d position = Eigen::Vector2d((i + u - 1)*cellsize[0] + start[0], (j + v - 1)*cellsize[1] + start[1]);
					affine += P.weight(i, j)*Map[i + u - 1][j + v - 1].velocity*(position - P.position).transpose();
				}
			}
		}
		density /= cellsize[0] * cellsize[1];
		P.volume = P.mass / density;
		P.position += P.velocity * DT;
		P.momentum = P.velocity * P.mass;
		P.B = affine;
	}
}

void Grid::Update_Deformation_Gradient()
{
	for (int p = 0; p < Material->points.size(); p++)
	{
		Particle& P = Material->points[p];
		double x = P.pseudo_position[0], y = P.pseudo_position[1];
		int u = P.int_position[0], v = P.int_position[1];
		Matrix2d A = Matrix2d::Zero();
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				A += Map[i + u - 1][j + v - 1].velocity * P.Weight_Gradient[i][j].transpose();
			}
		}
		P.Velocity_Gradient = A;
		A = A * DT + Matrix2d::Identity();
		P.Deformation_Gradient = A * P.Deformation_Gradient;
		P.J = P.Deformation_Gradient.determinant();
	}
}

void Grid::Update_Particle_Cloud()
{
	Update_Particle_Position();
	Update_Deformation_Gradient();
}

void Grid::Update_Grid()
{
	Update_From_Material();
	Compute_Weight_Gradient();
	Material->Compute_Energy_Derivative();
	Update_Force();
	Update_From_Force();
	Determine_Collision();
}
