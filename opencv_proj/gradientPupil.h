#pragma once

cv::Point2i findPupilCenterGradient(cv::Mat image);
cv::Mat matrixMagnitude(const cv::Mat & matX, const cv::Mat & matY);
cv::Mat computeMatXGradient(const cv::Mat & mat);
double computeDynamicThreshold(const cv::Mat & mat, double stdDevFactor);
void testPossibleCentersFormula(int x, int y, const cv::Mat & weight, double gx, double gy, cv::Mat & out);

