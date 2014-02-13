import serial, pygame, pygame.camera
import uploadToServer
PORT = '/dev/ttyUSB0'
ser = serial.Serial(PORT, baudrate=9600, bytesize=8, parity='N', stopbits=1, timeout=1)
camera = CameraPicture()
class CameraPicture:
    def  __init__(self):
        pygame.camera.init()
        self.cam = pygame.camera.Camera(pygame.camera.list_cameras()[0])
    def takePicture(self):
        self.cam.start()
        self.img = self.cam.get_image()
    def savePicture(self):
        ctime = time.localtime()
        name = "./images/"+str(ctime.tm_mon)+":"+str(ctime.tm_mday)+":"+str(ctime.tm_hour)+":"+str(ctime.tm_min)+":"+str(ctime.tm_sec)+"picture.jpg"
        pygame.image.save(self.img, str(ctime.tm_mon)+":"+str(ctime.tm_mday)+":"+str(ctime.tm_hour)+":"+str(ctime.tm_min)+":"+str(ctime.tm_sec)+"picture.jpg")
        self.cam.stop()
        return name


if __name__ == '__main__':

    while(true):
        data = ser.readlines()
        if(len(data) >= 1):
            params = data
            camera.takePicture()
            filename = camera.savePicture()
            uploadToServer.uploadeverything(params, filename)




 
