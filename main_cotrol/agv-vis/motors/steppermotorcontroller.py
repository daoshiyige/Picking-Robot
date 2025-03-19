import time
import struct
import serial

class SteppermotorController:
    # 发送16进制数据
    def send_hex_data(self, hex_string):
        data = bytes.fromhex(hex_string.replace(' ', ''))
        self.ser.write(data)
        print(f"Sent hex data: {hex_string}")

    # 发送字符串数据
    def send_string_data(self, data):
        self.ser.write((data + "\r\n").encode('utf-8'))
        print(f"Sent string data: {data}")

    def __init__(self,ser):
        self.ser = ser
        self.init_hex_data = [
            "41 54 00 07 e8 1c 08 17 70 00 00 00 00 80 3f 0d 0a"#设置角速度为1rad/s
        ]
        # 发送16进制初始化数据
        for data in self.init_hex_data:
            self.send_hex_data(data)
            time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令
        self.position=0

    def set_zero_position_init(self):
        for data in self.init_hex_data:
            self.send_hex_data(data)
            time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令
        ZERO_Data = [
            "41 54 00 07 e8 1c 08 16 70 00 00 9a 99 59 bf 0d 0a"
        ]
        for zero in ZERO_Data :
            self.send_hex_data(zero) 
            time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令
        self.position=0

    def set_zero_position(self):
        self.position=0

    def float_to_bytes(self,value):
        """将浮点数转换为字节并反向"""
        # 将浮点数转换为字节
        byte_data = struct.pack('>f', value)
        # 反转字节顺序
        return byte_data[::-1]
    
    def set_current_position_to_current(self,radian):#增减多少弧度
        try:
            self.position += radian 
            radian = float(radian)
            radian_bytes = self.float_to_bytes(radian)

            #拼接数据
            frame = b'\x41\x45\x00\x07\xe8\x1c\x08\x16\x70\x00\x00'+ radian_bytes + b'\x0d\x0a' 
            
            # 发送数据
            for data in self.init_hex_data:
                self.send_hex_data(data)
                time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令
            self.ser.write(frame)
            hex_string = ' '.join(f'{b:02X}' for b in frame)
            print(f"Sent frame: {hex_string}")  
        except ValueError as e:
            print(f"转换错误: {e}")
    
    def set_current_position(self,radian): #到达指定弧度
        try:
            radian = radian - self.position
            self.position += radian 
            radian = float(radian)
            radian_bytes = self.float_to_bytes(radian)

            #拼接数据
            frame = b'\x41\x45\x00\x07\xe8\x1c\x08\x16\x70\x00\x00'+ radian_bytes + b'\x0d\x0a' 
            
            # 发送数据
            for data in self.init_hex_data:
                self.send_hex_data(data)
                time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令
            self.ser.write(frame)
            hex_string = ' '.join(f'{b:02X}' for b in frame)
            print(f"Sent frame: {hex_string}")  
        except ValueError as e:
            print(f"转换错误: {e}")