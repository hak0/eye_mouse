#include "socket.h"



socketCommunication::socketCommunication()
{
	// initialize WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		std::cout << "WSA initialization failed!" << std::endl;
	}
	sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(11451);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	while (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
		std::cout << "Connection error! Please run cnn_backend.py." << std::endl;
		closesocket(sclient);
		_sleep(3000); //wait for 3s
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(11451);
		serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	}
}

socketCommunication::~socketCommunication()
{
	closesocket(sclient);
	WSACleanup();
}

int socketCommunication::sendData(const char * data, int bytes)
{
	if (bytes == 0)
		bytes = strlen(data);
	return send(sclient, data, bytes, 0);
	//send()用来将数据由指定的socket传给对方主机
	//int send(int s, const void * msg, int len, unsigned int flags)
	//s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
	//成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error 
}

int socketCommunication::sendData(const pngDataType & data)
{
	unsigned int len = data.bytes;
	int retVal = send(sclient, (char*)data.pData, len, 0);
	if (retVal <= 0)
		std::cout << "Sending failed!" << std::endl;
	return retVal;
}
int socketCommunication::recvData(char * data, int bytes)
{

	int retVal = recv(sclient, data, sizeof(data)/sizeof(data[0]), 0);
	if (retVal <= 0)
		std::cout << "Receiving failed!" << std::endl;
	return retVal;
}
socketCommunication::pngDataType::pngDataType(cv::Mat imgMat)
{
	std::vector<uchar> outImgEncode;
	// convert mat to png image
	cv::imencode(".png", imgMat, outImgEncode);
	bytes = outImgEncode.size();
	pData = reinterpret_cast<uchar*>(outImgEncode.data());
}

socketCommunication::pngDataType::~pngDataType()
{
	pData = NULL;
}
