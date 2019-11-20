# -*- coding: utf-8 -*-
"""
Created on Mon Nov 18 08:07:55 2019

@author: aa9720
"""

from botPeripherals import remoteControl, motorControl
from camera import RExEye
import time


def fullRemoteDrive():
    while remoteControl.getControllerMode() == 2:
        motors.sendMoveCommand(*remoteControl.getRemoteVectors())
        time.sleep(0.1)

def main():
    # go REx Go!
    motorControl.stop()
    time.sleep(0.1)
    motorControl.enterLineFollow()
    while motorControl.queryLineStatus() == 1:
        time.sleep(1)
    if remoteControl.getControllerMode() == 2:
        fullRemoteDrive()
    unFilteredPeople, filteredPeople = cam.findPeople()
    cam.findFavoritePerson(filteredPeople)












if __name__ == '__main__':
    cam = RExEye()
    motors = motorControl()
    remote = remoteControl()
    main()

