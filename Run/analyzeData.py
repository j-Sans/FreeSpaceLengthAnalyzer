import numpy as np
import sys

# dataType:
#   lf: squareSize
#   xf: area fraction
#   v: volume fraction
#   d: depth
#   anything else: boxSize
def get(dataFile, dataType):
    dataType = dataType.lower()
    index = 1 if dataType == "lf" else 2 if dataType == "xf" else 3 if dataType == "v" else 4 if dataType == "d" else 0

    data = []
    for line in dataFile:
        lineData = line.split()
        data.append(float(lineData[index]))
    return data

filename = "output.txt"
xAxis = "xf"
yAxis = "Lf"

if len(sys.argv) > 3:
    filename = sys.argv[1]
    xAxis = sys.argv[2]
    yAxis = sys.argv[3]
elif len(sys.argv) > 2:
    xAxis = sys.argv[1]
    yAxis = sys.argv[2]
elif len(sys.argv) > 1:
    filename = sys.argv[1]

# output.txt line format:
# [boxSize] [squareSize] [area fraction] [volume fraction] [depth]
data = open(filename, "r");

x = np.log10(get(data, xAxis))
data.seek(0)
y = np.log10(get(data, yAxis))

func = np.polyfit(x, y, 1)
print(yAxis + " = (" + str(func[0]) + " * " + xAxis + ") + " + str(func[1]))
