#include "stdafx.h"
#include "cursorAndKey.h"

#define  isKeyPressed(nVirtKey)     ((GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0)
cv::Point2d borderPointRelativeCoordinate[2][4]; // [2] stands for right&left eyes, and [4] for four border points

cv::Point2d getCursorLocation() {
	POINT p;
	GetCursorPos(&p);
	cv::Point2d cP(p.x, p.y);
	return cP;
}

bool onReleaseSpaceKey() {
	static bool isPressedFlag = 0;
	if (!isPressedFlag && isKeyPressed(VK_SPACE)) {
		isPressedFlag = !isPressedFlag;
	}
	if (isPressedFlag && !isKeyPressed(VK_SPACE)) {
		isPressedFlag = !isPressedFlag;
		return true;
	}
	return false;
}

bool onReleaseCtrlKey() {
	static bool isPressedFlag = 0;
	if (!isPressedFlag && isKeyPressed(VK_CONTROL)) {
		isPressedFlag = !isPressedFlag;
	}
	if (isPressedFlag && !isKeyPressed(VK_CONTROL)) {
		isPressedFlag = !isPressedFlag;
		return true;
	}
	return false;
}

bool onReleaseAltKey() {
	static bool isPressedFlag = 0;
	if (!isPressedFlag && isKeyPressed(VK_MENU)) {
		isPressedFlag = !isPressedFlag;
	}
	if (isPressedFlag && !isKeyPressed(VK_MENU)) {
		isPressedFlag = !isPressedFlag;
		return true;
	}
	return false;
}

void moveCursor(dualEye & eyes, mappingModel &model) {
	int screenWidth = GetSystemMetrics ( SM_CXSCREEN ); 
	int screenHeight= GetSystemMetrics ( SM_CYSCREEN ); 
	cv::Point2i absoluteCusorLocation = model.generateScreenPoint(eyes.getRelativeCoordinate());
	std::cout << "absoluteCusorLocation" << absoluteCusorLocation << std::endl;
	// Windows mouse_event api's absolute mouse position is different from what we get.
	// It divides screen to 65536*65536
	if (absoluteCusorLocation.x < 0) absoluteCusorLocation.x = 0;
	if (absoluteCusorLocation.y < 0) absoluteCusorLocation.y = 0;
	if (absoluteCusorLocation.x > screenWidth) absoluteCusorLocation.x = screenWidth;
	if (absoluteCusorLocation.y > screenHeight) absoluteCusorLocation.y = screenHeight;
	SetCursorPos(absoluteCusorLocation.x, absoluteCusorLocation.y);
}

void moveCursor(socketCommunication &com) {
	// receives relative value
	double_t x, y; 
	double_t pData[1];
	com.recvData((char*)pData, 8);
	x = pData[0] * screen_width;
	com.recvData((char*)pData, 8);
	y = pData[0] * screen_height;
	std::cout << "x: " << x / screen_width << " y: " << y/screen_height << std::endl;
	std::cout << "absx: " << x<< " absy: " << y << std::endl;
	if (x == 0 && y == 0) return;
	SetCursorPos(x, y);
}


void sendCursorLocation(socketCommunication &com) {
	// relative value, always in [0,1]
	cv::Point2d cursorLoc= getCursorLocation();
	double_t x = cursorLoc.x / screen_width, y = cursorLoc.y / screen_height, pData[2]; 
	*pData = x;
	*(pData + 1) = y;
	com.sendData((char*)pData, 16);
}
