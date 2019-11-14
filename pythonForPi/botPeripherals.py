#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 29 11:27:13 2019

@author: allard
"""

from serial import Serial
from serial.tools.list_ports import comports
import numpy as np


class teensy(Serial):
    _error = None
    _TEENSY_DESCRIPTION = ''
    def __init__(self):
        ports = comports()

        super(Serial, self).__init__(self.__findTeensy__(ports), 115200)
        self.timeout = 0.01
    def __findTeensy__(self, ports):
        port = [a.device for a in ports if self._TEENSY_DESCRIPTION in a.description]
        if len(port) == 1:
            return port
        else:
            return port[0]
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
        return np.vectorize(int)(returned.split(' ')[1].split(','))

    def enterLineFollow(self):
        self.write('lfm 1'.encode())
        returned = self.read_all().decode()
        if 'err' in returned:
            self._error = returned
            return int(returned[0])

    def exitLineFollow(self):
        self.write('lfm 0'.encode())
        returned = self.read_all().decode()
        if 'err' in returned:
            self._error = returned
            return int(returned[0])

    def stop(self):
        self.write('stp'.encode())
        returned = self.read_all().decode()
        if 'err' in returned:
            self._error = returned
            return int(returned[0])



class remoteControl(object):
    def __init__(self):
        self.thisIsBad=True