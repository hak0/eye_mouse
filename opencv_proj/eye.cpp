#include "eye.h"

eye::eye()
{
	pupilFound = false;
}

cv::Point2d eye::getRelativeCoordinate()
{
	cv::Point2d tempPoint;
	tempPoint.x = 1.0 * (pupilPoint.x) / (borderPoints[1].x - borderPoints[0].x);
	// Eye height value will greatly change when eye sweeps from top to the bottom.
	// It's a better idea to use this data instead of pupil center location.
	tempPoint.y = 1.0 * (borderPoints[1].y - borderPoints[0].y) / (borderPoints[1].x - borderPoints[0].x); 
	return tempPoint;
}

bool eye::pointsAreValid(const cv::Mat & frame)
{
	if (borderPoints[0].x > borderPoints[1].x) return false;
	if (borderPoints[0].y > borderPoints[1].y) return false;
	if (borderPoints[0].x < 0 || borderPoints[0].y < 0) return false;
	if (borderPoints[1].x < 0 || borderPoints[1].y < 0) return false;
	if (borderPoints[0].x > frame.cols || borderPoints[0].y > frame.rows) return false;
	if (borderPoints[1].x > frame.cols || borderPoints[1].y > frame.rows) return false;
	return true;
}

inline bool eye::pupilIsFound()
{
	return pupilFound;
}

inline cv::Mat eye::rangeOnFrame(const cv::Mat & frame)
{
	return frame(cv::Rect(borderPoints[0], borderPoints[1]));
}

void eye::drawOnFrame(cv::Mat & frame)
{
	if (pointsAreValid(frame)){
		// draw the rectangle of range
		cv::rectangle(frame, borderPoints[0], borderPoints[1], cv::Scalar(0, 255, 0), 1, 8, 0);
		if (pupilFound) {
			// draw the pupil point of teh eye
			cv::circle(frame, pupilPoint + borderPoints[0], 2, cv::Scalar(0, 255, 255), 2, 8, 0);
		}

	}
}

int eye::findPupilCenter(const cv::Mat & frame, bool useHoughMethod = false)
{
	pupilFound = false;
	if (!pointsAreValid(frame)) return 1;
	cv::Mat frameEyeRange = rangeOnFrame(frame);
	if (useHoughMethod) {
		// Use hough circle detection to find the pupil and locate the center.
		// Fast but not accurate. It may return no result since detection can fail.
		if (-1 != findPupilCenterHough(frameEyeRange, pupilPoint)) {
			pupilFound = true;
		}
	}
	else {
		// Use gradient method to find the pupil and locate the center
		// Slow but more accurate. It will always return a result.
		pupilPoint = findPupilCenterGradient(frameEyeRange); 
		pupilFound = true;
	}
	return 0;
}

void eye::setBorderPoints(const std::vector<cv::Point2d>::const_iterator& landmarksBegin, const std::vector<cv::Point2d>::const_iterator& landmarksEnd)
{
	auto xMinMaxL =
		std::minmax_element(landmarksBegin, landmarksEnd,

			[&](const auto &point1, const auto &point2) {
		return point1.x < point2.x;
	});
	auto yMinMaxL =
		std::minmax_element(landmarksBegin, landmarksEnd, // not includes +10
			[&](const auto &point1, const auto &point2) {
		return point1.y < point2.y;
	});
	borderPoints[0].x = (xMinMaxL.first)->x - 10;
	borderPoints[0].y = (yMinMaxL.first)->y - 10;
	borderPoints[1].x = (xMinMaxL.second)->x + 10;
	borderPoints[1].y = (yMinMaxL.second)->y + 10;
}

dualEye::dualEye(const std::vector<cv::Point2d> &landmarks)
{
	eyeRight.setBorderPoints(landmarks.begin() + 4, landmarks.begin() + 10);
	eyeLeft.setBorderPoints(landmarks.begin() + 10, landmarks.begin() + 16);
}

cv::Point2d dualEye::getRelativeCoordinate()
{
	return (eyeLeft.getRelativeCoordinate()+eyeRight.getRelativeCoordinate())/2;
}

void dualEye::drawOnFrame(cv::Mat & frame)
{
	eyeLeft.drawOnFrame(frame);
	eyeRight.drawOnFrame(frame);
}

int dualEye::findPupilCenter(const cv::Mat & frame)
{
	eyeLeft.findPupilCenter(frame);
	eyeRight.findPupilCenter(frame);
	return 0;
}

bool dualEye::pupilsAreFound()
{
	return eyeLeft.pupilIsFound() && eyeRight.pupilIsFound();
}

int dualEye::sendEyeNormarizedImage(const cv::Mat & frame, socketCommunication & com)
{
	// resize each eye image to 64*32, and stack them. "Right" eye is on the left.
	cv::Mat tempEye[2], outDualEye;
	cv::Size_<int> newSize(64, 32);
	// change color to grayscale
	cv::cvtColor(eyeRight.rangeOnFrame(frame), tempEye[0], cv::COLOR_BGR2GRAY);
	cv::cvtColor(eyeLeft.rangeOnFrame(frame), tempEye[1], cv::COLOR_BGR2GRAY);
	// resize to the normalized size 
	cv::resize(tempEye[0], tempEye[0], newSize);
	cv::resize(tempEye[1], tempEye[1], newSize);
	// concat two eyes vertically to a 64*64 image
	cv::vconcat(tempEye, 2, outDualEye);
	// equalize hist
	equalizeHist(outDualEye, outDualEye);
	// convet mat to png type
	socketCommunication::pngDataType pngImg(outDualEye);
	// send the data
	com.sendData(pngImg);
	return 0;
}

bool dualEye::pointsAreValid(const cv::Mat & frame)
{
	return eyeLeft.pointsAreValid(frame) && eyeRight.pointsAreValid(frame);
}
