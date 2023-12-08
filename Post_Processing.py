import matplotlib.pyplot as plt
import numpy as np

nuD = 100

x_tr = []
y_tr = []
x_trNB = []
y_trNB = []

counter = 0
with open('data/time_reversed3.00kV_100000000.txt') as f:
    lines = f.readlines()
    for line in lines:
        if counter == 0:
            counter +=1
            continue
        if counter % 5000 != 0 and counter > 50:
            counter +=1
            continue
        counter +=1
        x_tr.append(line.split()[0])
        y_tr.append(line.split()[1])

counter = 0
with open('data/time_reversed_NB3.00kV_100000000.txt') as f:
    lines = f.readlines()
    for line in lines:
        if counter == 0:
            counter +=1
            continue
        if counter % 50 != 0 and counter > 50:
            counter +=1
            continue
        counter +=1
        x_trNB.append(line.split()[0])
        y_trNB.append(line.split()[1])

x_tr = np.log(np.asarray(x_tr, float) * nuD)
y_tr = np.log(np.asarray(y_tr, float)+1)
x_trNB = np.log(np.asarray(x_trNB, float) * nuD)
y_trNB = np.log(np.asarray(y_trNB, float))

lenAny = len(x_tr)
lenB = len(x_trNB)
t_rightAny = float(x_tr[lenAny-1])
t_rightB = float(x_trNB[lenB-1])

timestep = (t_rightAny - t_rightB)/(lenAny - lenB)

print(lenAny)
print(lenB)
print(timestep)

for i in range(lenAny-lenB):
    x_trNB = np.append(x_trNB, x_trNB[lenB-1] + i * timestep)
    y_trNB = np.append(y_trNB, y_trNB[lenB-1] + i * timestep)
    
print(len(x_trNB))

plt.xlabel(r'$ln{(t \cdot \nu_D)}$')
plt.ylabel("$\ln{(N)}$")

plt.plot(np.asarray(x_tr, float), np.asarray(y_tr, float))

if lenB > 1:
    plt.plot(np.asarray(x_trNB, float), np.asarray(y_trNB, float))
    plt.legend(["Any category", "category B"], loc ="upper left")
else:
    plt.legend(["Any category"], loc ="upper left")
plt.show()
