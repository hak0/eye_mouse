#pragma once

void dlibPointsDetect(cv::Mat frame, std::vector<cv::Point2d>& landmarks, dlib::frontal_face_detector & detector, dlib::shape_predictor & pose_model);

const double DLIB_SCALE_RATIO = 0.75;
