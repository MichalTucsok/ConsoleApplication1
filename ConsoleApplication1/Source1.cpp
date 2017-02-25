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
	int ct = 0;
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

		ss << name << (ct + 1) << type;

		string filename = ss.str();
		ss.str("");

		//imwrite(filename, frame);
		//imwrite("C:\\Users\\Michal\\Documents\\Visual Studio 2015/"+cntr + ".bmp", frame);

		imwrite("C:\\Users\\Michal\\Documents\\Visual Studio 2015/" + filename + ".bmp", frame);
		ct++;
		if (ct == 10) break;
		if (waitKey(30) >= 0) break;   // you can increase delay to 2 seconds here
	}

	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}