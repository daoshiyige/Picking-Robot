import serial
import threading
import time
import math
import struct


class StepperMotor1:
    # 发送16进制数据
    def send_hex_data(self, hex_string):
        data = bytes.fromhex(hex_string.replace(' ', ''))
        self.ser.write(data)
        print(f"Sent hex data: {hex_string}")

    # 发送字符串数据
    def send_string_data(self, data):
        self.ser.write((data + "\r\n").encode('utf-8'))
        print(f"Sent string data: {data}")

    def __init__(self, ser):
        self.ser = ser

    def float_to_bytes(self, value):
        """将浮点数转换为字节并反向"""
        # 将浮点数转换为字节
        byte_data = struct.pack('>f', value)
        # 反转字节顺序
        return byte_data[::-1]

    def send_reset(self):
        frame = b'\x41\x54\x90\x00\xD1\xE4\x08' + b'\x70\x20\x00\x00\x00\x00\x00\x00' + b'\x0D\x0A'
        self.ser.write(frame)
        hex_string = ' '.join(f'{b:02X}' for b in frame)
        print(f"Sent frame: {hex_string} \r\nReset StepperMoter")

    def send_angel_control(self, dir, angel):
        angel = float(angel)
        angel_byte = self.float_to_bytes(angel)
        if dir == "l":
            frame = b'\x41\x54\x90\x00\xD1\xE4\x08' + b'\x70\x18\x00\x00' + angel_byte + b'\x0D\x0A'
            hex_string = ' '.join(f'{b:02X}' for b in frame)
            self.ser.write(frame)
            print(f"Sent frame: {hex_string} \r\nTurn Left {angel} degree")
        elif dir == "r":
            frame = b'\x41\x54\x90\x00\xD1\xE4\x08' + b'\x70\x19\x00\x00' + angel_byte + b'\x0D\x0A'
            hex_string = ' '.join(f'{b:02X}' for b in frame)
            self.ser.write(frame)
            print(f"Sent frame: {hex_string} \r\nTurn Right {angel} degree")

    def send_turn_to_position(self, position):
        position = float(position)
        position_byte = self.float_to_bytes(position)
        frame = b'\x41\x54\x90\x00\xD1\xE4\x08' + b'\x70\x16\x00\x00' + position_byte + b'\x0D\x0A'
        self.ser.write(frame)
        hex_string = ' '.join(f'{b:02X}' for b in frame)
        print(f"Sent frame: {hex_string} \r\nTurn to {position} degree")

    def send_speed(self, speed):
        speed = float(speed)
        speed_byte = self.float_to_bytes(speed)
        frame = b'\x41\x54\x90\x00\xD1\xE4\x08' + b'\x70\x17\x00\x00' + speed_byte + b'\x0D\x0A'
        self.ser.write(frame)
        hex_string = ' '.join(f'{b:02X}' for b in frame)
        print(f"Sent frame: {hex_string} \r\nSet Speed: {speed} ")

    def working_thread(self):
        try:
            while True:
                user_input = input("请输入控制指令:")
                command = user_input
                if command == "reset":
                    self.send_reset()
                elif command == "turn":
                    param = input("请输入转向left/right:   请输入转动角度(°):")
                    dir_input, degree_input = param.split()
                    self.send_angel_control(dir_input, degree_input)
                elif command == "turn to":
                    param = input("请输入转到的绝对角度(°):")
                    self.send_turn_to_position(param)
                elif command == "speed":
                    param = input("请输入设置的速度(rad):")
                    self.send_speed(param)
        except KeyboardInterrupt:
            print("程序终止")