#include "stdafx.h"
#include "dlibPointsDetect.h"


void dlibPointsDetect(cv::Mat frame, std::vector<cv::Point2d> &landmarks,
	dlib::frontal_face_detector &detector,
	dlib::shape_predictor &pose_model) {
	using namespace dlib;

	cv_image<dlib::bgr_pixel> cimg(frame);
	std::vector<rectangle> faces = detector(cimg);
	// Find the pose of each face.
	std::vector<full_object_detection> shapes;
	if (faces.size() != 1) {
		return;
	} else {
		full_object_detection shape = pose_model(cimg, faces[0]);
		shapes.push_back(shape);
		// Dlib's feature points are arranged in orderso we can get 6 landmarks
		// check https://blog.csdn.net/qq_39438636/article/details/79304130 for points and their numbers
		// below by id
		landmarks.push_back(cv::Point2d(
			shape.part(17).x() / DLIB_SCALE_RATIO,
			shape.part(17).y() / DLIB_SCALE_RATIO)); //#17 
		landmarks.push_back(cv::Point2d(
			shape.part(21).x() / DLIB_SCALE_RATIO,
			shape.part(21).y() / DLIB_SCALE_RATIO)); //#21
		landmarks.push_back(cv::Point2d(
			shape.part(22).x() / DLIB_SCALE_RATIO,
			shape.part(22).y() / DLIB_SCALE_RATIO)); //#22 
		landmarks.push_back(cv::Point2d(
			shape.part(26).x() / DLIB_SCALE_RATIO,
			shape.part(26).y() / DLIB_SCALE_RATIO)); //#26 
		landmarks.push_back(cv::Point2d(
			shape.part(36).x() / DLIB_SCALE_RATIO,
			shape.part(36).y() / DLIB_SCALE_RATIO)); //#36 
		landmarks.push_back(cv::Point2d(
			shape.part(37).x() / DLIB_SCALE_RATIO,
			shape.part(37).y() / DLIB_SCALE_RATIO)); //#37 
		landmarks.push_back(cv::Point2d(
			shape.part(38).x() / DLIB_SCALE_RATIO,
			shape.part(38).y() / DLIB_SCALE_RATIO)); //#38 
		landmarks.push_back(cv::Point2d(
			shape.part(39).x() / DLIB_SCALE_RATIO,
			shape.part(39).y() / DLIB_SCALE_RATIO)); //#39 
		landmarks.push_back(cv::Point2d(
			shape.part(40).x() / DLIB_SCALE_RATIO,
			shape.part(40).y() / DLIB_SCALE_RATIO)); //#40 
		landmarks.push_back(cv::Point2d(
			shape.part(41).x() / DLIB_SCALE_RATIO,
			shape.part(41).y() / DLIB_SCALE_RATIO)); //#41 
		landmarks.push_back(cv::Point2d(
			shape.part(42).x() / DLIB_SCALE_RATIO,
			shape.part(42).y() / DLIB_SCALE_RATIO)); //#42 
		landmarks.push_back(cv::Point2d(
			shape.part(43).x() / DLIB_SCALE_RATIO,
			shape.part(43).y() / DLIB_SCALE_RATIO)); //#43 
		landmarks.push_back(cv::Point2d(
			shape.part(44).x() / DLIB_SCALE_RATIO,
			shape.part(44).y() / DLIB_SCALE_RATIO)); //#44 
		landmarks.push_back(cv::Point2d(
			shape.part(45).x() / DLIB_SCALE_RATIO,
			shape.part(45).y() / DLIB_SCALE_RATIO)); //#45 
		landmarks.push_back(
			cv::Point2d(shape.part(46).x() / DLIB_SCALE_RATIO,
				shape.part(46).y() / DLIB_SCALE_RATIO)); //#46
		landmarks.push_back(
			cv::Point2d(shape.part(47).x() / DLIB_SCALE_RATIO,
				shape.part(47).y() / DLIB_SCALE_RATIO)); //#47
		landmarks.push_back(
			cv::Point2d(shape.part(48).x() / DLIB_SCALE_RATIO,
				shape.part(48).y() / DLIB_SCALE_RATIO)); //#48
		landmarks.push_back(
			cv::Point2d(shape.part(54).x() / DLIB_SCALE_RATIO,
				shape.part(54).y() / DLIB_SCALE_RATIO)); //#54 mouth right corner
		landmarks.push_back(cv::Point2d(
			shape.part(57).x() / DLIB_SCALE_RATIO,
			shape.part(57).y() / DLIB_SCALE_RATIO)); //#57 mouth central bottom corner
		landmarks.push_back(
			cv::Point2d(shape.part(8).x() / DLIB_SCALE_RATIO,
				shape.part(8).y() / DLIB_SCALE_RATIO)); //#8 chin corner
	}
}