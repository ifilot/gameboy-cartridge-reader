# -*- coding: utf-8 -*-

import serial
import serial.tools.list_ports
from tqdm import tqdm
import time

def main():
    ser = connect()
    read_board_id(ser)
    devid = read_device_id(ser)
    if(devid[0] != 0xBF and devid[1] != 0xB7):
        ser.close()
        return
    erase_block(ser, 0)
    data = read_block(ser, 0)
    print_block(data[0:256], 256//8)
    checkdata = write_block(ser)
    data = read_block(ser, 0)
    ser.close()
    
    print_block(data[0:256], 256//8)
    assert(checkdata[0:256] == data[0:256])

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
            
    print_block(rsp, 256//8)
    
    return rsp

def read_device_id(ser):
    ser.write(b'DEVIDSST')
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(2)
    
    print('%02X%02X' % (rsp[0], rsp[1]))
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

def write_block(ser):
    with open('tetris.gb', 'rb') as f:
        checkdata = bytearray(f.read())
        ser.write(b'WRST0000')
        rsp = ser.read(8)
        print(rsp)
        ser.write(checkdata[0:256])
        return checkdata[0:256]

def erase_block(ser, block_id):
    ser.write(b'ESST%04X' % (block_id * 0x1000))
    rsp = ser.read(8)
    print(rsp)
    rsp = ser.read(2)
    print('%02X%02X attempts' % (rsp[0], rsp[1]))
        
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