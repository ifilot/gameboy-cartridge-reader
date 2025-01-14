# -*- coding: utf-8 -*-

import serial
from time import sleep

ser = serial.Serial('COM18', 
                    115200, 
                    bytesize=serial.EIGHTBITS,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_ONE,
                    timeout=1.5)  # open serial port

if not ser.isOpen():
    ser.open()

def read_cart(filename):
    data = bytearray()
    f = open(filename, 'rb')    
    data += bytearray(f.read())
    f.close()
    return data

data = read_cart(r'F:\Dropbox\Projects\Gameboy\ROMS\TETRIS.gb')

ser.write(b'READINFO')
rsp = ser.read(8)
print(rsp)
rsp = ser.read(16)
print(rsp)

datalen = 0x150
ser.write(b'READHDR0')
rsp = ser.read(8)   
print(rsp)
rsp = bytearray(ser.read(datalen))
datagot = len(rsp)
print(' '.join('{:02x}'.format(x) for x in rsp))
print("%04X" % len(rsp))

ser.write(b'RAMON000')
rsp = ser.read(8)   
print(rsp)

ser.write(b'RDBK%04X' % 0xA000)
rsp = ser.read(8)
print(rsp)
rsp = ser.read(0x1000)
print(' '.join('{:02x}'.format(x) for x in rsp[0:64]))

ser.write(b'RAMOFF00')
rsp = ser.read(8)
print(rsp)


ser.close()