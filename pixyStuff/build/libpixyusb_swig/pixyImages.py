#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Oct 25 06:47:30 2019

@author: andrew
"""


import pixy
import numpy as np
import cv2
import time
from imutils.object_detection import non_max_suppression



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
            images.append(self.processImage(b, pythonImageArray))
        return np.array(images, np.uint8)

    def findPeople(self, imageArray, drawOnImage=False):
        (rects, widths) = self._hog.detectMultiScale(imageArray, winStride=(4,4),
                                                     padding=(8,8), scale=1)
        rects = np.array([[x, y, x + w, y + h] for (x, y, w, h) in rects])
        pick = non_max_suppression(rects, probs=None, overlapThresh=0.65)
        if drawOnImage:
            for (xA, yA, xB, yB) in pick:
                cv2.rectangle(imageArray, (xA, yA), (xB, yB), (0, 255, 0), 2)
        return pick



pixyCam = pixyCamera()

img = pixyCam.captureImages(1)[0, :, :, :]
rects = pixyCam.findPeople(img, True)
cv2.imshow('Testing', img)
cv2.waitKey(0)
