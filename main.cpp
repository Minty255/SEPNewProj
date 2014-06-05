// File:	main.cpp
// By:		Minh Dang
// Date:	May 2014
// Info:	Multiple objects detection using blob detection approach.
//			Also contain implementation of particles filter
//			Credits to: Dr Raymond Sheh and Dr Patrick Peursum for explaining
//			the theory behind particles filters.
//			This code has been downloaded with modification and built on top.

//Written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy 
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights 
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
//COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
//IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <sstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <math.h>
#include <time.h>
#include "LED.h"
#include "Particle.h"

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=500;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 5*5;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//numbers of particle
const int NUM_PARTICLES = 1000;
// constant for adjusting
const double LAMDA = 20.0;
// constant for uncertainty
const int UNCERT = 15;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string windowName4 = "Extra Test Window";
const string trackbarWindowName = "Trackbars";

// Vector of particles and object
double *oldScore;
vector<Position> objectCoord;
vector<Particle> particles;


// first time running flag
bool firstRun = true;

void on_trackbar(int, void*)
{
	//This function gets called whenever a
	// trackbar position is changed
}

string intToString(int number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars() {
	//create window for trackbars
	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the
	//trackbar is moved(eg.H_LOW), the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is 
	//moved(eg. on_trackbar)                    ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );
}

void drawObject(vector<LED> theLEDs,Mat &frame) {
	for (int ii = 0; ii < theLEDs.size(); ii++) {
		Particle temp(theLEDs[ii].getXPos(), theLEDs[ii].getYPos(), 0, 0, 0, 0);
		objectCoord.push_back(temp);
		cv::circle(frame, cv::Point(theLEDs[ii].getXPos(), theLEDs[ii].getYPos()),
					5, cv::Scalar(0, 0, 255));
		// Label the object with coordinate
		cv::putText(frame, intToString(theLEDs[ii].getXPos())+ " , " +
				intToString(theLEDs[ii].getYPos()), cv::Point(theLEDs[ii].getXPos(),
				theLEDs[ii].getYPos()+20), 1, 1, Scalar(0, 255, 0));
	}
}

void morphOps(Mat &thresh) {
	//create structuring element that will be used to "dilate" and "erode" image.

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(1, 1));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(2, 2));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
}

void trackFilteredObject(Mat threshold,Mat HSV, Mat &cameraFeed) {

	vector<LED> reds;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20x20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably
				//just a bad filter we only want the object with the largest
				//area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA){
					LED red;

					red.setXPos(moment.m10/area);
					red.setYPos(moment.m01/area);
					
					reds.push_back(red);

					objectFound = true;

				} else {
					objectFound = false;
				}
			}
			//let user know you found an object
			if(objectFound ==true) {
				//draw object location on screen
				drawObject(reds,cameraFeed);
			}
		} else {
			putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),
					1,2,Scalar(0,0,255),2);
		}
	}
}

// Overloading method for multiple object tracking
void trackFilteredObject(LED theLED, Mat threshold,Mat HSV, Mat &cameraFeed) {
	vector<LED> leds;
	Mat temp;

	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				
				// Check for object area
				if(area>MIN_OBJECT_AREA){
					LED led;

					led.setXPos(moment.m10/area);
					led.setYPos(moment.m01/area);
					led.setType(theLED.getType());
					led.setColour(theLED.getColour());
					leds.push_back(led);

					objectFound = true;

				} else {
					objectFound = false;
				}
			}

			//let user know you found an object
			if(objectFound == true){
				//draw object location on screen
				drawObject(leds,cameraFeed);
			}
		} else {
			putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),
					1,2,Scalar(0,0,255),2);
		}
	}
}

// Callback function for mouse event
void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	if(event == EVENT_LBUTTONDOWN) {
		// Use for testing, create/modify first particle
		particles[0].setX(x);
		particles[0].setY(y);

		cout << "Left button clicked - position (" << x << ", " << y << ")" << endl;
	} else if(event == EVENT_RBUTTONDOWN) {
		cout << "Right button clicked - position (" << x << ", " << y << ")" << endl;
	} else if(event == EVENT_MBUTTONDOWN) {
		cout << "Middle button clicked - position (" << x << ", " << y << ")" << endl;
	} else if (event == EVENT_MOUSEMOVE) {
		//cout << "Mouse move - position (" << x << ", " << y << ")" << endl;
	}
}

