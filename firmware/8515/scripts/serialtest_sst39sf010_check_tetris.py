# -*- coding: utf-8 -*-

import serial
import serial.tools.list_ports
from tqdm import tqdm
import time

def main():
    ser = connect()
    read_board_id(ser)
    compile_time(ser)
    
    data = bytearray()
    for i in range(8):
        data += read_block(ser, i)
    ser.close()
    
    #print_block(data, 256//8)
    
    with open('tetris.gb', 'rb') as f:
        assert(data == bytearray(f.read()))

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
                            512000, 
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

def read_device_id(ser):
    ser.write(b'DEVIDSST')
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(2)
    
    print('%02X%02X' % (rsp[0], rsp[1]))

def read_block(ser, blockid):
    ser.write(b'RDBK%04X' % (blockid * 0x1000))
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(0x1000)
    
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