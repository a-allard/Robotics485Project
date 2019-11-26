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
cam.rotation=180
time.sleep(2)

array = PiRGBArray(cam)

imageIndex = 0
numToAverage = 5
thetasToAverage = np.zeros(numToAverage)
theta = 0

for frame in cam.capture_continuous(array, format='bgr', use_video_port=True):
    img = frame.array
    img = imutils.resize(img, 400)
    img2 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img2 = cv2.dilate(img2, np.ones((3, 3)))
    blurred = cv2.GaussianBlur(img2, (3, 3), 2)
    edged = cv2.Canny(blurred, 80, 150)
    lines = cv2.HoughLinesP(edged, 1, np.pi/180, 30, 90, 1)
    if not lines is None:
        lines = lines.squeeze()
        thetas = np.zeros((lines.shape[0], 2))
        index = 0
        for x1, y1, x2, y2 in lines:
            cv2.line(img, (x1,y1),(x2,y2),(0,255,0),2)
            delx = x2 - x1
            dely = y1 - y2
            if delx == 0:
                delx = 1
            if dely == 0:
                dely = 1
            thetas[index, 0] = np.arctan(delx/dely)
            index += 1
        numParLines = np.array([len(thetas[0.03 > abs(thetas[:, 0] - t), 0]) / lines.shape[0] / (np.sqrt((y2-y1)**2 + (x2-x1)**2) / 50) * y2 for t in thetas[:, 0]])
        thetasToAverage[imageIndex] = thetas[numParLines.argmax(), 0]
        imageIndex += 1
        if imageIndex == numToAverage:
            imageIndex = 0
            theta = thetasToAverage.mean()
        cv2.arrowedLine(img, (250, 200), (int(round(250 + 30*np.sin(np.pi - theta))), int(round(200 + 30*np.cos(np.pi - theta)))), (100, 0, 255), 2)
    frame.truncate(0)
    cv2.imshow('testing', img)
    cv2.waitKey(40)

