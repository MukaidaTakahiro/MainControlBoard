import serial
import time



ser = serial.Serial("COM14", 115200)
cnt = 0

high_pri_cmd = b'\xff\x00\x15\x02\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x26'
prb_set_sw_11 = b'\xff\x00\x07\x04\x01\x01\x0c'
prb_set_sw_10 = b'\xff\x00\x07\x04\x01\x00\x0b'
prb_set_sw_01 = b'\xff\x00\x07\x04\x00\x01\x0b'
prb_set_sw_00 = b'\xff\x00\x07\x04\x00\x00\x0a'
prb_cmd_list = [prb_set_sw_00, prb_set_sw_01, prb_set_sw_10, prb_set_sw_11]
#high_pri_cmd = b'\xff\x00\x06\x04\x00\x09'

while True:
    for cmd in prb_cmd_list:
        print("send:")
        print(cmd)
        ser.write(cmd)
        time.sleep(2)
        print(ser.read_all())
        print(2)


ser.close()