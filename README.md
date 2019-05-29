# Free-Space Length Analyzer
Analyzes nanoparticles in polymer matrices to quantify the dispersion and the spacing of the particles.

This program calculates the free space length of a 2D image of particle locations. The program can analyze files of particle locations (-f), or it can simulate and analyze slices of a crystalline lattice of particles (--SC, --BCC, --FCC), or it can parse and analyze a simulation of nanoparticles in 3D (-s).

## Free-Space Length
* TO DO
* Number of squares tested with per iteration
* Explain the "wrapping" functionality of boxes on one side wrapping to the other side.

## How To Use It
First, to compile it, the SFML library is required. It can be downloaded from [this website](https://www.sfml-dev.org/index.php).

To compile, from within the main directory:
```
$ cd Source
$ make
```
Then, move the resulting executible (`SquareCalculator`) to a folder with the files to analyze. To run the program, do: 
```
$ ./SquareCalculator [flags]
```

#### General flags:
* __`-f`__      sets the FILE to be analyzed. Must be followed by a string representing valid filepath. This flag cannot be set with a crystal lattice flag or with the -s flag.
* __`-l`__      sets the LENGTH DECREMENT by which a length will be decremented each iteration. A smaller numbers will get a more precise result but take longer. The default is 0.01. The final result will only be accurate based on the length decrement. This has no effect if the -s flag is set.
* __`help`__    shows these helpful descriptions.
#### Crystal lattice and simulator flags:
* __`--SC`__    indicates that a SIMPLE CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.
* __`--BCC`__   indicates that a BODY CENTERED CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.
* __`--FCC`__   indicates that a FACE CENTERED CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.
* __`-v`__      sets the VOLUME FRACTION. Must be followed by a positive double. Note that it has no effect if no lattice flag has been set.
* __`-d`__      sets the DEPTH. Must be followed by a positive int. Note that it has no effect if the -f or -s flag is also set.
* __`-m`__      sets the MAX DEPTH and automatically indicates that all depths up to the maxdepth should be analyzed. Must be followed by a positive int. Note that it has no effect if the -f flag is also set. Note that some of the .crcl files parsed might be overridden by later .crcl files with higher depths, so although the data will be taken properly, not all of the .crcl files of every depth will be present.
* __`-i`__      indicates that the program ITERATE through all depths by cutting slices of every depth up to the max depth from the set depth or 1, if no depth was set. Then it will analyze at each depth. Note that it has no effect if the -f flag is also set.
* __`-a`__      indicates that ALL circle files that are created or parsed will be analyzed. Note that it has no effect if the -f flag is also set.
* __`-s`__      indicates that a movie_0.xyz file from a SIMULATOR should be parsed and analyzed. There must be a movie_0.xyz file in the directory. This flag cannot be set with a crystal lattice flag or with the -f flag. The -l and -d flags have no effect with -s. See the [Simulator](#simulator) section below.

## File Formats
### Input files
#### Particle position "circle" files (*.crcl)
```
[number of particles] [diameter (1.0)] [box size] [volume fraction (can have dummy value)]
[x position] [y position] [opacity (only used for visualizer)]
[x position] [y position] [opacity]
[x position] [y position] [opacity]
...
```

There should be a number of lines after the header equal to `[number of particles]`. The `[diameter]` is generally `1`, so that the free-space length is in terms of the particle size. The `[box size]` should be in the same units as `[diameter]` and should be the size of the image. Finally, the `[volume fraction]` is only used to keep track of the value, but is not used in the calculations. If the volume fraction is unknown, a negative value can help indicate that. Each line following the header line should be a pair of doubles represent a particle that at the position described in that line. If not using the `[opacity]`, it can just be set to `0.99999` or something similar. Otherwise, it can be used as a number between 0 and 1 indicating how dark the circle should be for the visualizer, where lighter circles are supposed to be further away (deeper in the sample).

Example:
```
9 1.0 83.55533333 -1.0
64.79206667 0.3758666667 0.99999
2.051333333 2.674266667 0.99999
67.42993333 14.7334 0.99999
36.75553333 20.2894 0.99999
32.8416 27.88813333 0.99999
0.2868 34.95346667 0.99999
70.84446667 46.59573333 0.99999
81.302 57.742 0.99999
78.12006667 82.09406667 0.99999
```

Here, the image size is around 83.5 particle diameters, so all coordinates are between 0 and that bound. The points are from a TEM scan, and the volume fraction is irrelevant, so that is set to `-1.0`, and opacity is defaulted to `0.99999`.

### Output files
#### Free-space length data (output.txt)
```
[box size 1] [free space length 1] [area fraction 1] [volume fraction 1] [depth 1]
[box size 2] [free space length 2] [area fraction 2] [volume fraction 2] [depth 2]
...
```

Each line stores the output of a single run of the program with certain settings. The `[box size]` is the size of the entire image of particles, in units of particle diameters. The `[free space length]` is the calculated side length of the square that contains 0 particles more than any other number of particles when placed randomly in the box. It is also in units of particle diameters. The `[area fraction]` is the fraction of the 2D image taken up by particles, out of the whole area.

Note: both `[volume fraction]` and `[depth]` should be ignored if the program analyzed a given file of particle positions (such as a TEM image). This means that it was neither particles from a crystal lattice nor a frame from the simulation. The `[volume fraction]` is the fraction of the 3D space that is taken up by particles, out of the whole volume. The 2D image may be a slice of the 3D image that has been flattened into 2D. The `[depth]` is the height of the slice, again in units of particle diameters.

Example:
```
51.177554 6.667554 0.010124 0.002000 3.333330
51.177554 7.237554 0.010990 0.002000 3.333330
51.177554 8.677554 0.010423 0.002000 3.333330
51.177554 8.537554 0.008337 0.002000 3.333330
51.177554 9.017554 0.006850 0.002000 3.333330
51.177554 8.217554 0.008898 0.002000 3.333330
29.928815 2.748815 0.043918 0.010000 3.333330
29.928815 3.318815 0.042464 0.010000 3.333330
29.928815 2.838815 0.053807 0.010000 3.333330
29.928815 2.568815 0.047782 0.010000 3.333330
29.928815 3.058815 0.052229 0.010000 3.333330
29.928815 4.538815 0.042347 0.010000 3.333330
```

Here, 12 samples give the free-space length (in units of particle diameters) based on given particle configurations. There are two sets of 6 trials, where each set has a specific volume concentration with simulated particles. Each trial is a slice of a 3D image of the simulated particles. The depth was held constant.

See `analyzeData.py` in [Helpful Tools](#helpful-tools) below for a python file that analyzes the output data to find a line of best fit.

#### Mode particles file (mode.txt)
```
[box size]
[initial side length] [mode number of particles contained]
[next side length] [mode number of particles contained]
...
[next side length] [mode number of particles contained]
[final side length] 0
```

After the first line, each line represents a tested box size. The `[side length]` is the size of that box. It is units of particle diameters. When a large number of boxes are randomly placed onto the image, and the number of particles contained are counted, the mode (more boxes contain that many particles than any other number of particles) is the `[mode number of particles contained]`. The side length of the first box with a mode of `0` contained particles is the free-space length, so the simulation will stop when it gets to a box that has a mode of `0`.

Example:
```
10.235511
0.807960 2
0.806960 2
0.805960 2
...
0.018960 1
0.017960 1
0.016960 0
```

Here, the first tested side length is `0.807960`, and the size of the box is decreased by `0.001` every iteration. The initial box has a mode of 2 particles contained. The free space length is `0.016960` because that is the largest box containing a mode of `0` particles.

Note that if the initial box has a mode of 0 particles, the side length is increased until the mode isn't `0`. This ensures that the largest box with a mode of `0` is found, as opposed to an arbitrary one with a mode of `0`.

#### Particles per square (distribution.txt)
```
[box size] [current side length] [mode with given square length]
0 [number of boxes with 0 particles]
1 [number of boxes with 1 particle]
2 [number of boxes with 2 particles]
...
```

As the program goes through different side lengths, it will periodically record data on the number of squares that had a each number of particles, for a given square size. The histogram for the final square size tested (the free-space length) will always be recorded. The largest square size to contain 0 particles more often than any other number of particles is the free-space length. The `[box size]` is the size of the entire image. The `[current side length]` is the size of the square. The `[mode]` is the most common number of particles contained in teh square when many are randomly placed throughout the entire image. The `[box size]` and the `[side length]` are in units of particle diameters.

Intermediate histogram example:
```
47.509033 7.509033 6
0 3
1 105
2 360
3 847
4 1772
5 2349
6 2736
7 2493
8 2112
9 1875
10 1625
11 1212
12 895
13 691
14 446
15 290
16 148
17 36
18 5
19 0
20 0
```

Last histogram example:
```
47.509033 2.129033 0
0 7336
1 7265
2 3704
3 1299
4 324
5 70
6 2
7 0
8 0
9 0
10 0
11 0
12 0
13 0
14 0
15 0
16 0
17 0
18 0
19 0
20 0
```

Here, the first example is a histogram taken with a square size greater than the free-space length. The mode is 6 because more boxes (2736) contain 6 particles than any other number of particles. Because the mode is greater than 0, this is not the free-space length. The second example shows a box size where the square length is the free-space length. That is because there, the mode is 0.

See `HistogramVisualizer.py` in [Helpful Tools](#helpful-tools) below for a python file that analyzes the output data to find a line of best fit.

### Intermediate files
#### Free-space length box "square" files (*.sqr)
```
[number of squares] [box size]
[x position] [y position] [side length]
[x position] [y position] [side length]
[x position] [y position] [side length]
...
```

This file is typically used by the visualizer for drawing squares on a picture of the particles. There should be a number of lines after the header equal to `[number of particles]`. This is typically 1. `[x position]` and `[y position]` are used for rendering the square. `[side length]` is the calculated free-space length, which is the side length of the square for which the mode number of particles contained is 0.

Example:
```
1 10.235511
7.922514 10.171255 0.016960
```

Here, there will be 1 square drawn of side length `0.016960` at position `(7.922514, 10.171255)`. The size of the entire image is `10.235511`.

## Helpful tools
### HistogramVisualizer.py
TO DO
### analyzeData.py
TO DO
### Visualizer
TO DO

## Simulator
The -s flag indicates that a simulated 3D cube of particles will be analyzed. The program will parse a movie_0.xyz file (see below), then cut the resulting cube into slices with a thickness equal to the depth, outputting a .crcl file for each slice (on all 3 axes). Then, each .crcl file is analyzed. movie_0.xyz files can be created using [Kai Zhang](https://github.com/statisticalmechanics)'s [Hard Sphere simulator](https://github.com/statisticalmechanics/hardsphere). 

When the program is being run in this mode, settings can be set using an external file. The file *makecalcinput* can be editted to set the following parameters.
* framesToIgnore — The number of frmes of the simulation from the movie file to ignore. Because the Hard Sphere simulator begins with the particles in a lattice configuration before they move randomly, ignoring the first few frames, say 10, is necessary to get particles in random 3D positions.
* framesToRecord — This is the number of frames to parse and slice up. This is typically 1, but especially with very large depths, if a single frame doesn't provide enough samples, then this can be increased.
* depth — This OVERRIDES a depth provided with the -d flag.
* squareLengthDecrement — This OVERRIDES a decrement provided with the -l flag.

## Current Status
Please note: this README is incomplete. The program needs more testing and this readme needs to be expanded. These are some of the update plans.

Currently, the focus has been on `/Source/`. The other folders can be ignored.
* Add example files to demonstrate the program into the directory, with description in readme
* Add explanation of movie files to README and link to Kai's work (with permission). Also add calcinput and makecalcinput for when running the program with -s.
* Add polydispersity to main source folder
* Figure out about running the visualizer

## Acknowledgements
This program was made with the help of [Kai Zhang](https://github.com/statisticalmechanics) for Prof. Sanat Kumar in the Department of Chemical Engineering at Columbia University. It would not have been possible without the assistance and guidance of Kai.
