// File:	Particle.cpp
// By:		Minh Dang
// Date:	May 2014
// Info:	This class inherit from Position class.
//			This basically a container class for particles
//			which is use for particles filter

#include "Particle.h"

Particle::Particle(void)
{
}

// Take in the parameters and call the super class alternate constructor
Particle::Particle(int x, int y, int z, int roll, int pitch, int yaw)
					: Position(x, y, z, roll, pitch, yaw) {
	m_score = 0;
}

Particle::~Particle(void)
{
}