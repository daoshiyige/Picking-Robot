import serial
import threading
import time
import math
import struct

class MiMotorController:
# 初始化数据列表

    def __init__(self, ser):
        # 发送初始化数据
        self.ser = ser
        self.radian1 = 0
        self.radian2 = 0
        self.id1=1
        self.id2=2


    def set_zero_position(self,id):
        """根据 ID 发送不同的指令"""
        if id == self.id1:
            commands_to_send = [self.init_hex_data[0]]  # 发送第 1 条指令
            self.radian1=0
        elif id == self.id2:
            commands_to_send = [self.init_hex_data[4]]  # 发送第 5 条指令
            self.radian2=0
        else:
            print("无效的 ID")
            return
        for command in commands_to_send:
            self.send_hex_data(command)
            time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令
    
    def float_to_bytes(self,value):
        """将浮点数转换为字节并反向"""
        # 将浮点数转换为字节
        byte_data = struct.pack('>f', value)
        # 反转字节顺序
        return byte_data[::-1]
    
    # 发送特定格式的数据
    def set_current_position(self,electrode_id, radian):
        try:
            # 将电极ID转为十六进制
            #electrode_id = int(electrode_id, 16)
            
            # 计算32位扩展ID
            id_field = (0b100100000000011111101 << 8) | electrode_id
            id_field = (id_field << 3) | 0b100  # 添加2~0位为100
            id_field_bytes = id_field.to_bytes(4, 'big')

            if electrode_id==self.id1:
                self.radian1=radian
            elif electrode_id==self.id2:
                self.radian2=radian    
            # 将弧度转换为4字节的浮点数数据，并反向字节顺序
            radian = float(radian)
            radian_bytes = self.float_to_bytes(radian)

            # 拼接数据
            frame = b'\x41\x54' + id_field_bytes + b'\x08' + b'\x16' + b'\x70' + b'\x00' + b'\x00' + radian_bytes + b'\x0d\x0a' 

            # 发送数据
            self.ser.write(frame)
            hex_string = ' '.join(f'{b:02X}' for b in frame)
            print(f"Sent frame: {hex_string}")

        except ValueError as e:
            print(f"转换错误: {e}")
    
    def set_current_position_to_current(self,electrode_id,radian):
        try:
            # 将电极ID转为十六进制
            #electrode_id = int(electrode_id, 16)
            
            # 计算32位扩展ID
            id_field = (0b100100000000011111101 << 8) | electrode_id
            id_field = (id_field << 3) | 0b100  # 添加2~0位为100
            id_field_bytes = id_field.to_bytes(4, 'big')

            if electrode_id==self.id1:
                self.radian1+=radian
                radian=self.radian1
            elif electrode_id==self.id2:
                self.radian2+=radian  
                radian=self.radian2
            # 将弧度转换为4字节的浮点数数据，并反向字节顺序
            radian = float(radian)
            radian_bytes = self.float_to_bytes(radian)

            # 拼接数据
            frame = b'\x41\x54' + id_field_bytes + b'\x08' + b'\x16' + b'\x70' + b'\x00' + b'\x00' + radian_bytes + b'\x0d\x0a' 

            # 发送数据
            self.ser.write(frame)
            hex_string = ' '.join(f'{b:02X}' for b in frame)
            print(f"Sent frame: {hex_string}")

        except ValueError as e:
            print(f"转换错误: {e}")
    
    def get_current_position(self,electrode_id):
        if electrode_id==self.id1:
            return self.radian1
        elif electrode_id==self.id2:
            return self.radian2

# 使用示例
if __name__ == "__main__":
    # 创建 MotorController 实例
    run = MiMotorController()
    time.sleep(10)
    # 设置电机 1 的当前位置为 0.5
    run.set_current_position(1, 0.5)