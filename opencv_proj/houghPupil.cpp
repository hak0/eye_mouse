#include "stdafx.h"
#include "houghPupil.h"

// »ô·òÔ²ÄâºÏ·½·¨
int findPupilCenterHough(cv::Mat image, cv::Point2i &center) {
	std::vector<cv::Vec3f> circles;
	using namespace cv;
	// Convert image to grayscale
	Mat imageGray, imageBinarized;
	cvtColor(image, imageGray, COLOR_BGR2GRAY);
	// Equalize the contrast
	equalizeHist(imageGray, imageGray);
	// threshold(imageGray, imageBinarized, 100, 255, THRESH_BINARY);
	// threshold(imageGray, imageBinarized, 60, 255, THRESH_BINARY | THRESH_OTSU);
	// Extract the contour
	Canny(imageGray, imageGray, 100, 200, 3);
	imshow("pupil", imageGray);
	//HoughCircles(imageBinarized, circles, HOUGH_GRADIENT, 1, imageGray.rows / 8,
	HoughCircles(imageGray, circles, HOUGH_GRADIENT, 1, imageGray.rows / 16,
		50, 20, imageGray.rows / 4, imageGray.rows / 2);

	if (circles.empty()) {
		return -1;
	}
	else {
		center.x = circles[0][0];
		center.y = circles[0][1];
		return 0;
	}
}

