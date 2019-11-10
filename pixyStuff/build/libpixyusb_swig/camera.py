#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov  1 09:31:09 2019

@author: allard
"""


import io
import numpy as np
import psutil
import cv2
from PIL import Image
import time
from imutils.object_detection import non_max_suppression
import imutils
import pandas as pd
from picamera import PiCamera
from picamera.array import PiRGBArray
from random import randint



class RExEye(object):
    def __init__(self):
        self._defaultRes = (400, 640)
        self,_favoritePersonLocation = None
        self._eyeCam = PiCamera()
        self._eyeCam.resolution = self._defaultRes
        self._eyeCam.color_effects = (128, 128)
        self._camImage = PiRGBArray(self._eyeCam)
        self._hog = cv2.HOGDescriptor()
        self._hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
        time.sleep(2)


    def __captureImage__(self):
        self._eyeCam.capture(self._camImage, format='bgr', use_video_port=True)
        return self._camImage.array.copy()

    def findPeople(self, imageArray=None, drawOnImage=False, useOverLap=True, parseWidth=125):
        if not imageArray:
            imageArray = self.__captureImage__()
        imageArray = imutils.resize(imageArray, width=min(parseWidth, imageArray.shape[1]))
        (rects, widths) = self._hog.detectMultiScale(imageArray, winStride=(4, 4),
                                                     padding=(8, 8), scale=1.09)
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

    def findFavoritePerson(self, peopleArray):
        if not self._favoritePersonLocation:
            if peopleArray.size == 0:
                return -1
            self._favoritePersonLocation = randint(0, peopleArray.shape[0])
        diffs = np.abs(peopleArray - self._favoritePersonLocation)
        self._favoritePersonLocation = peopleArray[diffs.sum(1).argmin(), :]
        return True

    def angleChange(self):
        pass