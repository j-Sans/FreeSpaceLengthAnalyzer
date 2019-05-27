# Free-Space Length Analyzer
Analyzes nanoparticles in polymer matrices to quantify the dispersion and the spacing of the particles.

This program calculates the free space length of a 2D image of particle locations. The program can analyze files of particle locations (-f), or it can simulate and analyze slices of a crystalline lattice of particles (--SC, --BCC, --FCC), or it can parse and analyze a simulation of nanoparticles in 3D (-s).

## How To Use It
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
