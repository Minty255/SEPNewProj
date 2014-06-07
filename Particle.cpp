#include "Particle.h"

Particle::Particle() {

}

Particle::Particle(State state, double score) {
	m_state = State(state);
	m_score = score;
}