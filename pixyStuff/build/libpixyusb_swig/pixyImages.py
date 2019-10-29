#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Oct 25 06:47:30 2019

@author: andrew
"""


import pixy
import numpy as np
import psutil
import cv2
import time
from imutils.object_detection import non_max_suppression
import imutils
import pandas as pd



class pixyCamera(object):
    def __init__(self):
        if not pixy.pixy_init() == 0:
            print("Error opening camera")
        pixy.pixy_cam_set_auto_white_balance(10)
        pixy.pixy_cam_set_auto_exposure_compensation(100)
        self._hog = cv2.HOGDescriptor()
        self._hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())



    def __interpColors__(self, x, y, dat, width, index):
        if y & 1:
            if x & 1:
                r = dat[index]
                g = dat[index - 1] + dat[index + 1] + (dat[index - width] + dat[index + width]) >> 2
                b = dat[index + width - 1] + dat[index - width + 1] + (dat[index - width -1] + dat[index + width + 1])>>2
            else:
                r = (dat[index - 1] + dat[index + 1]) >> 1
                g = dat[index]
                b = (dat[index - width] + dat[index + width]) >> 1
        else:
            if x & 1:
                r = (dat[index - width] + dat[index + width]) >> 1
                g = dat[index]
                b = (dat[index - 1] + dat[index + 1]) >> 1
            else:
                r = dat[index + width - 1] + dat[index - width + 1] + (dat[index - width -1] + dat[index + width + 1])>>2
                g = dat[index - 1] + dat[index + 1] + (dat[index - width] + dat[index + width]) >> 2
                b = dat[index]
        return (r, g, b)

    def __processImage__(self, b, c):
        index = b[0].rwidth
        data = np.ndarray((b[0].rheight, b[0].rwidth, 3), np.uint8)

        for j in range(b[0].rheight - 2):
            index += 1
            for k in range(b[0].rwidth - 2):
                data[j, k, 0], data[j, k, 1], data[j, k, 2] = self.__interpColors__(k, j, c, b[0].rwidth, index)
                index += 1
            index += 1
        return np.array(data, dtype=np.uint8)

    def captureImages(self, numImages=1):
        images = []
        b = pixy.responceBlock(1)
        for i in range(numImages):
            rawImagePointer = pixy.pixy_getImage(b, 0x21, 0, 0, 320, 200)
            pythonImageArray = np.array([pixy.getChar(rawImagePointer, i) for i in range(b[0].numPixels)])
            images.append(self.__processImage__(b, pythonImageArray))
        return np.array(images, np.uint8)

    def findPeople(self, imageArray, drawOnImage=False, useOverLap=True):
        imageArray = imutils.resize(imageArray, width=min(200, imageArray.shape[1]))
        (rects, widths) = self._hog.detectMultiScale(imageArray, winStride=(2, 2),
                                                     padding=(4, 4), scale=1.05)
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

for i in range(10000):
    #time.sleep(0.01)
    t = time.time()
    img = pixyCam.captureImages(1)[0, :, :, :]
    t3=time.time()
    preRecTemp = psutil.sensors_temperatures()['cpu-thermal'][0].current
    rects, picks, personPick = pixyCam.findPeople(img, True)
    t2=time.time()
    print('Image #{0} cpuTemp: {3}C\nTime: {2:.4f} People: {1}\n'.format(i, len(picks), t2-t, psutil.sensors_temperatures()['cpu-thermal'][0].current))
    dfRow = {'Image#':i,'cpuTemp':psutil.sensors_temperatures()['cpu-thermal'][0].current,'Time':t2-t,'PeopleDetected':len(picks), 'cpuFreq':psutil.cpu_freq().current, 'imageCapTime':t3-t,'imageParseTime':t3-t2, 'PreRecTemp':preRecTemp}
    totalDF = totalDF.append(dfRow, ignore_index=True)
    #cv2.imshow('Testing', personPick)
    #cv2.waitKey(10)
#cv2.waitKey(0)
