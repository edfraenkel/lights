#!/usr/bin/env python3
import serial
import time

# configure the serial connections (the parameters differs on the device you are connecting to)
def open_serial():
    return serial.Serial(
        port='/dev/ttyACM1',
        baudrate=9600,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )


print('Enter your commands below.\r\nInsert "exit" to leave the application.')

while True:
    # get keyboard input
    command = input(">> ")
    # connect
    ser = open_serial()
    if not ser.isOpen():
        try:
            ser.open()
        except Exception as e:
            print(e)
    time.sleep(0.1)
    # send the character to the device
    ser.write(bytes(command, "UTF8"))
    out = bytes("", "UTF8")
    time.sleep(0.1)
    while ser.inWaiting() > 0:
        out += ser.read(1)
    if out != '':
        print(str(out, "UTF8"))
    time.sleep(0.1)
    ser.close()


