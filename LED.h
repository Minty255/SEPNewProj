// File:	LED.h
// By:		Minh Dang
// Date:	May 2014
// Info:	Definition for LED.cpp

#pragma once
#include <string>
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;
using namespace std;

class LED
{
public:
	LED(void);
	~LED(void);

	LED(string name);

	int getXPos() { return xPos; }
	void setXPos(int x) { xPos = x; }
	
	int getYPos() { return yPos; }
	void setYPos(int y) { yPos = y; }

	Scalar getHSVMin() { return hsvMin; }
	void setHSVMin(Scalar min) { hsvMin = min; }

	Scalar getHSVMax() { return hsvMax; }
	void setHSVMax(Scalar max) { hsvMax = max; }

	string getType() { return type; }
	void setType(string t) { type = t; }

	Scalar getColour() { return colour; }
	void setColour(Scalar c) { colour = c; }

private:
	int xPos, yPos;
	string type;
	Scalar hsvMin, hsvMax;
	Scalar colour;
};

