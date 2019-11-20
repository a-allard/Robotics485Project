#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 29 11:27:13 2019

@author: allard
"""

from serial import Serial
from serial.tools.list_ports import comports
import numpy as np
import time


class teensy(Serial):
    _error = None
    _TEENSY_DESCRIPTION = None
    def __init__(self, teensyDes='', openPort=True):
        self._TEENSY_DESCRIPTION = teensyDes
        if openPort:
            self.openPort()
    def openPort(self):
        ports = comports()
        super(Serial, self).__init__(self.__findTeensy__(ports), 115200)
        self.timeout = 0.01
    def __findTeensy__(self, ports):
        if not self._TEENSY_DESCRIPTION:
            port = ports[0].device
        port = [a.device for a in ports if self._TEENSY_DESCRIPTION in a.description]
        if len(port) == 1:
            return port
        else:
            return port[0]
    def query(self, strout):
        self.write(strout.encode())
        time.sleep(0.01)
        return self.read_all().decode()
    def __del__(self):
        self.close()
        super(Serial, self).__del__()

class motorControl(teensy):
    def __init__(self):
        super(teensy, self).__init__('TEENSY40', True)
        self.write('fco 0'.encode())
    def sendMoveCommand(self, velMag, velDirection, rotation):
        """
        Sends a command to the Teensy to change the robots position.
        This will change both velocity and angular position.
        To only rotate write velMag and velDirection 0.

        All angles are relative to the camera direction CCW+ CW-.

        Parameters
        ----------
        velMag : int
            The magnitude of the velocity to send to the Teensy in %
            of motor current.
        velDirection : float
            Direction of the velMag in radians.
        rotation : flaot
            How far to rotate the robot.

        Returns
        -------
        Sucess : bool
            If the Teensy returned no error

        """
        self.write('vel {0},{1.4f},{2:.4f}'.format(velMag, velDirection, rotation).encode())

        returnedStr = self.read_all().decode()
        if 'err' not in returnedStr.lower():
            return True
        else:
            self._error = returnedStr
            return int(returnedStr[0])


    def queryMoveCommand(self):
        self.write('vel?'.encode())
        returned = self.read_all().decode()
        if 'err' in returned:
            self._error = returned
            return int(returned[0])
        return np.vectorize(float)(returned.split(' ')[1].split(','))

    def enterLineFollow(self):
        returned = self.query('lfm 1')
        if 'err' in returned:
            self._error = returned
            return int(returned[0])

    def exitLineFollow(self):
        returned = self.query('lfm 0')
        if 'err' in returned:
            self._error = returned
            return int(returned[0])

    def queryLineStatus(self):
        returned = self.query('lfs')
        if 'err' in returned.lower():
            self._error = returned
            return int(returned[0])
        return int(returned)


    def stop(self):
        self.write('stp'.encode())
        returned = self.read_all().decode()
        if 'err' in returned:
            self._error = returned
            return int(returned[0])

    def __del__(self):
        super(teensy, self).__del__()



class remoteControl(teensy):
    def __init__(self):
        super(teensy, self).__init__('TEENSY32', True)
        self.write('fco 0'.encode())

    def getRemoteCommand(self):
        # CONTROLMODE,VEL,THETA,PHI
        # all values are relative to the forward direction of REx
        motionControllerMode = self.query('fcs').split(',')
        return (int(motionControllerMode[0]),
                float(motionControllerMode[1]),
                float(motionControllerMode[2]),
                float(motionControllerMode[4]))

    def getControllerMode(self):
        return int(self.query('bcm'))
    def getRemoteVectors(self):
        controlVector = self.query('vpt').split(',')
        return (float(controlVector[0]),
                float(controlVector[1]),
                float(controlVector[2]))

    def __del__(self):
        self.write('fco 1'.encode())
        super(teensy, self).__del__()
