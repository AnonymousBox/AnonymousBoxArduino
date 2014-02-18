import serial, pygame, pygame.camera, time, json, ast
import uploadToServer
PORT = '/dev/ttyUSB0'
ser = serial.Serial(PORT, baudrate=9600, bytesize=8, parity='N', stopbits=1, timeout=1)
class CameraPicture:
    def  __init__(self):
        pygame.camera.init()
        self.cam = pygame.camera.Camera(pygame.camera.list_cameras()[0])
    def takePicture(self):
        self.cam.start()
        self.img = self.cam.get_image()
    def savePicture(self):
        ctime = time.localtime()
        name = "/tmp/"+str(ctime.tm_mon)+":"+str(ctime.tm_mday)+":"+str(ctime.tm_hour)+":"+str(ctime.tm_min)+":"+str(ctime.tm_sec)+"picture.jpg"
        pygame.image.save(self.img, name)
        self.cam.stop()
        return name


if __name__ == '__main__':
    camera = CameraPicture()
    while(True):
        data = ser.readlines()
        if(len(data) >= 1):
            data[0] = data[0].replace('\x02','')
            print data
            params = ast.literal_eval(data[0]);
            print params
            camera.takePicture()
            filename = camera.savePicture()
            uploadToServer.uploadeverything(params, filename)




 
