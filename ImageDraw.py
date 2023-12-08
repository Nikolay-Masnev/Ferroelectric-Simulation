##import sys
##import numpy as np
##import matplotlib.pyplot as plt 
##
##fileName = str(sys.argv[1])
##
##count = 0
##
##data = []
##
##with open(fileName) as fp:
##    while True:
##        count += 1
##        line = fp.readline()
##        data.append(float(line))
##        
##        if not line or count == 1000000:
##            break
##npData = np.asarray(data, dtype=np.float32)

import sys
from PIL import Image
import numpy as np

fileName = str(sys.argv[1])
count = 0
data = []
with open(fileName) as fp:
    while True:
        count += 1
        line = fp.readline()
        data.append(float(line))
        if not line or count == 1000000:
            break
npData = np.asarray(data, dtype=np.float32)
drawData = np.zeros((1000, 1000, 3), dtype=np.uint8)

for i in range(npData.size):
    x = int(i%1000)
    y = int(i/1000)

    if (npData[i] > 0):
        drawData[y,x] = [0,0,255]
    else:
        drawData[y,x] = [255,255,0]
        
image = Image.fromarray(drawData)
image = image.resize((int(1000/np.sqrt(3)), 1000))
image.show()









