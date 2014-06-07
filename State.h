#pragma once

class State {
public:
	State();
	~State();

	State(int x, int y, int z, int roll, int pitch, int yall);
	State(const State &state);

	int getX() { return m_x; }
	void setX(int x) { m_x = x; }

	int getY() { return m_y; }
	void setY(int y) { m_y = y; }

	int getZ() { return m_z; }
	void setZ(int z) { m_z = z; }

	int getRoll() { return m_roll; }
	void setRoll(int roll) { m_roll = roll; }

	int getPitch() { return m_pitch; }
	void setPitch(int pitch) { m_pitch = pitch; }

	int getYall() { return m_yall; }
	void setYall(int yall) { m_yall = yall; }

// Currently set to public for easy access,
// can be change to private later to use getters
// and setters.
public:
	int m_x, m_y, m_z, m_roll, m_pitch, m_yall;
};
