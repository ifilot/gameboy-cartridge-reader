# -*- coding: utf-8 -*-

import serial
import serial.tools.list_ports
from tqdm import tqdm
import time

def main():
    ser = connect()
    read_board_id(ser)
    read_device_id(ser)
    ser.close()

def connect():
    # autofind any available boards
    ports = serial.tools.list_ports.comports()
    portfound = None
    for port in ports:
        #print(port.pid, port.vid)
        if port.pid == 54 and port.vid == 0x2341:
            portfound = port.device
            break

    # specify the COM port below
    if portfound:
        ser = serial.Serial(portfound, 
                            115200, 
                            bytesize=serial.EIGHTBITS,
                            parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE,
                            timeout=None)  # open serial port
                   
        if not ser.isOpen():
            ser.open()
    
    return ser

def read_board_id(ser):
    ser.write(b'READINFO')
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(16)
    print(rsp)

def read_device_id(ser):
    ser.write(b'DEVIDSST')
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(2)
    
    print('%02X%02X' % (rsp[0], rsp[1]))

if __name__ == '__main__':
    main()