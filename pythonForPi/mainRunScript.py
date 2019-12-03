# -*- coding: utf-8 -*-
"""
Created on Mon Nov 18 08:07:55 2019

@author: aa9720
"""

from botPeripherals import remoteControl, motorControl
from camera import RExEye
import time
import numpy as np
import cv2
import gc


def fullRemoteDrive():
    while remote.getControllerMode() == 2:
        x, y, phi = remote.getRemoteVectors()
        print((x, y, phi))
        if abs(x) < 1:
            x = 0
        if abs(y) < 1:
            y = 0
        if abs(phi) < 1:
            phi = 0
        motors.sendMoveCommand(x, y, phi)
        time.sleep(0.1)
        img = cam.__captureImage__()
        cv2.imshow('test', img)
        cv2.waitKey(1)
    motors.stop()

def findObsticles(imgForObsticles, imgToDrawOn=None):
    blurX = 35
    blurY = 35
    diffGain = 65
    threshold = 120
    ObsticleContourMinSize = 10
    if imgToDrawOn is None:
        imgToDrawOn = imgForObsticles.copy()
    diff = abs(np.diff(np.float64(cv2.blur(imgForObsticles,
                                           (blurX, blurY))), 1, 0))
    difference = cv2.threshold(np.uint8(diff*diffGain),
                               threshold,
                               255,
                               cv2.THRESH_BINARY)[1]
    im2, obsticleContours, hierachy = cv2.findContours(difference,
                                                       cv2.RETR_TREE,
                                                       cv2.CHAIN_APPROX_NONE)

    obsticleContoursFilt = [c for c in obsticleContours if (c[:, 0, :].max(0) - c[:, 0, :].min(0)).min() > ObsticleContourMinSize]
    if len(obsticleContoursFilt) > 0:
        maxs = np.array([c[:, 0, :].max(0) for c in obsticleContoursFilt]).max(0)
        mins = np.array([c[:, 0, :].min(0) for c in obsticleContoursFilt]).min(0)
    else:
        maxs = np.array([1, 1])
        mins = np.array([0, 0])
    imgContours = cv2.drawContours(imgToDrawOn, obsticleContoursFilt, -1, (0, 220, 250), 2)
    imgContours = cv2.rectangle(imgContours, (mins[0], mins[1]), (maxs[0], maxs[1]), (255, 0, 0), 1)
    return (maxs, mins, imgContours)


def avoidObstacle():
    print("Avoiding an obstable!!!")
    x = 5
    y = 5
    motors.sendMoveCommand(0, 0, 0)
    motors.sendMoveCommand(x, 0, 0)
    time.sleep(2)
    motors.sendMoveCommand(0, y, 0)
    time.sleep(6)
    motors.sendMoveCommand(-x, 0, 0)
    time.sleep(2)
    motors.sendMoveCommand(0, 0, 0)
    time.sleep(1)


