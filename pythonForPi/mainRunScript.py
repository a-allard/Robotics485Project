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
        if abs(x) < 10:
            x = 0
        if abs(y) < 10:
            y = 0
        if abs(phi) < 5:
            phi = 0
        phi *= 0.05
        motors.sendMoveCommand(x, y, phi)
        print((x, y, phi))
        time.sleep(0.1)
    motors.stop()

def findObsticles(imgForObsticles, imgToDrawOn=None):
    blurX = 35
    blurY = 35
    diffGain = 75
    threshold = 130
    ObsticleContourMinSize = 10
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

    maxs = np.array([c[:, 0, :].max(0) for c in obsticleContoursFilt]).max(0)
    mins = np.array([c[:, 0, :].min(0) for c in obsticleContoursFilt]).min(0)
    if not imgToDrawOn:
        imgToDrawOn = imgForObsticles.copy()
    imgContours = cv2.drawContours(imgToDrawOn, obsticleContoursFilt, -1, (0, 220, 250), 2)
    imgContours = cv2.rectangle(imgContours, (mins[0], mins[1]), (maxs[0], maxs[1]), (255, 0, 0), 1)

    return (maxs, mins, imgContours)


def lineFollow():
    xOffset = 0
    phiOffset = 0
    while remote.getControllerMode() == 1:
        img = cam.__captureImage__()
        img2 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        ObsticlesMax, ObsticlesMin, obsticlesMarked = findObsticles(img2, img)
        img3 = cv2.morphologyEx(img2, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))
        ret, img3 = cv2.threshold(img3, 80, 255, cv2.THRESH_BINARY_INV)
        im2, contours, hierarchy = cv2.findContours(img3, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
        contoursAtBottom = [c for c in contours if c[:, 0, 1].max() == (img.shape[0] - 1)]
        imgContours = cv2.drawContours(obsticlesMarked, contoursAtBottom, -1, (0, 255, 0), 3)
        if len(contoursAtBottom) > 0:
            # finding center of line in bottom of image:
            centerOfLine = contoursAtBottom[0][contoursAtBottom[0][:,0,1]==(img.shape[0] - 1), 0, :][:,0].mean()
            bottomCorners = np.where(contoursAtBottom[0][:,0,1]==(img.shape[0] - 1))[0].min(), np.where(contoursAtBottom[0][:,0,1]== (img.shape[0] - 1))[0].max()

            leftSide = np.diff(img2.shape - contoursAtBottom[0][bottomCorners[0] - 50: bottomCorners[0], 0, :], axis=0)
            rightSide = np.diff(img2.shape - contoursAtBottom[0][bottomCorners[1]: bottomCorners[1] + 50, 0, :], axis=0)
            try:
                angle = (np.arctan(leftSide[:, 0] / leftSide[:, 1]) + np.arctan(rightSide[:, 0] / rightSide[:, 1])) / 2
            except:
                angle = np.array([0, 0])

            imgFinal = cv2.arrowedLine(imgContours, (int(round(centerOfLine)), img.shape[0] - 10), (int(round(centerOfLine + 50 * np.sin(angle.mean() - np.pi))), int(round(img.shape[0] - 1 + 50 * np.cos(angle.mean() - np.pi)))), (0, 0, 255), 2)

            cv2.imshow('test', cv2.line(imgFinal, (int(round(centerOfLine-10)), (img.shape[0] - 1)), (int(round(centerOfLine-10)), 400), (255, 0, 0), 2))
            xOffset = (centerOfLine / img.shape[1] - 0.5) * 2 * 20
            phiOffset = np.interp(-angle.mean(), (-np.pi, np.pi), (-20, 20))
            if(abs(phiOffset) > 0)  and (abs(phiOffset) < 0.3):
                phiOffset = 0.3
            print('X: {0} Phi: {1}'.format(xOffset, phiOffset))
        else:
            xOffset = 0
            phiOffset = 0
            cv2.imshow('test', img)
        cv2.waitKey(1)
        x, y, phi = remote.getRemoteVectors()
        #x += xOffset
        if abs(x) < 10:
            x = 0
        if abs(y) < 10:
            y = 0
        y *= 0.6
        if abs(phi) < 5:
            phi = 0
        #phi += phiOffset
        phi *= 0.3
        motors.sendMoveCommand(x, y, phi)
        time.sleep(0.1)
        motors.sendMoveCommand(x / 2, y / 2, phi / 2)
        print('X: {0:.2f} Phi: {1:.2f}...Actual: X: {2:.2f}, Y: {3:.2f}, PHI: {4:.2f}'.format(xOffset, phiOffset, x, y, phi))

def main():
    # go REx Go!
    motors.stop()
    time.sleep(0.1)
    while 1:
        if remote.getControllerMode() == 2:
            fullRemoteDrive()
        if remote.getControllerMode() == 1:
            lineFollow()
        unFilteredPeople, filteredPeople = cam.findPeople()
        x,y = cam.findFavoritePersonLocation(filteredPeople)
        if not x == None:
            v,th,ph = remote.getRemoteVectors()
            motors.sendMoveCommand(v, th, x * np.pi / 3)






if __name__ == '__main__':
    cam = RExEye()
    motors = motorControl()
    remote = remoteControl()
    main()

