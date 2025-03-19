import serial
import time

global serdev
serdev = serial.Serial(
        port='/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0',  # 根据实际情况修改串口
        baudrate=921600,      # 串口波特率
        timeout=1
    )

init_string_data = [             #初始化TTL转CAN
    "AT+CG",
    "AT+CAN_FRAMEFORMAT=1,0,291,0",
    "AT+CAN_BAUD=1000000",
    "AT+CAN_MODE=0",
    "AT+AT"
]

init_hex_data = [                    #初始化小米电机
    "41 54 30 07 e8 0c 08 01 01 00 00 00 00 00 00 0d 0a",#设置机械零位
    "41 54 90 07 e8 0c 08 05 70 00 00 01 00 00 00 0d 0a",#设置为位置模式
    "41 54 18 07 e8 0c 08 00 00 00 00 00 00 00 00 0d 0a",#启动电机
    "41 54 90 07 e8 0c 08 17 70 00 00 00 00 00 3f 0d 0a",#设置位置模式速度最大值
    "41 54 30 07 e8 14 08 01 01 00 00 00 00 00 00 0d 0a",
    "41 54 90 07 e8 14 08 05 70 00 00 01 00 00 00 0d 0a",
    "41 54 18 07 e8 14 08 00 00 00 00 00 00 00 00 0d 0a",
    "41 54 90 07 e8 14 08 17 70 00 00 00 00 00 3f 0d 0a"
]

def send_hex_data(hex_string):
    data = bytes.fromhex(hex_string.replace(' ', ''))
    serdev.write(data)
    print(f"Sent hex data: {hex_string}")


for data in init_string_data:
    serdev.write((data + "\r\n").encode('utf-8'))
    print(f"Sent string data: {data}")
    time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令

for data in init_hex_data:
    send_hex_data(data)
    time.sleep(0.1)  # 添加延时以确保设备有足够的时间处理每条命令