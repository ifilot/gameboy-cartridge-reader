# -*- coding: utf-8 -*-

import serial
import serial.tools.list_ports
from tqdm import tqdm
import time

def main():
    ser = connect()
    read_board_id(ser)
    compile_time(ser)
    read_header(ser)
    #read_block(ser, 0)
    #read_cartridge_32k(ser)
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
    
    print_block(rsp, 0x150//8)
            
def read_block(ser, blockid):
    ser.write(b'RDBK%04X' % (blockid * 0x1000))
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(0x1000)
            
    print_block(rsp, 8)

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

def read_cartridge_32k(ser):
    data = bytearray()
    for i in range(0,8):
        ser.write(b'RDBK%04X' % (i * 0x1000))
        rsp = ser.read(8)
        print(rsp)
        rsp = ser.read(0x1000)
        data += rsp
        
    with open('tetris.gb', 'rb') as f:
        checkdata = bytearray(f.read())
    
    print(data == checkdata)

if __name__ == '__main__':
    main()