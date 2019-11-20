#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov  1 09:31:09 2019

@author: allard
"""


import numpy as np
import cv2
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
        self.favoritePersonLocation = None
        self._eyeCam = PiCamera()
        self._eyeCam.resolution = self._defaultRes
        self._eyeCam.color_effects = (128, 128)
        self._camImage = PiRGBArray(self._eyeCam)
        self._hog = cv2.HOGDescriptor()
        self._hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
        self._lastRects = None
        time.sleep(2)


    def __captureImage__(self):
        self._camImage.truncate(0)
        self._eyeCam.capture(self._camImage, format='bgr', use_video_port=True)
        self._lastRects = None
        return self._camImage.array.copy()

    def findPeople(self, imageArray=None, drawOnImage=False, useOverLap=True, parseWidth=125):
        if imageArray is None:
            imageArray = self.__captureImage__()
        if self._lastRects is None:
            return (self._lastRects.copy(), self._lastPicks.copy())
        imageArray = imutils.resize(imageArray, width=min(parseWidth, imageArray.shape[1]))
        (rects, widths) = self._hog.detectMultiScale(imageArray, winStride=(12, 12),
                                                     padding=(15, 15), scale=1.09)
        rects = np.array([[x, y, x + w, y + h] for (x, y, w, h) in rects])
        if useOverLap:
            pick = non_max_suppression(rects, probs=None, overlapThresh=0.5)
        else:
            pick = rects
        self._lastRects = rects.copy()
        self._lastPicks = pick.copy()
        if drawOnImage:
            for (xA, yA, xB, yB) in pick:
                cv2.rectangle(imageArray, (xA, yA), (xB, yB), (0, 255, 0), 2)
            return (rects, pick, imageArray)
        return (rects, pick)

    def findFavoritePerson(self, peopleArray):
        if not self._favoritePersonLocation:
            if peopleArray.size == 0:
                return -1
            self.favoritePersonLocation = randint(0, peopleArray.shape[0])
        diffs = np.abs(peopleArray - self._favoritePersonLocation)
        self.favoritePersonLocation = peopleArray[diffs.sum(1).argmin(), :]
        return True

    def findFavoritePersonLocation(self, peopleArray):
        if not self.findFavoritePerson(peopleArray):
            return None
        centerX = (self.favoritePersonLocation[2] - self.favoritePersonLocation[0]) / 2 + self.favoritePersonLocation[0]
        centerY = (self.favoritePersonLocation[3] - self.favoritePersonLocation[1]) / 2 + self.favoritePersonLocation[1]
        centerX = (centerX / self._camImage.shape[0] - 0.5) * 2
        centerY = (centerY / self._camImage.shape[1] - 0.5) * 2
        return(centerX, centerY)








if __name__ == '__main__':
    cam = RExEye()
    imgCounter = 0
    peopleDF = pd.DataFrame()
    while 1:
        img = cam.__captureImage__()
        newImg = cam.findPeople(img, True, False, 250)
        if newImg[0].size > 0:
            for row in newImg[0]:
                peopleDF = peopleDF.append(pd.DataFrame([[imgCounter, row[0], row[1], row[2], row[3]]]), ignore_index=True)
            imgCounter += 1
        cv2.imshow('testing', newImg[2])
        cv2.waitKey(10)
