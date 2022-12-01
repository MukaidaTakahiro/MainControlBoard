import serial
import time

ser = serial.Serial("COM13", 115200)
cnt = 0

#high_pri_cmd = b'\xff\x00\x15\x02\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x26'
high_pri_cmd = b'\xff\x00\x06\x04\x00\x09'

while True:
    #cnt = (cnt + 1) % 100
    #send_str = cnt.to_bytes(1, 'big')
    #ser.write(send_str)
    print("send:")
    print(high_pri_cmd)
    ser.write(high_pri_cmd)
    time.sleep(5)
    print(ser.read_all())
    print(2)


ser.close()