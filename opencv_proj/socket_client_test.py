#!/usr/bin/python3
# 文件名：client.py

# 导入 socket、sys 模块
import socket
import sys

# 创建 socket 对象
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

# 获取本地主机名
#host = socket.gethostname() 
#print(host)
host = "127.0.0.1"
#print(host)

# 设置端口号
port = 11451

# 连接服务，指定主机和端口
s.connect((host, port))

# 接收小于 1024 字节的数据
msg='hello world!'+ "\r\n"
s.send(msg.encode('ascii'))


s.close()
