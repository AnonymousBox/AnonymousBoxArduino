#!/usr/bin/env python  
import serial, pygame, pygame.camera, time, ast
import uploadToServer

PORT = '/dev/ttyUSB0'

class CameraPicture:
    """Handles taking and saving pictures"""
    def  __init__(self):
        pygame.camera.init()
        self.cam = pygame.camera.Camera(pygame.camera.list_cameras()[0])
    def take_picture(self):
        """captures the picture from the connected webcam"""
        self.cam.start()
        self.img = self.cam.get_image()
    def save_picture(self):
        """saves the taken picture on the fs"""
        ctime = time.localtime()
        name = "/tmp/"+str(ctime.tm_mon)+":"+str(ctime.tm_mday)+":"+str(ctime.tm_hour)+":"+ str(ctime.tm_min)+":"+str(ctime.tm_sec)+"picture.jpg"
        pygame.image.save(self.img, name)
        self.cam.stop()
        return name


if __name__ == '__main__':
    while True:
        #While loop searches for the arduino plugged into usb and
        #breaks out of while loop once found
        try:
            ser = serial.Serial(PORT, baudrate=9600, bytesize=8, parity='N',
            stopbits=1, timeout=1)
            break
        except serial.serialutil.SerialException as e:
            time.sleep(1)
            pass

    camera = CameraPicture()

    while(True):
        DATA = ser.readlines()
        if len(DATA) >= 1:
            DATA[0] = data[0].replace('\x02', '')
            print DATA
            PARAMS = ast.literal_eval(DATA[0])
            print PARAMS
            camera.take_picture()
            FILENAME = camera.save_picture()
            uploadToServer.uploadeverything(PARAMS, FILENAME)
