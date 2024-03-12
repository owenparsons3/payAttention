import serial
import time

ser = serial.Serial('/dev/cu.usbmodemF412FA9F91B02', 9600, timeout=0.1)

while True:
    serialPortReading = ser.readline().decode().strip()
    # spr = float(serialPortReading)
    # resistance = ((1024+2*spr)*10000)/(512-spr)
    # conductivity = str(1/resistance)
    timestamp = time.strftime('%H:%M:%S')
    with open('arduinoReadings.csv','a') as f:
        f.write(timestamp + ' ' + serialPortReading + '\n')
