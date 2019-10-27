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


pixy.pixy_init()
pixy.pixy_cam_set_auto_white_balance(10)
pixy.pixy_cam_set_auto_exposure_compensation(100)



def interpColors(x, y, dat, width, index):
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


def processImage(b, c):
    index = b[0].rwidth
    data = np.ndarray((b[0].rheight, b[0].rwidth, 3), np.uint8)

    for j in range(b[0].rheight - 2):
        index += 1
        for k in range(b[0].rwidth - 2):
            data[j, k, 0], data[j, k, 1], data[j, k, 2] = interpColors(k, j, c, b[0].rwidth, index)
            index += 1
        index += 1
    return np.array(data, dtype=np.uint8)

pixy.pixy_run()
time.sleep(2)
def captureImages(numImages):
    images = []
    b = pixy.responceBlock(1)
    for i in range(numImages):
        time.sleep(0.01)
        # pixy.pixy_stop()
        a = pixy.pixy_getImage(b, 0x21, 0, 0, 320, 200)
        c = [pixy.getChar(a, i) for i in range(b[0].numPixels)]
        c = np.array(c)
        images.append(processImage(b, c))
    return np.array(images, np.uint8)


myImage = captureImages(25)

index = 0
for im in myImage:
    cv2.imshow('pixyImage_{0}'.format(index), im)
    index += 1
