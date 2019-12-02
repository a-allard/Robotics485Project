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
                if contoursAtBottom[0][:,0,1].min() < (img.shape[0] - 40):
                    farEndTemp = np.where(contoursAtBottom[0][:,0,0]>=(img.shape[1] - 300))[0]
                    if farEndTemp.size == 0:
                        farEndTemp = np.where(contoursAtBottom[0][:,0,0] >= 300)[0]
                        if farEndTemp.size == 0:
                            print("far end not updated")
                            pass
                            #farEnd = np.where(contoursAtBottom[0][:,0,1]==(img.shape[0] - 1))[0].max() - img.shape[1] / 2

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
                cv2.imshow('test', imgFinal)
                xOffset = (centerOfLine / img.shape[1] - 0.5) * 14
                if xOffset < 0:
                    if xOffset < -4.5:
                        xOffset = -4.5
                else:
                    if xOffset > 4.5:
                        xOffset = 4.5
                phiOffset = np.interp(-angle.mean(), (-np.pi, np.pi), (-9, 9))
                yOffset = 0
                print('X: {0} Phi: {1}'.format(xOffset, phiOffset))
                #if :
                 #   avoidObstacle()
            else:
                xOffset = 0
                if farEnd > 0:
                    phiOffset = 2.0
                    xOffset = -1.4
                else:
                    phiOffset = -2.0
                    xOffset = 1.4
                print(farEnd)
        cv2.imshow('test', img)
            if phiOffset < 0:
                if phiOffset < -2.2:
                    phiOffset = -2.2
            else:
                if phiOffset > 2.2:
                    phiOffset = 2.2
        cv2.waitKey(1)
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
                y += yOffset + 0.5
        else:
            y += yOffset
        motors.sendMoveCommand(x, y, phi)
        print('X: {0:.2f} Phi: {1:.2f}...Actual: X: {2:.2f}, Y: {3:.2f}, PHI: {4:.2f}'.format(xOffset, phiOffset, x, y, phi))
    time.sleep(1)

def main():
    # go REx Go!
    motors.stop()
    time.sleep(0.1)
    while 1:
        if remote.getControllerMode() == 2:
            fullRemoteDrive()
        if remote.getControllerMode() == 1:
            lineFollow()
        else:
            img = cam.__captureImage__()
            unFilteredPeople, filteredPeople, peopleImg = cam.findPeople(img, True)
            print("found people")
            x, y = cam.findFavoritePersonLocation(filteredPeople)
            cv2.imshow('test', peopleImg)
            cv2.waitKey(1)
            if not x == None:
                v,th,ph = remote.getRemoteVectors()
                motors.sendMoveCommand(v, th, x * np.pi / 3)






if __name__ == '__main__':
    cam = RExEye()
    motors = motorControl()
    remote = remoteControl()
    main()

