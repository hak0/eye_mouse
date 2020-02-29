#pragma once

#include "mapping.h"
#include "eye.h"
cv::Point2d getCursorLocation();

bool onReleaseSpaceKey();

bool onReleaseCtrlKey();

bool onReleaseAltKey();

void moveCursor(dualEye & eyes, mappingModel & model);
void moveCursor(socketCommunication & com);

void sendCursorLocation(socketCommunication & com);

const int screen_height = 2160;
const int screen_width = 4096;
