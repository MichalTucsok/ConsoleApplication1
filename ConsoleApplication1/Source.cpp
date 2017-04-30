

/////////////////////////////////////////////////////////////////////////

#include "opencv2/video/tracking.hpp"
#include "opencv2/videoio.hpp"
#include <ctype.h>
#include "opencv2/opencv.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv\cv.h>
#include <windows.h>
#include <math.h>

using namespace cv;
using namespace std;

const static int SENSITIVITY_VALUE = 20;

static void help()
{
	// print a welcome message, and the OpenCV version
	cout << "\nThis is a demo of Lukas-Kanade optical flow lkdemo(),\n"
		"Using OpenCV version " << CV_VERSION << endl;
	cout << "\nIt uses camera by default, but you can provide a path to video as an argument.\n";
	cout << "\nHot keys: \n"
		"\tESC - quit the program\n"
		"\tr - auto-initialize tracking\n"
		"\tc - delete all the points\n"
		"\tn - switch the \"night\" mode on/off\n"
		"To add/remove a feature point click it\n" << endl;
}

Point2f point;
bool addRemovePt = false;

static void onMouse(int event, int x, int y, int /*flags*/, void* /*param*/)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		point = Point2f((float)x, (float)y);
		addRemovePt = true;
	}
}

int main(int argc, char** argv)
{
	//Create a black image with the size as the camera output
	Mat imgLines;
	Mat grayImage1, grayImage2;
	Mat differenceImage;
	Mat imgOriginal;
	Mat thresholdImage;
	Mat imgHSV;
	Mat imgThresholded;
	Mat imgTmp;
	Mat imgTmp2;

	Moments oMoments;

	bool bSuccess;

	double dM01;
	double dM10;
	double dArea;
	double vzdialenost;

	int posX;
	int posY;
	int iLastX = -1;
	int iLastY = -1;


	VideoCapture cap(0); //capture the video from webcam
	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
	Size subPixWinSize(10, 10), winSize(31, 31);

	const int MAX_COUNT = 500;
	bool needToInit = false;
	bool nightMode = false;

	cv::CommandLineParser parser(argc, argv, "{@input||}{help h||}");
	string input = parser.get<string>("@input");
	/*
	if (parser.has("help"))
	{
		help();
		return 0;
	}
	if (input.empty())
		cap.open(0);
	else if (input.size() == 1 && isdigit(input[0]))
		cap.open(input[0] - '0');
	else
		cap.open(input);

	if (!cap.isOpened())
	{
		cout << "Could not initialize capturing...\n";
		return 0;
	}
	*/
	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
												//blue color detection settings
	int iLowH = 80;
	int iHighH = 130;

	int iLowS = 150;
	int iHighS = 255;

	int iLowV = 60;
	int iHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	cap.read(imgTmp);	//temporary image from camera
	imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

	vector<Vec3f> circles;
	int radius;
	Point center;


	namedWindow("LK Demo", 1);
	setMouseCallback("LK Demo", onMouse, 0);

	Mat gray, prevGray, image, frame;
	vector<Point2f> points[2];

	//for (;;)
	while(true)
	{
		// imgOriginal premenovane na frame //
		bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) { //if not success, break loop      
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		frame.copyTo(image);
		// grayImage1 premenovane na gray
		cvtColor(image, gray, COLOR_BGR2GRAY);
		

		GaussianBlur(gray, gray, Size(9, 9), 2, 2);

		/// Apply the Hough Transform to find the circles
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, 20, 100, 155, 20, 300);
		waitKey(100);
		/// Draw the circles detected
		for (size_t i = 0; i < circles.size(); i++) {
			//center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			center.x = cvRound(circles[i][0]);
			center.y = cvRound(circles[i][1]);

			radius = cvRound(circles[i][2]);
			// circle center
			circle(image, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			circle(image, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		}

		imshow("Hough circles", image);

		cvtColor(frame, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

																									  //morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//Calculate the moments of the thresholded image
		oMoments = moments(imgThresholded);

		dM01 = oMoments.m01;
		dM10 = oMoments.m10;
		dArea = oMoments.m00;

		// if the area <= 10000, I consider that there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 10000) {
			//calculate the position of the ball
			posX = dM10 / dArea;
			posY = dM01 / dArea;

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0) {
				//Draw a red line from the previous point to the current point
				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
				//std::cout << "X: " << (int)(iLastX-posX) << "     Y: " << (int)(iLastY-posY) << endl;   //pocitam si vzdialenost
				//vzdialenost = sqrt((double)((iLastX - posX)*(iLastX - posX)) + (double)((iLastY - posY)*(iLastY - posY)));
				//cout << vzdialenost  << endl;
				//cout << "\n cm\n" << endl;
				vzdialenost = 2.2 / radius * 1000;
				cout << vzdialenost << endl;
				cout << "\n cm\n" << endl;
				
					
					//printf("Z os %f", 2.2 / radius * 1000);
				//Sleep(2000);
			}

			iLastX = posX;
			iLastY = posY;
		}

		imshow("Thresholded Image", imgThresholded); //show the thresholded image

		frame = frame + imgLines;
		imshow("Original", frame); //show the original image

		


		if (nightMode)
			image = Scalar::all(0);

		if (needToInit)
		{
			// automatic initialization
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			cornerSubPix(gray, points[1], subPixWinSize, Size(-1, -1), termcrit);
			addRemovePt = false;
		}
		else if (!points[0].empty())
		{
			vector<uchar> status;
			vector<float> err;
			if (prevGray.empty())
				gray.copyTo(prevGray);
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
				3, termcrit, 0, 0.001);
			size_t i, k;
			for (i = k = 0; i < points[1].size(); i++)
			{
				if (addRemovePt)
				{
					if (norm(point - points[1][i]) <= 5)
					{
						addRemovePt = false;
						continue;
					}
				}

				if (!status[i])
					continue;

				points[1][k++] = points[1][i];
				circle(image, points[1][i], 3, Scalar(0, 255, 0), -1, 8);
			}
			points[1].resize(k);
		}

		if (addRemovePt && points[1].size() < (size_t)MAX_COUNT)
		{
			vector<Point2f> tmp;
			tmp.push_back(point);
			cornerSubPix(gray, tmp, winSize, Size(-1, -1), termcrit);
			points[1].push_back(tmp[0]);
			addRemovePt = false;
		}

		needToInit = false;
		////////////////
		imshow("LK Demo", image);

		char c = (char)waitKey(10);
		if (c == 27)
			break;
		switch (c)
		{
		case 'r':
			needToInit = true;
			break;
		case 'c':
			points[0].clear();
			points[1].clear();
			break;
		case 'n':
			nightMode = !nightMode;
			break;
		}

		std::swap(points[1], points[0]);
		cv::swap(prevGray, gray);
	}

	return 0;
}