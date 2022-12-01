
import socket

HOST_NAME = "192.168.22.11"
PORT = 30704

#ipv4を使うので、AF_INET
#tcp/ip通信を使いたいので、SOCK_STREAM
sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#サーバーと接続
sock.connect((HOST_NAME,PORT))
#データ送信
sock.send(b"\x1B\x01\x00\x00\x00\x01\x00\x00\x00")
#サーバーからデータ受信
rcv_data = sock.recv(1024)
print("receive data : {}".format(rcv_data))
sock.close()