#pragma once
#include "stdafx.h"

#pragma comment(lib,"ws2_32.lib")  

class socketCommunication
{
public:
	struct pngDataType{
		pngDataType(cv::Mat imgMat);
		~pngDataType();
		uint bytes = 0;
		uchar* pData = NULL;
	};
	socketCommunication();
	~socketCommunication();
	int sendData(const char* data, int bytes=0);
	int sendData(const pngDataType &data);
	int recvData(char * data, int bytes=0);
private:
	sockaddr_in serAddr;
	SOCKET sclient;
};

