# device initialization
import time
from motors import *
from capture.capture import *
# 2.63s for 50 cm
# 4.83s for 100 cm
# reverse:
# 12s for ~250cm 

def input_to_output(Z0, Y0, delta_Z, delta_Y):
    Rz = (Z0 + delta_Z) / 40
    Ry = (Y0 + delta_Y) / 40

    # 检查输入是否在有效范围内
    if Rz ** 2 + Ry ** 2 > 1:
        print("输入超出有效范围，无解。")
        return None

    # 计算α
    alpha = 2 * np.arctan2(Ry, Rz)

    # 计算β
    beta = 2 * np.arccos(Rz / (2 * np.cos(alpha / 2)))

    # 考虑β的正负两种情况
    beta_plus = beta
    beta_minus = -beta

    # 计算θ1和θ2
    theta1_plus = (alpha + beta_plus) / 2
    theta2_plus = (alpha - beta_plus) / 2

    theta1_minus = (alpha + beta_minus) / 2
    theta2_minus = (alpha - beta_minus) / 2

    return [(theta1_plus, theta2_plus), (theta1_minus, theta2_minus)]

testcontroller0 = BaseMotorController(serdev)
testcontroller1 = ClawController(serdev)
steppermotor = StepperMotor1(serdev)
testcontroller2 = MiMotorController(serdev)
testcontroller1.ID_read()
# # forward: testcontroller.setspeed([0xffff, 0xffff, 0x7f00 -0x027f, 0x7f00 - 0x027f])


while True:
    testcontroller1.open_claw()
    Z0 = 40 * np.cos(np.radians(30)) + 40 * np.cos(np.radians(10))
    Y0 = 40 * np.sin(np.radians(30)) + 40 * np.sin(np.radians(10))
    target_found, x, y, z = TargetCapture()
    if target_found:
        testcontroller0.setspeed([0x8080, 0x8080, 0x8080, 0x8080])
        if x > 0.01:
            steppermotor.send_angel_control("r", 10)
        elif x < -0.01:
            steppermotor.send_angel_control("l", 10)
        else:
            result = input_to_output(Z0, Y0, z, y)
            if result:
                theta1_plus, theta2_plus = result[0]
                # 将角度转换为弧度
                theta1_plus_rad = np.radians(theta1_plus + 30)
                theta2_plus_rad = np.radians(theta2_plus - 10)
                testcontroller2.set_current_position(1, theta1_plus_rad)
                testcontroller2.set_current_position(2, theta2_plus_rad)
                time.sleep(5)
                testcontroller1.close_claw()
                time.sleep(2)
                testcontroller2.set_current_position(1, 0)
                testcontroller2.set_current_position(2, 0)
                time.sleep(3)
                steppermotor.send_reset()
                steppermotor.send_angel_control("r", 90)
                testcontroller1.open_claw()
                time.sleep(1)
                steppermotor.send_angel_control("l", 90)
    else:
        testcontroller0.setspeed([0xffff, 0xffff, 0x7f00 - 0x027f, 0x7f00 - 0x027f])
    # 可添加适当的延迟，避免过于频繁地运行
    time.sleep(1)