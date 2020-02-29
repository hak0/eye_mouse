#include "mapping.h"

void mappingModel::pushPoints(const cv::Point2d & eyePoint, const cv::Point2d & screenPoint)
{
	eyePoints.push_back(eyePoint);
	screenPoints.push_back(screenPoint);
}

void mappingModel::clear()
{
	std::vector<cv::Point2d>().swap(eyePoints);
	std::vector<cv::Point2d>().swap(screenPoints);
}

void mappingModel::solveParameter()
{
	cv::Mat_<double> eyeMat = cv::Mat(eyePoints.size(), 2,CV_64F, eyePoints.data());
	cv::Mat_<double> screenMat = cv::Mat(screenPoints.size(), 2, CV_64F, screenPoints.data());
	cv::reduce(eyeMat, offsetEye, 0, cv::REDUCE_AVG);
	cv::reduce(screenMat, offsetScreen, 0, cv::REDUCE_AVG);
	for (int i = 0; i < eyeMat.rows; i++) {
		eyeMat.row(i) = eyeMat.row(i) - offsetEye;
		screenMat.row(i) = screenMat.row(i) - offsetScreen;
	}
	cv::Mat parameter00, parameter11;
	cv::solve(eyeMat.col(0), screenMat.col(0), parameter00, cv::DECOMP_QR);
	cv::solve(eyeMat.col(1), screenMat.col(1), parameter11, cv::DECOMP_QR);
	parameter = (cv::Mat_<double>(2,2) << parameter00.at<double>(0, 0) ,0.0 , 0.0 ,	parameter11.at<double>(0, 0));

}

cv::Point2i mappingModel::generateScreenPoint(cv::Point2d eyePoint)
{
	cv::Mat eyeMat = (cv::Mat_<double>(1, 2) << eyePoint.x, eyePoint.y);
	cv::Mat result = cv::Mat_<double>(1, 2);
	eyeMat = eyeMat - offsetEye;
	result = eyeMat * parameter;
	result = result + offsetScreen;
	return cv::Point2i(result.at<double>(0,0), result.at<double>(0,1));
}

void mappingModel::printData()
{
	std::cout << "eyeMat" << eyePoints << std::endl;
	std::cout << "screenMat" << screenPoints << std::endl;
	std::cout << "parameter" << parameter << std::endl;
}

int mappingModel::size()
{
	return eyePoints.size();
}
