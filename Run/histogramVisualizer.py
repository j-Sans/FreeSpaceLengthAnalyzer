import sys
import numpy as np
import matplotlib.pyplot as plot

x = []
y = []

def getFilename(fileNum):
    return "distribution_" + str(fileNum) + ".txt"

fileNum = sys.argv[1]

with open(getFilename(fileNum), 'r') as f:
    f.readline()
    for line in f:
        y.append(int(line.split(' ')[1]))
        x = np.arange(len(y))

plot.xlabel("Particles per box")
plot.ylabel("Histogram")
plot.bar(x, y)
plot.show()
