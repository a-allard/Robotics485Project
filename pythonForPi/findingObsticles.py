#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Nov 26 13:58:21 2019

@author: allard
"""


import cv2
import numpy as np
import os
import imutils
from numpy.fft import fft, ifft


img = cv2.imread(os.path.expanduser('~/Downloads/blocked2.jpg'))
img = imutils.resize(img, 600)

img2 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

morphed = cv2.morphologyEx(img2, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))

cv2.imshow('moph', morphed)


ret, binary = cv2.threshold(morphed, 80, 255, cv2.THRESH_BINARY_INV)

im2, contours, hierarchy = cv2.findContours(binary, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)


contoursAtBottom = [c for c in contours if c[:, 0, 1].max() == (img.shape[0] - 1)]

imgContours = cv2.drawContours(img.copy(), contours, -1, (0, 255, 0), 3)
cv2.imshow('binary', binary)


# finding center of line in bottom of image:

imgFinal = imgContours

cv2.imshow('test', imgFinal)



# %%

myFt = fft(cv2.blur(img2, (3, 3)), axis=1)
myFt[:, [1, 2, -1, -2]] = 0

myFtV = fft(img2, axis=0)
myFtV[[0, 1, 2, -1, -2], :] = 0

data = np.abs(ifft(myFtV, axis=0) - ifft(myFt, axis=-1))

unFFtedImage = np.uint8(data)
edges = cv2.Canny(unFFtedImage, 20, 40)
lines = cv2.HoughLinesP(edges, 1, np.pi/180, 60, minLineLength=1).squeeze()

newImg = img.copy()
for x1, y1, x2, y2 in lines:
    cv2.line(newImg, (x1, y1), (x2, y2), (255, 0, 0), 2)

cv2.imshow('fftLines', newImg)

cv2.imshow('fftedImage', np.uint8(data))
fftThres = cv2.threshold(np.uint8(data*4), 180, 255, cv2.THRESH_BINARY)[1]
cv2.imshow('binaryFFT', fftThres)

# %%
cv2.imshow('veryBlurred', cv2.blur(img2, (35, 35)))
diff = abs(np.diff(np.float64(cv2.blur(img2, (35, 35))), 1, 0))
cv2.imshow('derivative', np.uint8(diff*75))
difference = cv2.threshold(np.uint8(diff*75), 130, 255, cv2.THRESH_BINARY)[1]
cv2.imshow('diffThresh', difference)

im2, obsticleContours, hierachy = cv2.findContours(difference, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)

#obsticleContours = [c for c in obsticleContours if c.shape[0] > 80]

obsticleContoursRefined = [c for c in obsticleContours if
                           (c[:, 0, :].max(0) - c[:, 0, :].min(0)).min() >
                           10]

maxs = np.array([c[:, 0, :].max(0) for c in obsticleContoursRefined]).max(0)
mins = np.array([c[:, 0, :].min(0) for c in obsticleContoursRefined]).min(0)

imgContours = cv2.drawContours(img.copy(), obsticleContoursRefined, -1, (0, 220, 250), 2)
imgContours = cv2.rectangle(imgContours, (mins[0], mins[1]), (maxs[0], maxs[1]), (255, 0, 0), 1)

cv2.imshow('obsicles', imgContours)