def lineFollow():
    xOffset = 0
    phiOffset = 0
    yOffset = 0
    angle = np.array([2,2])
    lastGoodAngle = angle.copy()
    farEnd = 0
    while remote.getControllerMode() == 1:
        img = cam.__captureImage__()
        imgFinal = img.copy()
        img2 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        #ObsticlesMax, ObsticlesMin, obsticlesMarked = findObsticles(img2, img)
        img3 = cv2.morphologyEx(img2, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))
        ret, img3 = cv2.threshold(img3, 80, 255, cv2.THRESH_BINARY_INV)
        im2, contours, hierarchy = cv2.findContours(img3, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
        contoursAtBottom = [c for c in contours if c[:, 0, 1].max() == (img.shape[0] - 1)]
        #imgContours = cv2.drawContours(obsticlesMarked, contoursAtBottom, -1, (0, 255, 0), 3)
        imgContours = cv2.drawContours(img, contoursAtBottom, -1, (0, 255, 0), 3)
        if len(contoursAtBottom) > 0:
            if contoursAtBottom[0].min() < (img.shape[0] - 80):
                # finding center of line in bottom of image:
                centerOfLine = contoursAtBottom[0][contoursAtBottom[0][:,0,1]==(img.shape[0] - 1), 0, :][:,0].mean()
                bottomCorners = np.where(contoursAtBottom[0][:,0,1]==(img.shape[0] - 1))[0].min(), np.where(contoursAtBottom[0][:,0,1]== (img.shape[0] - 1))[0].max()
                if contoursAtBottom[0][:,0,1].min() < (img.shape[0] - 250):
                    farEndTemp = np.where(contoursAtBottom[0][:,0,0]>=(img.shape[1] - 50))[0]
                    if farEndTemp.size == 0:
                        farEndTemp = np.where(contoursAtBottom[0][:,0,0] >= 50)[0]
                        if farEndTemp.size == 0:
                            print("far end not updated")
                        else:
                            farEnd = -1
                    else:
                        farEnd = 1

                leftSide = np.diff(img2.shape - contoursAtBottom[0][bottomCorners[0] - 75: bottomCorners[0], 0, :], axis=0)
                rightSide = np.diff(img2.shape - contoursAtBottom[0][bottomCorners[1]: bottomCorners[1] + 75, 0, :], axis=0)
                try:
                    newAngle = (np.arctan(leftSide[:, 0] / leftSide[:, 1]) + np.arctan(rightSide[:, 0] / rightSide[:, 1])) / 2
                    lastGoodAngle = angle.copy()
                    angle = newAngle.copy()
                    imgFinal = cv2.arrowedLine(imgContours, (int(round(centerOfLine)), img.shape[0] - 10), (int(round(centerOfLine + 50 * np.sin(angle.mean() - np.pi))), int(round(img.shape[0] - 1 + 50 * np.cos(angle.mean() - np.pi)))), (0, 0, 255), 2)
                except:
                    angle = np.array([0, 0])
                    imgFinal = imgContours
                #cv2.imshow('test', imgFinal)
                xOffset = (centerOfLine / img.shape[1] - 0.5) * 15
                if xOffset < 0:
                    if xOffset < -5.5:
                        xOffset = -5.5
                else:
                    if xOffset > 5.5:
                        xOffset = 5.5
                phiOffset = np.interp(-angle.mean(), (-np.pi, np.pi), (-7, 7))
                if phiOffset < 0:
                    if phiOffset < -1.9:
                        phiOffset = -1.9
                else:
                    if phiOffset > 1.9:
                        phiOffset = 1.9
                yOffset = 0
                print('X: {0} Phi: {1}'.format(xOffset, phiOffset))
                #if :
                 #   avoidObstacle()
            else:
                if farEnd > 0:
                    phiOffset = 1.9
                    xOffset = -1.8
                else:
                    phiOffset = -1.9
                    xOffset = 1.8
                print(farEnd)
        else:
            if farEnd > 0:
                phiOffset = 1.9
                xOffset = -1.5
            else:
                phiOffset = -1.9
                xOffset = 1.5
            print(farEnd)


        cv2.imshow('test', imgFinal)
        cv2.waitKey(2)
        x, y, phi = remote.getRemoteVectors()
        if abs(x) < 1:
            x = 0
        if abs(y) < 1:
            y = 0
        if abs(phi) < 1:
            phi = 0
        x += xOffset
        phi -= phiOffset
        if len(contoursAtBottom) > 0:
            if contoursAtBottom[0][:,0,1].min() < (img.shape[0] - 40):
                y += 4 + yOffset
            else:
                y += yOffset + 0.75
        else:
            y += yOffset
        motors.sendMoveCommand(x, y, phi)
        print('X: {0:.2f} Phi: {1:.2f}...Actual: X: {2:.2f}, Y: {3:.2f}, PHI: {4:.2f}'.format(xOffset, phiOffset, x, y, phi))

def main():
    # go REx Go!
    motors.stop()
    time.sleep(0.1)
    while 1:
        if remote.getControllerMode() == 2:
            fullRemoteDrive()
            time.sleep(1)
            gc.collect()
        if remote.getControllerMode() == 1:
            lineFollow()
            time.sleep(1)
            gc.collect()
        else:
            img = cam.__captureImage__()
            #unFilteredPeople, filteredPeople, peopleImg = cam.findPeople(img, True)
            #unFilteredPeople, filteredPeople, peopleImg = cam.findPeople(img, True, False, 250)
            #x, y = cam.findFavoritePersonLocation(filteredPeople)
            img2 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            img3 = cv2.morphologyEx(img2, cv2.MORPH_CLOSE, np.ones((1, 1), np.uint8))
            # ret, img3 = cv2.threshold(img3, 10, 40, cv2.THRESH_BINARY_INV)
            # im2, contours, hierarchy = cv2.findContours(img3, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
            # contours = [c for c in contours if abs(c[:, 0, :].max(0) - c[:, 0, :].min(0)).min() > 10]
            # contours = [c for c in contours if abs(c[:, 0, :].max(0) - c[:, 0, :].min(0)).max() < 250]
            z, contours = cam.findObject(img3, True, 1280)
            imgDrawn = cv2.drawContours(img, contours, -1, (0, 255, 0), 3)
            cv2.imshow('test', imgDrawn)
            cv2.waitKey(1)
            if len(contours) > 0:
                m = cv2.moments(contours[0])
                x = (m['m10'] / m['m00'] - (img.shape[1] - 1) / 2) / (img.shape[1] - 1)
                v,th,ph = remote.getRemoteVectors()
                motors.sendMoveCommand(v, th, (x * np.pi / 2) + ph)
                print(x)
            else:
                v,th,ph = remote.getRemoteVectors()
                motors.sendMoveCommand(v, th, ph)






if __name__ == '__main__':
    cam = RExEye()
    motors = motorControl()
    remote = remoteControl()
    main()

