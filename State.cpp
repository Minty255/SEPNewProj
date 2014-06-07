#include "State.h"

State::State() {

}

// Alternate constructor
State::State(int x, int y, int z, int roll, int pitch, int yall) {
	m_x = x;
	m_y = y;
	m_z = z;
	m_roll = roll;
	m_pitch = pitch;
	m_yall = yall;
}

// Copy constructor
State::State(const State &state) {
	m_x = state.m_x;
	m_y = state.m_y;
	m_z = state.m_z;
	m_roll = state.m_roll;
	m_pitch = state.m_pitch;
	m_yall = state.m_yall;
}
