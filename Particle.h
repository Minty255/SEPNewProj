#pragma once

#include "State.h"

class Particle {
public:
	Particle();
	~Particle();

	Particle(State state, double score);

	State getState() { return m_state; }
	void setState(State state) { m_state = State(state); }

	double getScore() { return m_score; }
	void setScore(double score) { m_score = score; }

// Currently set to public for easy access,
// can be change to private later and use getters
// and setters
public:
	State m_state;
	double m_score;
};