import serial
import time

class BaseMotorController:
    def __init__(self, ser):
        self.ser = ser
        self.id = 10

    def setspeed(self, speed):
        extid  = 0b110 << 24 | 0x0C << 16 | 0x08 << 8 | self.id
        extid = extid << 3 | 0b100
        frame = b'\x41\x54' + extid.to_bytes(4, 'big')  + b'\x08' + speed[0].to_bytes(2, "big") + speed[1].to_bytes(2, "big") + speed[2].to_bytes(2,"big") + speed[3].to_bytes(2, "big") + b'\x0d\x0a'
        self.ser.write(frame)

