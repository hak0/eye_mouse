#include "stdafx.h"
#include "dlibPointsDetect.h"
#include "socket.h"
#include "eye.h"
#include "cursorAndKey.h"
#include "main.h"
const cv::String window_name = "Capture - Face detection";


int main() {
	socketCommunication com;
	cv::Mat frame;
	cv::Mat frame_small;
	mappingModel eyeToScreenModel;
	cv::VideoCapture cap;
	// Load dlib face detection and pose estimation models
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	dlib::shape_predictor pose_model;
	dlib::deserialize("data/"
		"shape_predictor_68_face_landmarks.dat") >>
		pose_model;

	// Read the stream
	//cap.open("data/test.mp4"); // load test video
	cap.open(0); // load the local web camera
	if (!cap.isOpened()) {
		return -1;
	}

	std::cout << "First we need data to train the model." << std::endl;
	std::cout << "Move your mouse and look at the cursor, then press Space Key to add sample data." << std::endl;

	bool stop = false;
	int pointsCnt = 0;
	bool isTrained = false;
	bool mouseControlActivated = false;
	while (!stop) {
		std::vector<cv::Point2d> landmarks;
		cap >> frame;
		if (frame.empty())
			break;
		// Change the frame size to accelerate dlib processing
		cv::resize(frame, frame_small, frame_small.size(), DLIB_SCALE_RATIO, DLIB_SCALE_RATIO, 1);
		// Detect feature points
		dlibPointsDetect(frame_small, landmarks, detector, pose_model);
		// Find pupil points
		if (!landmarks.empty()) {
			dualEye eyes(landmarks); //it will automatically find border points for each eye from landmarks
			if (eyes.pointsAreValid(frame)) {
				if (!mouseControlActivated && onReleaseSpaceKey()) {
					com.sendData("calib  ");
					sendCursorLocation(com);
					eyes.sendEyeNormarizedImage(frame, com);
					std::cout << "We have collected " << ++pointsCnt
						<< (pointsCnt == 1 ? " point." : " points.") << std::endl;
					if (pointsCnt >= 20 && !isTrained) {
						std::cout << "Now you can train the model by pressing Ctrl Key!" << std::endl;
					}
					if (pointsCnt > 20 && pointsCnt % 20 == 0 && isTrained) {
						std::cout << "You can start a new train to improve the model." << std::endl;
					}
				}
				if (onReleaseCtrlKey()) {
					mouseControlActivated = false;
					if (pointsCnt < 20) {
						std::cout << "Points are not enough. Please add at least 20 points." << std::endl;
					}
					else {
						if (!isTrained)
							std::cout << "Start training. It will take a while." << std::endl;
						else
							std::cout << "Improving the model. It will take a while." << std::endl;
						com.sendData("train  ");
						char retVal[5];
						com.recvData(retVal);
						std::cout << "Training completed!" << std::endl;
						isTrained = true;
					}
				}
				if (onReleaseAltKey()) {
					if (isTrained)
						mouseControlActivated = !mouseControlActivated;
					std::cout << "Mouse control is " << (!mouseControlActivated ? "OFF" : "ON") << std::endl;
				}
				if (mouseControlActivated) {
					com.sendData("predict");
					eyes.sendEyeNormarizedImage(frame, com);
					moveCursor(com);
				}
			}
		}
		// Draw dlib feature points
		for (auto &it : landmarks) {
			cv::circle(frame, it, 4, cv::Scalar(0, 0, 255), -1);
		}
		//_sleep(100); //slow down the process to diagnose
		cv::imshow(window_name, frame);
		if (cv::waitKey(30) == 27) { //Pressed 'Esc' key
			com.sendData("exit   ");
			stop = true;
		}
	}
	return 0;
}

void getPredictedCursorLocation(socketCommunication &com) {
	// It receives relative value, but will convert the location to absolute coordinates 

}