// Generate new particles given a boundary and range
int genNewCoord(int currCoord, int uBoundary) {
	int newCoord;
	
	do {
		newCoord = currCoord + (rand() % UNCERT + (-(UNCERT - 10)));
	} while (!((newCoord >= 0) && (newCoord <= uBoundary)));

	return newCoord;
}

// Resample particles to generate new one
void resample(vector<double> cumulative) {
	default_random_engine generator;
	uniform_real_distribution<double> distribution(0.0f, 1.0f);
	vector<Particle> newParticles;

	int newX, newY, newZ, newRoll, newPitch, newYaw;
	int bound = (double)particles.size() * 0.85;

	for (int ii = 0; ii < particles.size(); ii++) {
		double uniRand = distribution(generator);
		int iPos = 0;

		// Only keep a % of old particles, the rest will be generated, this way we
		// can ensure that new objects enter the scene can still be detected
		if (ii < bound) {
			// Iterate through to see where is the position in the cumulative sum
			// This will iterate through and stop at either the last element or found
			// the correct place
			// This while loop code is provided by Dr Patrick Peursum 2nd June 2014
			while ((iPos < cumulative.size()-1) && (cumulative[iPos] < uniRand)) {
				iPos++;
			}

			// Copy particles to new location
			newX = genNewCoord(particles[iPos].getX(), FRAME_WIDTH);
			newY = genNewCoord(particles[iPos].getY(), FRAME_HEIGHT);
			newParticles.push_back(Particle(newX, newY, particles[iPos].getZ(),
						particles[iPos].getRoll(), particles[iPos].getPitch(),
						particles[iPos].getYaw()));

			//oldScore[particles.size()-1] = particles[iPos].getScore();
		} else {
			newParticles.push_back(
					Particle(rand()%FRAME_WIDTH, rand()%FRAME_HEIGHT, 0, 0, 0, 0));
			//oldScore[particles.size()-1] = 0;
		}

		newParticles[0].setScore(1.0f/particles.size());
	}

	// Clean the old particles vector store newly generated particles
	particles.clear();
	particles.insert(particles.begin(), newParticles.begin(), newParticles.end());
}

// Scoring particles function
void scoreParticles(Mat &feed) {
	double temp = 0.0, total = 0.0;
	double d1, d2, t1, t2;
	double sumSqrtWt = 0.0;
	vector<double> cumulative(particles.size());
	vector<double> scores;

	// This will iterate through, cal the dist between each particle and each obj
	// Then it will feed the calculated distance into a function to give it a score
	// Highest score (closest dist) between a particle and an object will be store
	for (int ii = 0; ii < particles.size(); ii++) {
		for (int jj = 0; jj < objectCoord.size(); jj++) {
			d1 = pow((double)(particles[ii].getX() - objectCoord[jj].getX()), 2);
			d2 = pow((double)(particles[ii].getY() - objectCoord[jj].getY()), 2);
			t1 = 1/LAMDA;
			t2 = sqrt(d1+d2);
			temp = t1*exp(-t1*t2);
			
			scores.push_back(temp);
		}

		// Update particle's score and also calculate the total score
		sort(scores.begin(), scores.end());
		
		if (scores.size() != 0) {
			// THIS CONTAIN BUG WHICH NEED TO BE FIX - don't know how to yet...
			// If first run, no change to score otherwise need to multiply by old wt
			//if (firstRun == true) {
				particles[ii].setScore(scores[scores.size()-1]);
			//	firstRun = false;
			//} else
			//	particles[ii].setScore(scores[scores.size()-1] * oldScore[ii]);
			
			total += scores[scores.size()-1];
			scores.clear();
		}
	}
	
	// Normalise the score for all particles
	// Also construct a cummulatives score for all the particles
	if (total > 0) {
		for (int ii = 0; ii < particles.size(); ii++) {
			particles[ii].setScore(particles[ii].getScore() / total);

			if (ii == 0)
				cumulative[ii] = particles[ii].getScore();
			else if (ii == (particles.size() - 1))
				cumulative[ii] = 1.0;
			else
				cumulative[ii] = cumulative[ii-1] + particles[ii].getScore();

			sumSqrtWt += pow(particles[ii].getScore(), 2);
		}
	}

	// Calculate the survival rate of particles
	double survivalRate = 1.0 / (sumSqrtWt * particles.size());

	// Check % of particles gonna survive before resample
	//if (survivalRate > 0.2) 
		resample(cumulative);
}

