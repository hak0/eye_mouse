#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "dlib/opencv.h"
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing.h"
#include "dlib/gui_widgets.h"

#include <stdio.h>

void dlibPointsDetect(cv::Mat frame, std::vector<cv::Point2d> &landmarks, dlib::frontal_face_detector &detector, dlib::shape_predictor &pose_model);
void findPupil(cv::Mat image, std::vector<cv::Vec3f>& circles);

const cv::String window_name = "Capture - Face detection";
const double scale_ratio = 0.3;

int main()
{
	// Load dlib face detection and pose estimation models
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	dlib::shape_predictor pose_model;
	dlib::deserialize("C:/Users/lxcom/source/repos/opencv_proj/opencv_proj/data/shape_predictor_68_face_landmarks.dat") >> pose_model;

	// Read the stream
	cv::VideoCapture cap;
	cap.open("C:/Users/lxcom/source/repos/opencv_proj/opencv_proj/test.mp4");
	if (!cap.isOpened())
	{
		return -1;
	}
	cv::Mat frame;
	cv::Mat frame_small;

	bool stop = false;
	while (!stop)
	{
		cap >> frame;
		if (frame.empty())
			break;
		// Change the frame size to accelerate dlib processing
		cv::resize(frame, frame_small, frame_small.size(), scale_ratio, scale_ratio, 1);
		// Detect feature points
		std::vector<cv::Point2d> landmarks;
		dlibPointsDetect(frame_small, landmarks, detector, pose_model);
		// Select eye range
		if (!landmarks.empty()) {
			// find the border of range
			auto xMinMax = std::minmax_element(landmarks.begin()+4, landmarks.begin()+10, 

				[&](const auto &point1,const auto &point2) {
				return point1.x < point2.x;
			});
			auto yMinMax = std::minmax_element(landmarks.begin()+4, landmarks.begin()+10, 
				[&](const auto &point1,const auto &point2) {
				return point1.y < point2.y;
			});
			cv::Point2d eyePoint1((xMinMax.first)->x-10, (yMinMax.first)->y-10), eyePoint2((xMinMax.second)->x+10, (yMinMax.second)->y+10);
			// draw the rectangle of range
			cv::rectangle(frame, eyePoint1, eyePoint2, cv::Scalar(0,255,0), 1, 8, 0);

			// find the pupil
			cv::Mat eyeRange = frame(cv::Rect(eyePoint1, eyePoint2));
			std::vector<cv::Vec3f> circles;
			findPupil(eyeRange, circles);
			unsigned int eyeHeight = abs(eyePoint2.y - eyePoint1.y);
			//find the circle whose radius is most approximate to 1/2 eye height
			if (!circles.empty() && circles.size() > 0) {
				std::vector<cv::Vec3f>::iterator pupil = std::min_element(circles.begin(), circles.end(),
					[&](const auto &circle1, const auto &circle2) {
					return abs(circle1[2]-eyeHeight/2) < abs(circle2[2]-eyeHeight/2);
				});
				cv::Point2d center(cvRound((*pupil)[0]),cvRound((*pupil)[1]));
				center += eyePoint1;
				int radius = cvRound((*pupil)[2]);
				if (radius > 2) {
					cv::circle(frame, center, 2, cv::Scalar(0, 255, 255), 2, 8, 0);
					cv::circle(frame, center, radius, cv::Scalar(0, 255, 255), 2, 8, 0);
				}
				std::cout << "circls.empty()" << circles.empty() << " circles.size()" << circles.size() << " radius" << radius << std::endl;
			}
		}

		// Draw feature points
		for (auto& it : landmarks) {
			cv::circle(frame, it, 4, cv::Scalar(0, 0, 255), -1);
		}
		//_sleep(500); slow down the process to diagnose
		cv::imshow(window_name, frame);
		if (cv::waitKey(30) >= 0)
			stop = true;
	}
	return 0;
}

