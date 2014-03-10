#!/usr/bin/env python  
import serial, pygame, pygame.camera, time, ast
import uploadToServer

PORT = '/dev/ttyUSB0'

class CameraPicture:
    """Handles taking and saving pictures"""
    def  __init__(self):
        pygame.camera.init()
        self.cams = map(pygame.camera.Camera, pygame.camera.list_cameras())
        self.imgs = [None] * 3
    def take_pictures(self):
        """captures the picture from the connected webcam"""
        for i in range(len(self.cams)): 
            self.cams[i].start()
            self.imgs[i] = self.cams[i].get_image()
            self.cams[i].stop()
    def save_pictures(self):
        """saves the taken picture on the fs"""
        ctime = time.localtime()
        names = [None] * len(self.cams)
        for i in range(len(self.cams)):
            names[i] = "/tmp/"+str(ctime.tm_mon)+":"+str(ctime.tm_mday)+":"+str(ctime.tm_hour)+":"+ str(ctime.tm_min)+":"+str(ctime.tm_sec)+str(i)+"picture.jpg"
            pygame.image.save(self.imgs[i], names[i])
        return names


#   if __name__ == '__main__':
#       while True:
#           #While loop searches for the arduino plugged into usb and
#           #breaks out of while loop once found
#           try:
#               ser = serial.Serial(PORT, baudrate=9600, bytesize=8, parity='N',
#               stopbits=1, timeout=1)
#               break
#           except serial.serialutil.SerialException as e:
#               time.sleep(1)
#               pass
#       cameras = CameraPicture()

#       while(True):
#           data = ser.readlines()
#           if len(data) >= 1:
#               data[0] = data[0].replace('\x02', '')
#               print data
#               params = ast.literal_eval(data[0])
#               print params
#               cameras.take_pictures()
#               filenames = cameras.save_pictures()
#               uploadToServer.uploadeverything(params, filenames[0])
