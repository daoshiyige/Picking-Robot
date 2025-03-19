class ClawController:
    # 发送16进制数据
    def send_hex_data(self, hex_string):
        data = bytes.fromhex(hex_string.replace(' ', ''))
        self.ser.write(data)
        print(f"Sent hex data: {hex_string}")

    def __init__(self,ser):
        self.ser = ser

    def open_claw(self):
        header = b'\x41\x54'
        ID = 0x0200FD04  # 将 ID 转换为整数
        angle = b'\x96\x00'
        speed = b'\xE8\x03'

        # 对 ID 进行位移操作
        ID_shifted = (ID << 3) | 0b100

        # 将 ID_shifted 转换为字节，假设 ID 占用 4 字节
        ID_bytes = ID_shifted.to_bytes(4, byteorder='big')

        # 拼接数据
        frame = header + ID_bytes + b'\x08' + angle + b'\x00\x00' + speed + b'\x00\x00' + b'\x0d\x0a'

        # 发送数据
        self.ser.write(frame)

        # 打印发送的帧数据
        hex_string = ' '.join(f'{b:02X}' for b in frame)
        print(f"Sent frame: {hex_string}")

    def close_claw(self):
        header = b'\x41\x54'
        ID = 0x02000404  # 将 ID 转换为整数
        angle = b'\x58\x02'
        speed = b'\xE8\x03'

        # 对 ID 进行位移操作
        ID_shifted = (ID << 3) | 0b100

        # 将 ID_shifted 转换为字节，假设 ID 占用 4 字节
        ID_bytes = ID_shifted.to_bytes(4, byteorder='big')

        # 拼接数据
        frame = header + ID_bytes + b'\x08' + angle + b'\x00\x00' + speed + b'\x00\x00' + b'\x0d\x0a'

        # 发送数据
        self.ser.write(frame)

        # 打印发送的帧数据
        hex_string = ' '.join(f'{b:02X}' for b in frame)
        print(f"Sent frame: {hex_string}")
    
    def ID_read(self):
        header = b'\x41\x54'
        ID = 0x0000FD04  # 将 ID 转换为整数
        angle = b'\x01\x00'
        speed = b'\x00\x00'

        # 对 ID 进行位移操作
        ID_shifted = (ID << 3) | 0b100

        # 将 ID_shifted 转换为字节，假设 ID 占用 4 字节
        ID_bytes = ID_shifted.to_bytes(4, byteorder='big')

        # 拼接数据
        frame = header + ID_bytes + b'\x08' + angle + b'\x00\x00' + speed + b'\x00\x00' + b'\x0d\x0a'

        # 发送数据
        self.ser.write(frame)

        # 打印发送的帧数据
        hex_string = ' '.join(f'{b:02X}' for b in frame)
        print(f"Sent frame: {hex_string}")