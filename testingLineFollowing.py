import math
import cv2
import numpy as np
import os
import imutils
import time

from picamera import PiCamera
from picamera.array import PiRGBArray

cam = PiCamera()
cam.resolution = (640, 400)
cam.color_effects = (128, 128)
time.sleep(2)

array = PiRGBArray(cam)

for frame in cam.capture_continuous(array, format='bgr', use_video_port=True):
    img = frame.array
    img = imutils.resize(img, 400)
    blurred = cv2.GaussianBlur(img, (9, 9), 0)
    edged = cv2.Canny(blurred, 140, 150)
    lines = cv2.HoughLinesP(edged, 1, np.pi/180, 10, 80, 1)
    if not lines is None:
        for x1, y1, x2, y2 in lines.squeeze():
            cv2.line(img, (x1,y1),(x2,y2),(0,255,0),2)
    frame.truncate(0)
    cv2.imshow('testing', img)
    cv2.waitKey(100)

