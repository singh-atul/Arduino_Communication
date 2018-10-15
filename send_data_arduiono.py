import serial # you need to install the pySerial :pyserial.sourceforge.net
import time
# your Serial port should be different!
arduino = serial.Serial('/dev/ttyACM1', 9600)
time.sleep(2)
arduino.flushInput()


points = [b'30,F' , b'45,S' , b'22,L' , b'12,R']
# for i in points:
#     print(i)

def sendData(data):
    arduino.write(data)

for i in points:
    while True:
        arduino.flushInput()
        sendData(i)
        time.sleep(.5)
        d = arduino.readline()
        if len(d)> 0 :
            print(d)
            break
arduino.close()

#   command = raw_input("Type something..: (on/ off / bye )");
# 	if command =="on":
# 		print "The LED is on..."
# 		time.sleep(1)
# 		arduino.write('H')
# 		onOffFunction()
# 	elif command =="off":
# 		print "The LED is off..."
# 		time.sleep(1)
# 		arduino.write('L')
# 		onOffFunction()
# 	elif command =="bye":
# 		print "See You!..."
# 		time.sleep(1)
# 		arduino.close()
# 	else:
# 		print "Sorry..type another thing..!"
# 		onOffFunction()
#
# time.sleep(2) #waiting the initialization...
#
# onOffFunction()
