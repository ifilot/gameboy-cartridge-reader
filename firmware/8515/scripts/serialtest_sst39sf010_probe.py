# -*- coding: utf-8 -*-

import serial
import serial.tools.list_ports
from tqdm import tqdm
import time

def main():
    ser = connect()
    read_board_id(ser)
    compile_time(ser)
    res = read_header(ser)
    print_block(res, 0x150 // 8)
    ser.close()

def connect():
    # autofind any available boards
    ports = serial.tools.list_ports.comports()
    portfound = None
    for port in ports:
        #print(port.pid, port.vid)
        if port.pid == 0x6001 and port.vid == 0x0403:
            portfound = port.device
            print('Found Device: %s' % portfound)
            break

    # specify the COM port below
    if portfound:
        ser = serial.Serial(portfound, 
                            14400, 
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
    
def compile_time(ser):
    ser.write(b'COMPTIME')
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(16)
    print(rsp)
    rsp = ser.read(16)
    print(rsp)

def read_header(ser):
    ser.write(b'READHDR0')
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(0x150)
    return rsp
    
def print_block(data, nr_lines):
    for i in range(nr_lines):
        for j in range(8):
            print('%02X ' % data[i*8+j], end='')
        print('  ', end='')
        for j in range(8):
            if data[i*8+j] > 32 and data[i*8+j] < 126:
                print(chr(data[i*8+j]), end='')
            else:
                print('.', end='')
        print()

if __name__ == '__main__':
    main()