void dlibPointsDetect(cv::Mat frame, std::vector<cv::Point2d> &landmarks, dlib::frontal_face_detector &detector, dlib::shape_predictor &pose_model) {
	using namespace dlib;

	cv_image<dlib::bgr_pixel> cimg(frame);
	std::vector<rectangle> faces = detector(cimg);
	// Find the pose of each face.
	std::vector<full_object_detection> shapes;
	for (unsigned long i = 0; i < faces.size(); ++i) {
		full_object_detection shape = pose_model(cimg, faces[i]);
		shapes.push_back(shape);
		// Dlib's feature points are arranged in orderso we can get 6 landmarks below by id
		landmarks.push_back(cv::Point2d(shape.part(17).x()/scale_ratio, shape.part(17).y()/scale_ratio)); //#17 left brow left corner
		landmarks.push_back(cv::Point2d(shape.part(21).x()/scale_ratio, shape.part(21).y()/scale_ratio)); //#21 left brow right corner
		landmarks.push_back(cv::Point2d(shape.part(22).x()/scale_ratio, shape.part(22).y()/scale_ratio)); //#22 right brow left corner
		landmarks.push_back(cv::Point2d(shape.part(26).x()/scale_ratio, shape.part(26).y()/scale_ratio)); //#26 right brow right corner
		landmarks.push_back(cv::Point2d(shape.part(36).x()/scale_ratio, shape.part(36).y()/scale_ratio)); //#36 left eye left corner
		landmarks.push_back(cv::Point2d(shape.part(37).x()/scale_ratio, shape.part(37).y()/scale_ratio)); //#36 left eye left corner
		landmarks.push_back(cv::Point2d(shape.part(38).x()/scale_ratio, shape.part(38).y()/scale_ratio)); //#36 left eye left corner
		landmarks.push_back(cv::Point2d(shape.part(39).x()/scale_ratio, shape.part(39).y()/scale_ratio)); //#39 left eye right corner
		landmarks.push_back(cv::Point2d(shape.part(40).x()/scale_ratio, shape.part(40).y()/scale_ratio)); //#42 right eye left corner
		landmarks.push_back(cv::Point2d(shape.part(41).x()/scale_ratio, shape.part(41).y()/scale_ratio)); //#42 right eye left corner
		landmarks.push_back(cv::Point2d(shape.part(42).x()/scale_ratio, shape.part(42).y()/scale_ratio)); //#42 right eye left corner
		landmarks.push_back(cv::Point2d(shape.part(43).x()/scale_ratio, shape.part(43).y()/scale_ratio)); //#42 right eye left corner
		landmarks.push_back(cv::Point2d(shape.part(44).x()/scale_ratio, shape.part(44).y()/scale_ratio)); //#42 right eye left corner
		landmarks.push_back(cv::Point2d(shape.part(45).x()/scale_ratio, shape.part(45).y()/scale_ratio)); //#45 right eye right corner
		landmarks.push_back(cv::Point2d(shape.part(46).x()/scale_ratio, shape.part(46).y()/scale_ratio)); //#31 nose left corner
		landmarks.push_back(cv::Point2d(shape.part(47).x()/scale_ratio, shape.part(47).y()/scale_ratio)); //#35 nose right corner
		landmarks.push_back(cv::Point2d(shape.part(48).x()/scale_ratio, shape.part(48).y()/scale_ratio)); //#48 mouth left corner
		landmarks.push_back(cv::Point2d(shape.part(54).x()/scale_ratio, shape.part(54).y()/scale_ratio)); //#54 mouth right corner
		landmarks.push_back(cv::Point2d(shape.part(57).x()/scale_ratio, shape.part(57).y()/scale_ratio)); //#57 mouth central bottom corner
		landmarks.push_back(cv::Point2d(shape.part(8).x()/scale_ratio, shape.part(8).y()/scale_ratio));   //#8 chin corner
	}

	std::cout << "Number of faces detected: " << faces.size() << std::endl;
}

void findPupil(cv::Mat image, std::vector<cv::Vec3f>& circles)
{
	using namespace cv;
	// Convert image to grayscale
	Mat imageGray, imageBinarized;
	cvtColor(image, imageGray, COLOR_BGR2GRAY);
	// Binarization
	int blockSize = 11;
	int constValue = 2;

	//GaussianBlur(imageGray, imageGray, Size(9, 9), 0);
	equalizeHist(imageGray, imageGray);
	//threshold(imageGray, imageBinarized, 100, 255, THRESH_BINARY);
	//threshold(imageGray, imageBinarized, 60, 255, THRESH_BINARY | THRESH_OTSU);
	Canny(imageGray, imageBinarized, 100, 200, 3);
	//std::vector<std::vector<Point>> contours;
	//findContours(imageBinarized, contours, RETR_CCOMP, CHAIN_APPROX_NONE);
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	drawContours(imageGray, contours, i, Scalar(255, 255, 255), 1, 8);
	//	imshow("pupil", imageGray);
	//	//RotatedRect fittedEllipse = fitEllipseAMS(contours[i]);
	//}
	imshow("pupil", imageBinarized);
	HoughCircles(imageBinarized, circles, HOUGH_GRADIENT, 1, imageGray.rows / 8, 100, 20, 2, 0);
}
