/*
#include "opencv2/opencv.hpp"
#include<sstream>
using namespace cv;
using namespace std;
int main(int, char**)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	cvNamedWindow("mywindow", CV_WINDOW_AUTOSIZE);
	int ct=0;
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
					  // do any processing
		imshow("mywindow", frame);
		

//		String filename = "cropped_" + (ct + 1);
//		imwrite(filename + ".jpg", frame);

		
		
			// ...

			// saving part:
			//std::string savingName = "C:\\Users\\Michal\\Documents\\Visual Studio 2015/" + std::to_string(cntr) + ".jpg";
			//cv::imwrite(savingName, frame);
			//cntr++;
		//String filename = "cropped_" + cntr;
		//imwrite("C:\\Users\\Michal\\Documents\\Visual Studio 2015/" +filename + ".jpg", frame);
	    //cntr++;
			stringstream ss;

string name = "image_";
string type = ".jpg";

ss<<name<<(ct + 1)<<type;

string filename = ss.str();
ss.str("");

//imwrite(filename, frame);
			//imwrite("C:\\Users\\Michal\\Documents\\Visual Studio 2015/"+cntr + ".bmp", frame);
		
		imwrite("C:\\Users\\Michal\\Documents\\Visual Studio 2015/"+ filename + ".bmp", frame);
		ct++;
		if (ct == 10) break;
		if (waitKey(30) >= 0) break;   // you can increase delay to 2 seconds here
	}

	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}
*/
// 888888888888888888888888888888888888888888888888888888888888888888
/*
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

using namespace cv;
using namespace std;

void help(char** argv)
{
	cout << "\nThis program gets you started reading a sequence of images using cv::VideoCapture.\n"
		<< "Image sequences are a common way to distribute video data sets for computer vision.\n"
		<< "Usage: " << argv[0] << " <path to the first image in the sequence>\n"
		<< "example: " << argv[0] << " right%%02d.jpg\n"
		<< "q,Q,esc -- quit\n"
		<< "\tThis is a starter sample, to get you up and going in a copy paste fashion\n"
		<< endl;
}

int main(int argc, char** argv)
{
	if (argc !=2)
	{
		help(argv);
		return 1;
	}

	string arg = argv[1];
	VideoCapture sequence(arg);
	if (!sequence.isOpened())
	{
		cerr << "Failed to open Image Sequence!\n" << endl;
		return 1;
	}

	Mat image;
	namedWindow("Image | q or esc to quit", CV_WINDOW_NORMAL);

	for (;;)
	{
		sequence >> image;
		if (image.empty())
		{
			cout << "End of Sequence" << endl;
			break;
		}

		imshow("image | q or esc to quit", image);

		char key = (char)waitKey(500);
		if (key == 'q' || key == 'Q' || key == 27)
			break;
	}

	return 0;
}*/
//8888888888888888888888888888888888888888888888888888888888888888888888
/*
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <cmath>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
	int count;
	double area, ar;
	Mat frame, fore, img, prevImg, temp, gray, vehicle_ROI, img_temp;
	VideoCapture cap("/Users/alien/Documents/Traffic/trafficdb/video/cctv052x2004080516x01638.avi");
	BackgroundSubtractorMOG2 bg(500, 25, false);
	vector<vector<Point> > contours;
	vector<Rect> cars;
	namedWindow("Frame");

	cap >> img_temp;
	cvtColor(img_temp, gray, CV_BGR2GRAY);
	gray.convertTo(temp, CV_8U);
	bilateralFilter(temp, prevImg, 5, 20, 20);

	while (true)
	{
		count = 0;
		cap >> frame;

		cvtColor(frame, gray, CV_BGR2GRAY);
		gray.convertTo(temp, CV_8U);
		bilateralFilter(temp, img, 5, 20, 20);
		bg.operator()(img, fore);
		erode(fore, fore, Mat());
		dilate(fore, fore, Mat());
		findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		vector<vector<Point> > contours_poly(contours.size());
		vector<Rect> boundRect(contours.size());

		for (size_t i = 0; i < contours.size(); i++)
		{
			approxPolyDP(Mat(contours[i]), contours_poly[i], 10, true);
			boundRect[i] = boundingRect(Mat(contours_poly[i]));
			rectangle(frame, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);

			vehicle_ROI = img(boundRect[i]);
			area = contourArea(contours[i], false);
			ar = vehicle_ROI.cols / vehicle_ROI.rows;
			if (area > 450.0 && ar > 0.8)
			{
				count = count + 1;
			}
		}
		stringstream ss;
		ss << count;
		string s = ss.str();
		int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
		double fontScale = 2;
		int thickness = 3;
		cv::Point textOrg(10, 130);
		cv::putText(frame, s, textOrg, fontFace, fontScale, Scalar(0, 255, 0), thickness, 5);

		int win_size = 10;
		int maxCorners = 200;
		double qualityLevel = 0.01;
		double minDistance = 1;
		int blockSize = 3;
		double k = 0.04;
		vector<Point2f> img_corners;
		img_corners.reserve(maxCorners);
		vector<Point2f> prevImg_corners;
		prevImg_corners.reserve(maxCorners);

		goodFeaturesToTrack(img, img_corners, maxCorners, qualityLevel, minDistance, Mat(), blockSize, true);

		cornerSubPix(img, img_corners, Size(win_size, win_size), Size(-1, -1),
			TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));

		vector<uchar> features_found;
		features_found.reserve(maxCorners);
		vector<float> feature_errors;
		feature_errors.reserve(maxCorners);

		calcOpticalFlowPyrLK(img, prevImg, img_corners, prevImg_corners, features_found, feature_errors,
			Size(win_size, win_size), 3,
			cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3), 0, k);

		for (int i = 0; i < features_found.size(); i++) {

			Point2f p0(ceil(img_corners[i].x), ceil(img_corners[i].y));
			Point2f p1(ceil(prevImg_corners[i].x), ceil(prevImg_corners[i].y));
			line(frame, p0, p1, CV_RGB(255, 0, 0), 5);
		}

		prevImg = img;

		imshow("Frame", frame);

		if (waitKey(5) >= 0)
			break;
	}

}
*/

