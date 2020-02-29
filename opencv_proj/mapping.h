#pragma once
#include "stdafx.h"

class mappingModel {
public:
	void pushPoints(const cv::Point2d & eyePoint, const cv::Point2d & screenPoint);
	void clear();
	void solveParameter();
	cv::Point2i generateScreenPoint(cv::Point2d eyePoint);
	void printData();
	int size();
private:
	std::vector<cv::Point2d> eyePoints;
	std::vector<cv::Point2d> screenPoints;
	cv::Mat parameter;
	cv::Mat offsetEye;
	cv::Mat offsetScreen;
};