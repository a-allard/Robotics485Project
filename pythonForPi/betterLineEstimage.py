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


img = cv2.imread(os.path.expanduser('~/Downloads/etape.jpg'))
img = imutils.resize(img, 300)

img2 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
# img2 = cv2.GaussianBlur(img2, (3, 3), 2)
img3 = cv2.morphologyEx(img2, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))
# img3 = img2
cv2.imshow('moph', img3)
ret, img3 = cv2.threshold(img3, 80, 255, cv2.THRESH_BINARY_INV)
im2, contours, hierarchy = cv2.findContours(img3, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
contoursAtBottom = [c for c in contours if c[:, 0, 1].max() == (img.shape[0] - 1)]
imgContours = cv2.drawContours(img, contoursAtBottom, -1, (0, 255, 0), 3)
cv2.imshow('binary', img3)
# finding center of line in bottom of image:
centerOfLine = contoursAtBottom[0][contoursAtBottom[0][:,0,1]==(img.shape[0] - 1), 0, :][:,0].mean()
bottomCorners = np.where(contoursAtBottom[0][:,0,1]==(img.shape[0] - 1))[0].min(), np.where(contoursAtBottom[0][:,0,1]== (img.shape[0] - 1))[0].max()

leftSide = np.diff(img2.shape - contoursAtBottom[0][bottomCorners[0] - 50: bottomCorners[0], 0, :], axis=0)
rightSide = np.diff(img2.shape - contoursAtBottom[0][bottomCorners[1]: bottomCorners[1] + 50, 0, :], axis=0)
angle = (np.arctan(leftSide[:, 0] / leftSide[:, 1]) + np.arctan(rightSide[:, 0] / rightSide[:, 1])) / 2

imgFinal = cv2.arrowedLine(imgContours, (int(round(centerOfLine)), img.shape[0] - 10), (int(round(centerOfLine + 50 * np.sin(angle.mean() - np.pi))), int(round(img.shape[0] - 1 + 50 * np.cos(angle.mean() - np.pi)))), (0, 0, 255), 2)

cv2.imshow('test', cv2.line(imgFinal, (int(round(centerOfLine-10)), (img.shape[0] - 1)), (int(round(centerOfLine-10)), 400), (255, 0, 0), 2))