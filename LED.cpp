// File:	LED.cpp
// By:		Minh Dang
// Date:	May 2014
// Info:	This class in charge of storing HSV values
//			for image filtering to detect objects.

#include "LED.h"

LED::LED(void)
{

}

LED::LED(string name) {
	setType(name);

	if (name == "wLight") {
		setHSVMin(Scalar(149, 0, 252));
		setHSVMax(Scalar(153, 1, 256));
	}

	if (name == "red") {
		// Orange ping pong ball @HOME
		setHSVMin(Scalar(16, 96, 226));
		setHSVMax(Scalar(35, 256, 256));
		setColour(Scalar(0, 0, 255));

		// TESTING FOR QUAD LIGHT
		//setHSVMin(Scalar(142, 0, 254));
		//setHSVMax(Scalar(153, 7, 256));
		//setColour(Scalar(0, 0, 255));

		// TEST BALL @LAB
		//setHSVMin(Scalar(0, 88, 168));
		//setHSVMax(Scalar(39, 228, 256));
		//setColour(Scalar(0, 0, 255));
	}

	if (name == "blue") {
		// Testing code
	//	setHSVMin(Scalar(0, 126, 142));
	//	setHSVMax(Scalar(251, 195, 252));
	//	setColour(Scalar(255, 0, 0));
	}

	if (name == "yellow") {

	}
}

LED::~LED(void)
{
}