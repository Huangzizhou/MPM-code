#include "Particle.h"
#include "Parameters.h"
#include <stdlib.h>

Particle::Particle()
{
}

Particle::~Particle()
{
}

Particle::Particle(Eigen::Vector2d Pos, Eigen::Vector2d Vel, double Mass, MaterialType tag,double Miu)
{
	position = Pos;
	velocity = Vel;
	mass = Mass;
	Tag = tag;
	J = 1;
	B = Matrix2d::Zero();
	Deformation_Gradient = Matrix2d::Identity();
	volume = 0;
	momentum = mass * velocity;
	miu = Miu;
}

void Particle::Compute_Energy_Derivative()
{
	if (Tag == Elastic)
	{
		Energy_Derivative = Matrix2d::Zero();
		JacobiSVD<Eigen::MatrixXd>svd(Deformation_Gradient, ComputeThinU | ComputeThinV);
		Matrix2d U = svd.matrixU();
		Matrix2d V = svd.matrixV();
		Matrix2d S = U.transpose()*Deformation_Gradient*V;
		double DET = S.determinant();
		Energy_Derivative += 2 * miu * (Deformation_Gradient - U * V.transpose()) * Deformation_Gradient.transpose();
		Energy_Derivative += lambda * (DET - 1.0) * DET * Matrix2d::Identity();
	}
	else if(Tag == Water)
	{
		Matrix2d out;
		double P = Hardness * (1 / pow(J, Gama) - 1);
		out = -P * Matrix2d::Identity();
		Energy_Derivative = out;
	}
}

void Particle::Update_Position()
{
	position += DT * velocity;
}