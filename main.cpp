#include <sstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <math.h>
#include <time.h>

#include "Sensor.h"

// Define image space width and height
const int IMG_WIDTH = 640;
const int IMG_HEIGHT = 480;

// Define window names
const string winName1 = "Original";
const string winName2 = "YUV";
const string winName3 = "Threshold";

int main(int argc, char *argv[]) {
	// Setup
	VideoCapture cap;
	Mat camFeed;
	Mat imgThreshold;
	Mat imgYUV;

	// Switch for calibration
	bool bCali = false;

	// Open cam and setup size
	cap.open(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, IMG_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, IMG_HEIGHT);

	// Seed random
	srand(time(NULL));

	// Infinite loop for continuous discrete img capture
	while (1) {
		cap.read(camFeed);

		if (bCali) {

		} else {

		}

		// Display camera feed
		imshow(winName1, camFeed);

		// Delay refresh rate else feeds won't show
		waitKey(30);
	}

	return 0;
}