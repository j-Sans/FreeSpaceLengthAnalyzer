# Free-Space Length Analyzer
Analyzes nanoparticles in polymer matrices to quantify the dispersion and the spacing of the particles.

This program calculates the free space length of a 2D image of particle locations. The program can analyze files of particle locations (-f), or it can simulate and analyze slices of a crystalline lattice of particles (--SC, --BCC, --FCC), or it can parse and analyze a simulation of nanoparticles in 3D (-s).

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
* __`-l`__      sets the LENGTH DECREMENT by which a length will be decremented each iteration. A smaller numbers will get a more precise result but take longer. The default is 0.01. The final result will only be accurate based on the length decrement
* __`help`__    you seemed to have figured this one out already...
#### Crystal lattice and simulator flags:
* __`--SC`__    indicates that a SIMPLE CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.
* __`--BCC`__   indicates that a BODY CENTERED CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.
* __`--FCC`__   indicates that a FACE CENTERED CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.
* __`-v`__      sets the VOLUME FRACTION. Must be followed by a positive double. Note that it has no effect if no lattice flag has been set.
* __`-d`__      sets the DEPTH. Must be followed by a positive int. Note that it has no effect if the -f flag is also set.
* __`-m`__      sets the MAX DEPTH and automatically indicates that all depths up to the maxdepth should be analyzed. Must be followed by a positive int. Note that it has no effect if the -f flag is also set.
* __`-i`__      indicates that the program ITERATE through all depths by cutting slices of every depth up to the max depth from the set depth or 1, if no depth was set. Then it will analyze at each depth. Note that it has no effect if the -f flag is also set.
* __`-a`__      indicates that ALL circle files that are created or parsed will be analyzed. Note that it has no effect if the -f flag is also set.
* __`-s`__      indicates that a movie_0.xyz file from a SIMULATOR should be parsed and analyzed. There must be a movie_0.xyz file in the directory. This flag cannot be set with a crystal lattice flag or with the -f flag.

## File Formats
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

#### Free-space length box "square" files (*.sqr)
TO DO

#### Output files (output.txt)
TO DO

## Current Status
Please note: this README is incomplete. The program needs more testing and this readme needs to be expanded. These are some of the update plans.

Currently, the focus has been on `/Source/`. The other folders can be ignored.
* Complete testing of the program for TEM imagery
* Add brief description of how to compile it with make
* Add example files to demonstrate the program into the directory
* Add explanation of file format to README
* Add explanation of movie files to README and link to Kai's work (with permission). Also add calcinput and makecalcinput for when running the program with -s.
* Add examples to the README
* Figure out about running the visualizer...
