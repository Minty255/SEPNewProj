// File:	Position.h
// By:		Minh Dang
// Date:	May 2014
// Info:	Definition for Position.cpp

#pragma once
#include <string>
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;
using namespace std;

class Position
{
public:
	Position(void);
	~Position(void);

	Position(int x, int y, int z, int roll, int pitch, int yaw);

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

	int getYaw() { return m_yaw; }
	void setYaw(int yaw) { m_yaw = yaw; }

private:
	int m_x, m_y, m_z, m_roll, m_pitch, m_yaw;
};

