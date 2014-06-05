// File:	Particle.h
// By:		Minh Dang
// Date:	May 2014
// Info:	Definition for Particle.cpp

#pragma once
#include "Position.h"

using namespace cv;
using namespace std;

class Particle : public Position
{
public:
	Particle(void);
	~Particle(void);

	Particle(int x, int y, int z, int roll, int pitch, int yaw);

	double getScore() { return m_score; }
	void setScore(double score) { m_score = score; }

private:
	double m_score;
};

