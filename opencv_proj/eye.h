#pragma once
#include "stdafx.h"

#include "socket.h"
#include "gradientPupil.h"
#include "houghPupil.h"

class eye {
public:
	eye();
	cv::Point2d getRelativeCoordinate(); //get pupil location relative to pupil 
	void drawOnFrame(cv::Mat &frame); // draw rectangle range and center pupil point on frame
	int findPupilCenter(const cv::Mat &frame, bool useHoughMethod); // find pupil center
	void setBorderPoints(const std::vector<cv::Point2d>::const_iterator & landmarksBegin, const std::vector<cv::Point2d>::const_iterator & landmarksEnd);
	inline 	bool pointsAreValid(const cv::Mat &frame); //check if points are valid for further use
	bool pupilIsFound();
	inline cv::Mat rangeOnFrame(const cv::Mat & frame);
private:
	cv::Point2i borderPoints[2];// left-up and right-down border points
	cv::Point2i pupilPoint; //centeral point of pupil
	bool pupilFound;
};

class dualEye {
public:
	dualEye(const std::vector<cv::Point2d> &landmarks);
	cv::Point2d getRelativeCoordinate(); //get pupil location relative to pupil,
	// may be wrong if pupilsAreFound is false 
	void drawOnFrame(cv::Mat &frame); // draw rectangle range and center pupil point on frame
	int findPupilCenter(const cv::Mat &frame); // find pupil center
	bool pupilsAreFound(); //check if pupils for both eyes are found
	int sendEyeNormarizedImage(const cv::Mat &frame, socketCommunication & com); //send eye range to python backend server
	bool pointsAreValid(const cv::Mat &frame); //check if points are valid for further use

private:
	eye eyeLeft;
	eye eyeRight;
};