// Iterate through and output all particles' score
void outputScores() {
	for (int ii = 0; ii < particles.size(); ii++)
		printf("Particle (%d, %d): %.50f\n", particles[ii].getX(),
				particles[ii].getY(), particles[ii].getScore());
}

// Median filter will help filter out noise
// THIS MAKE PROCESSING WAYYYY TOO SLOW!
void medianFilter(Mat &img) {
	int *pixVal = new int[9];
	int counter = 0;
	unsigned char *input = (unsigned char*)(img.data);

	for (int col = 0; col < img.cols - 3; col++) {
		for (int row = 0; row < img.rows - 3; row++) {
			counter = 0;
			for (int ii = 0; ii < 3; ii++) {
				for (int jj = 0; jj < 3; jj++) {
					pixVal[counter] = (input[(img.step[0]*(row+ii)) + (img.step[1]*(col+jj))]
									+ input[(img.step[0]*(row+ii)) + (img.step[1]*(col+jj)) + 1] +
									input[(img.step[0]*(row+ii)) + (img.step[1]*(col+jj)) + 2])/3;
					counter++;
				}
			}

			sort(pixVal, pixVal + 9);
			img.data[(img.step[0]*(row+1)) + (img.step[1]*(col+1)) + 0] = pixVal[4];
			img.data[(img.step[0]*(row+1)) + (img.step[1]*(col+1)) + 1] = pixVal[4];
			img.data[(img.step[0]*(row+1)) + (img.step[1]*(col+1)) + 2] = pixVal[4];
		}
	}

	delete [] pixVal;
}

int main(int argc, char* argv[])
{
	//if we would like to calibrate our filter values, set to true.
	bool calibrationMode = true;
	
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat filteredThresh;
	Mat HSV;
	Mat YUV;

	oldScore = new double[NUM_PARTICLES];

	if(calibrationMode){
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	
	srand(time(NULL));
	// Generate random particles
	for (int ii = 0; ii < NUM_PARTICLES; ii++) {
		Particle temp(abs(rand()%(FRAME_WIDTH)), abs(rand()%(FRAME_HEIGHT)), 0,0,0,0);
//		Particle temp(100, 100, 0, 0, 0, 0);
		particles.push_back(temp);
	}

	namedWindow(windowName);
	//namedWindow(windowName1);
	//namedWindow(windowName2);
	//namedWindow(windowName3);
	//namedWindow(windowName4);
	
	// Mouse click event
	setMouseCallback(windowName, CallBackFunc, NULL);

	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1) {
		// Clear objects in vector so could store new one 
		objectCoord.clear();
		
		//store image to matrix	
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		//cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

		// TESTING
		cvtColor(cameraFeed, YUV, COLOR_BGR2YUV);

		if(calibrationMode==true) {
			//if in calibration mode, we track objects based on the HSV slider values.
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			//inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
			inRange(YUV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
			morphOps(threshold);
			imshow(windowName2,YUV);
			trackFilteredObject(threshold,HSV,cameraFeed);
		} else {
			// Declare a list of object to be label - LED's colours
			LED red("red");

			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV, red.getHSVMin(), red.getHSVMax(), threshold);
			trackFilteredObject(red, threshold,HSV,cameraFeed);
		}
		
		// Display particle on the actual image frame
		for (int ii = 0; ii < particles.size(); ii++) {
			cv::circle(cameraFeed, cv::Point(particles[ii].getX(), particles[ii].getY()),
						1, Scalar(0, 0, 255), 1, 0, 0);
		}

		scoreParticles(cameraFeed);

		//filteredThresh = threshold.clone();
		//medianFilter(threshold);

		//show frames 
		imshow(windowName,cameraFeed);
		imshow(windowName1,threshold);
		//imshow(windowName4, filteredThresh);
		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}


	// TESTING LAB COLOUR SPACE
/*	while(1) {
		capture.read(cameraFeed);
		cvtColor(cameraFeed, threshold, CV_BGR2Lab);
		imshow(windowName,threshold);
		waitKey(30);
	}
	*/
	return 0;
}
