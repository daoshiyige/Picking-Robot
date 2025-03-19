import cv2
import numpy as np
from pyorbbecsdk import *
from utilscpt import frame_to_bgr_image
import torch

config = Config()
pipeline = Pipeline()
print("loading model")
model = torch.hub.load("/home/nx/agv-vis/capture/yolov5/", "custom", source="local", path="/home/nx/agv-vis/capture/best.pt",force_reload=True)
print("model loaded")
def __init__():
    
    try:
        profile_list = pipeline.get_stream_profile_list(OBSensorType.COLOR_SENSOR)
        color_profile = profile_list.get_default_video_stream_profile()
        config.enable_stream(color_profile)
        profile_list = pipeline.get_stream_profile_list(OBSensorType.DEPTH_SENSOR)
        assert profile_list is not None
        depth_profile = profile_list.get_default_video_stream_profile()
        assert depth_profile is not None
        config.enable_stream(depth_profile)
        print("color profile : {}x{}@{}_{}".format(color_profile.get_width(),
                                                   color_profile.get_height(),
                                                   color_profile.get_fps(),
                                                   color_profile.get_format()))
        print("depth profile : {}x{}@{}_{}".format(depth_profile.get_width(),
                                                   depth_profile.get_height(),
                                                   depth_profile.get_fps(),
                                                   depth_profile.get_format()))
    except Exception as e:
        print(e)
        return
    config.set_align_mode(OBAlignMode.HW_MODE)
    pipeline.enable_frame_sync()
    pipeline.start(config)


def captureFrame():
    while True:
        try:
            frames: FrameSet = pipeline.wait_for_frames(100)
            if frames is None:
                continue
            color_frame = frames.get_color_frame()
            if color_frame is None:
                continue
            # covert to RGB format
            color_image = frame_to_bgr_image(color_frame)
            if color_image is None:
                print("failed to convert frame to image")
                continue
            depth_frame = frames.get_depth_frame()
            if depth_frame is None:
                continue
            break
        except Exception as e:
            print(e)
            break
    depth_frame = frames.get_depth_frame()
    width = depth_frame.get_width()
    height = depth_frame.get_height()
    scale = depth_frame.get_depth_scale()
    depth_data = np.frombuffer(depth_frame.get_data(), dtype=np.uint16)
    depth_data = depth_data.reshape((height, width))
    depth_data = depth_data.astype(np.float32) * scale
    depth_image = cv2.normalize(depth_data, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
    depth_image = cv2.applyColorMap(depth_image, cv2.COLORMAP_JET)
    # return color_image, depth_image 
    return color_image, depth_image, depth_data, width, height
    
def TargetCapture():
    # 获取相机参数
    camera_param = pipeline.get_camera_param()
    # 提取 RGB 相机的内参
    rgb_intrinsics = camera_param.rgb_intrinsic
    # 提取深度相机的内参
    depth_intrinsics = camera_param.depth_intrinsic

    # step 1 : take image 1
    # 调用 captureFrame 函数捕获图像，返回彩色图像、深度图像、深度数据、图像宽度和高度
    color_img, depth_img, depth_dat, wid, heig = captureFrame()
    # 检查彩色图像是否捕获成功
    if color_img is None:
        print("Failed to capture image")
        return False, None, None, None

    # step 2: find the target
    # 使用目标检测模型对彩色图像进行检测
    results = model(color_img)
    # 清空 PyTorch 的 CUDA 缓存，释放内存
    torch.cuda.empty_cache()

    # select the largest target
    # 初始化最大目标面积为 0
    max_area = 0
    # 初始化最大目标为 None
    target = None

    # if no results, return
    # 检查检测结果中是否有目标
    if len(results.xyxy[0]) == 0:
        print("No target found")
        return False, None, None, None

    # 遍历所有检测到的目标
    for result in results.xyxy[0]:
        # 提取目标的边界框坐标
        x1, y1, x2, y2 = result[:4]
        # 计算目标的面积
        area = (x2 - x1) * (y2 - y1)
        # 如果当前目标面积大于最大面积
        if area > max_area:
            # 更新最大面积
            max_area = area
            # 更新最大目标
            target = result

    # 如果没有找到目标
    if target is None:
        print("No target found")
        return False, None, None, None

    # 如果 target 是一个 PyTorch 张量，先将其移动到 CPU
    if isinstance(target, torch.Tensor):
        target = target.cpu().numpy()  # 将张量转换为 NumPy 数组

    # step 3: move the camera to the target
    # calculate the center of the target
    # 计算目标的中心 x 坐标
    x_center = (target[0] + target[2]) / 2
    # 计算目标的中心 y 坐标
    y_center = (target[1] + target[3]) / 2
    # 获取 RGB 相机图像的中心 x 坐标
    ux = rgb_intrinsics.width / 2
    # 获取 RGB 相机图像的中心 y 坐标
    uy = rgb_intrinsics.height / 2

    # 确保目标中心坐标在深度数据的范围内
    if 0 <= x_center < wid and 0 <= y_center < heig:
        # 获取目标中心对应的深度值
        distance = depth_dat[int(y_center), int(x_center)]
    else:
        print("Target center is out of depth data range")
        return False, None, None, None

    # Calculate the 3D coordinates
    # Convert pixel coordinates to real-world coordinates
    # 计算目标在真实世界中的 x 坐标
    x_real = (x_center - ux) * distance / rgb_intrinsics.fx
    # 计算目标在真实世界中的 y 坐标
    y_real = (y_center - uy) * distance / rgb_intrinsics.fy
    z_real = distance

    return True, x_real, y_real, z_real
        
    #原先部分
    # # calculate the angle to move
    # x = (x_center - ux) / rgb_intrinsics.fx
    # theta_x = np.arctan(x)
    # print("theta_x:",theta_x)
    # # use camera intrinsic parameters to calculate the angle





__init__()
while True:
    TargetCapture()