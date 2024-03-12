outHR = []
outGSR = []

with open('arduinoReadings.csv','r') as f:
    for line in f:
        if 'Heart Rate' in line:
            outHR.append(line)
        else:
            outGSR.append(line)

with open('outHR.csv','a') as f:
    for line in outHR:
        f.write(line)

with open('outGSR.csv','a') as f:
    for line in outGSR:
        f.write(line)

