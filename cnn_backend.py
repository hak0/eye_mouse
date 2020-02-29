#!/usr/bin/python3
# 文件名：server.py

# 导入 socket、sys 模块
import socket
import sys
import numpy as np
from matplotlib import pyplot as plt
import cv2
np.random.seed(1337)  
from keras.datasets import mnist
from keras.utils import np_utils
from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout, Conv2D, MaxPooling2D, Flatten
from keras.optimizers import RMSprop, Adam
from math import floor
from struct import pack


def train_network(X, y):
    X = X / 255. 
    X = np.reshape(X, X.shape+(1,))
    model = Sequential([
        # LookieLookie model
        Conv2D(filters=20, kernel_size=5, strides=(1,1), activation="relu", input_shape=(64,64,1)),
        # Conv2D(filters=40, kernel_size=5, strides=(1,1), activation="relu", input_shape=(64,64,1)),
        MaxPooling2D(pool_size=(2,2), strides=(2,2)),
        Flatten(),
        Dropout(0.2),
        Dense(units=2, activation='tanh')
    ])
    # 定义优化器
    # rmsprop = RMSprop(lr=0.001, rho=0.9, epsilon=1e-08, decay=0.0)
    adam = Adam(lr=0.0005)
    model.compile(optimizer=adam,
                loss='mean_squared_error',
                metrics=['accuracy']) # metrics赋值为'accuracy'，会在训练过程中输出正确率
    # 这次我们用fit()来训练网路
    print('Training ------------')
    batch_size = floor(X.shape[0])
    if (batch_size < 4):
        batch_size = 4
    if (batch_size > 64):
            batch_size = 64
    model.fit(X, y, epochs=20, batch_size=batch_size, validation_split=0.2)
    print('\nTesting ------------')
    # 评价训练出的网络
    # loss, accuracy = model.evaluate(X_test, y_test)
    # print('test loss: ', loss)
    # print('test accuracy: ', accuracy)
    return model

def network_predict(model, x_input):
    x_input = np.reshape(x_input, x_input.shape+(1,))
    x_input = x_input / 255. 
    return model.predict(x_input)


def main():
    # 创建 socket 对象
    serversocket = socket.socket(
                socket.AF_INET, socket.SOCK_STREAM) 
    print("server is ready.")
    # 获取本地主机名
    # host = socket.gethostname()
    host = "127.0.0.1"
    port = 11451
    # 绑定端口号
    serversocket.bind((host, port))
    # 设置最大连接数，超过后排队
    serversocket.listen(5)
    # 建立客户端连接
    clientsocket,addr = serversocket.accept()      
    print("new connection: %s" % str(addr))

    x_list = []
    y_list = []
    stop = False
    while not stop:
        # We promise that the length of each command is 7 bytes.
        command = clientsocket.recv(7).decode('ascii')
        print('command:', command)
        if command == 'calib  ':
            # add points for training
            # read screen cursor relative location
            msg = clientsocket.recv(16)
            cursorLoc = np.frombuffer(msg, dtype=np.float64)
            print(cursorLoc)

            msg = clientsocket.recv(4096)
            imgbuf = np.frombuffer(msg, dtype=np.uint8)
            print(imgbuf.shape)
            img = cv2.imdecode(imgbuf,cv2.IMREAD_GRAYSCALE)

            if (type(cursorLoc) is np.ndarray and type(img) is np.ndarray):
                x_list.append(img)
                y_list.append(cursorLoc)
        elif command == 'train  ':
            X = np.array(x_list)
            y = np.array(y_list)
            model = train_network(X, y)
            clientsocket.send("OK".encode('ansi'))
        elif command == 'predict':
            msg = clientsocket.recv(4096)
            imgbuf = np.frombuffer(msg, dtype=np.uint8)
            print(imgbuf.shape)
            img = cv2.imdecode(imgbuf,cv2.IMREAD_GRAYSCALE)
            if (type(img) is np.ndarray):
                x_new = np.array(img).reshape((1,)+img.shape)
                y_new = network_predict(model, x_new).astype(np.float64);
            else:
                y_new = np.zeros((1,2), dtype=np.float64)
            print('y1, y2',y_new)
            clientsocket.send(y_new[0,0].tobytes())
            clientsocket.send(y_new[0,1].tobytes())

        elif command == 'exit   ':
            stop = True;
    clientsocket.close()

if __name__ == "__main__":
    main()