#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <iostream>


using namespace cv;
using namespace std;



int main(int argc, const char** argv)
{

	// add your file name
	VideoCapture cap(1);


	Mat flow, frame;
	// some faster than mat image container
	UMat  flowUmat, prevgray;

	for (;;)
	{

		bool Is = cap.grab();
		if (Is == false) {
			// if video capture failed
			cout << "Video Capture Fail" << endl;
			break;
		}
		else {



			Mat img;
			Mat original;

			// capture frame from video file
			cap.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
			resize(img, img, Size(640, 480));

			// save original for later
			img.copyTo(original);

			// just make current frame gray
			cvtColor(img, img, COLOR_BGR2GRAY);


			// For all optical flow you need a sequence of images.. Or at least 2 of them. Previous                           //and current frame
			//if there is no current frame
			// go to this part and fill previous frame
			//else {
			// img.copyTo(prevgray);
			//   }
			// if previous frame is not empty.. There is a picture of previous frame. Do some                                  //optical flow alg. 

			if (prevgray.empty() == false) {

				// calculate optical flow 
				calcOpticalFlowFarneback(prevgray, img, flowUmat, 0.4, 1, 12, 2, 8, 1.2, 0);
				// copy Umat container to standard Mat
				flowUmat.copyTo(flow);


				// By y += 5, x += 5 you can specify the grid 
				for (int y = 0; y < original.rows; y += 10) {
					for (int x = 0; x < original.cols; x += 10)
					{
						// get the flow from y, x position * 10 for better visibility
						const Point2f flowatxy = flow.at<Point2f>(y, x) * 10;
						// draw line at flow direction
						line(original, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255, 0, 0));
						// draw initial point
						circle(original, Point(x, y), 1, Scalar(0, 0, 0), -1);


					}

				}

				// draw the results
				namedWindow("prew", WINDOW_AUTOSIZE);
				imshow("prew", original);

				// fill previous image again
				img.copyTo(prevgray);

			}
			else {

				// fill previous image in case prevgray.empty() == true
				img.copyTo(prevgray);

			}


			int key1 = waitKey(20);

		}
	}
}