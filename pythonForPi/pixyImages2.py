#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Oct 25 06:47:30 2019

@author: andrew
"""


import io
import numpy as np
import psutil
import cv2
from PIL import Image
from picamera.array import PiRGBArray
import time
from imutils.object_detection import non_max_suppression
import imutils
import pandas as pd
from picamera import PiCamera



class pixyCamera(object):
    def __init__(self):
        self._hog = cv2.HOGDescriptor()
        self._hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

    def findPeople(self, imageArray, drawOnImage=False, useOverLap=True, maxScale=500):
        imageArray = imutils.resize(imageArray, width=min(maxScale, imageArray.shape[1]))
        (rects, widths) = self._hog.detectMultiScale(imageArray, winStride=(4, 4),padding=(8, 8), scale=1.09)
        rects = np.array([[x, y, x + w, y + h] for (x, y, w, h) in rects])
        if useOverLap:
            pick = non_max_suppression(rects, probs=None, overlapThresh=0.5)
        else:
            pick = rects
        if drawOnImage:
            for (xA, yA, xB, yB) in pick:
                cv2.rectangle(imageArray, (xA, yA), (xB, yB), (0, 255, 0), 2)
            return (rects, pick, imageArray)
        return (rects, pick)



pixyCam = pixyCamera()
totalDF = pd.DataFrame()
#img = io.BytesIO()

cam = PiCamera()
cam.resolution=(400, 640)
#cam.start_preview()
cam.color_effects=(128,128)
time.sleep(3)
img = PiRGBArray(cam)
time.sleep(1)
totalDF = pd.DataFrame()
peopleDF = pd.DataFrame()
i=-1
for frame in cam.capture_continuous(img, format='bgr', use_video_port=True):
    i+=1
    t = time.time()
    t3=time.time()
    preRecTemp = psutil.sensors_temperatures()['cpu-thermal'][0].current
    rects, picks, personPick = pixyCam.findPeople(frame.array, True, maxScale=250)
    img.truncate(0)
    t2=time.time()
    #peopleDF = peopleDF.append(*rects)
    print('Image #{0} cpuTemp: {3}C\nTime: {2:.4f} People: {1}\n'.format(i, len(picks), t2-t, psutil.sensors_temperatures()['cpu-thermal'][0].current))
    dfRow = {'Image#':i,'cpuTemp':psutil.sensors_temperatures()['cpu-thermal'][0].current,'Time':t2-t,'PeopleDetected':len(picks), 'cpuFreq':psutil.cpu_freq().current, 'imageCapTime':t3-t,'imageParseTime':t3-t2, 'PreRecTemp':preRecTemp}
    totalDF = totalDF.append(dfRow, ignore_index=True)
    cv2.imshow('Testing', personPick)
    cv2.waitKey(10)
    if(i>10000):
        break
#cv2.waitKey(0)
