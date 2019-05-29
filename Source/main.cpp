#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "CrystalSimulator.hpp"
#include "Parser.hpp"
#include "SquareHeuristic.hpp"
#include "Visualizer.hpp"

// #define PACKING_TYPE SC
// #define ITERATE_THROUGH_FILES
// #define MAX_DEPTH 10
// #define TEM_IMAGE

#define BUFFER_SIZE 256

struct malformed_command : public std::runtime_error {
public:
    malformed_command(std::string str) : std::runtime_error(str) {}
};

std::string help =
    (std::string)"This program calculates the free space length of a 2D image of particle\n\tlocations. The program can analyze files of particle locations (-f), or it can\n\tsimulate and analyze slices of a crystalline lattice of particles (--SC, --BCC,\n\t--FCC), or it can parse and analyze a simulation of nanoparticles in 3D (-s).\n" +
    (std::string)"Expected: ./SquareCalculator [flags]\n" +
    (std::string)"General flags:\n" +
    (std::string)"-f    * Sets the FILE to be analyzed. Must be followed by a string representing\n\tvalid filepath. This flag cannot be set with a crystal lattice flag or with the\n\t-s flag.\n" +
    (std::string)"-l    * Sets the LENGTH DECREMENT by which a length will be decremented each\n\titeration. A smaller numbers will get a more precise result but take longer. The\n\tdefault is 0.01. The final result will only be accurate based on the length\n\tdecrement. This has no effect if the -s flag is set.\n" +
    (std::string)"help  * You seemed to have figured this one out already...\n" +
    (std::string)"Crystal lattice and simulator flags:\n" +
    (std::string)"--sc * Indicates that a SIMPLE CUBIC lattice should be created and analyzed.\n\tThis flagcannot be set with -f or -s.\n" +
    (std::string)"--bcc * Indicates that a BODY CENTERED CUBIC lattice should be created and\n\tanalyzed.This flag cannot be set with -f or -s.\n" +
    (std::string)"--fcc * Indicates that a FACE CENTERED CUBIC lattice should be created and\n\tanalyzed.This flag cannot be set with -f or -s.\n" +
    (std::string)"-v    * Sets the VOLUME FRACTION. Must be followed by a positive double. Note\n\tthat it has no effect if no lattice flag has been set.\n" +
    (std::string)"-d    * Sets the DEPTH. Must be followed by a positive int. Note that it has no\n\teffect if the -f or -s flag is also set.\n" +
    (std::string)"-m    * Sets the MAX DEPTH and automatically indicates that all depths up to the\n\tmaxdepth should be analyzed. Must be followed by a positive int. Note that it\n\thas no effect if the -f flag is also set.\n" +
    (std::string)"-i    * Indicates that the program ITERATE through all depths by cutting slices\n\tof every depth up to the max depth from the set depth or 1, if no depth was set.\n\tThen it will analyze at each depth. Note that it has no effect if the -f flag is\n\talso set.\n" +
    (std::string)"-a    * Indicates that ALL circle files that are created or parsed will be\n\tanalyzed. Note that it has no effect if the -f flag is also set.\n" +
    (std::string)"-s    * Indicates that a movie_0.xyz file from a SIMULATOR should be parsed and\n\tanalyzed. There must be a movie_0.xyz file in the directory. This flag cannot be\n\tset with a crystal lattice flag or with the -f flag. The -l and -d flags have no\n\teffect with -s. See the README for more details.\n";

void run(int argc, char const* argv[]);
std::vector<std::string> makeCircleFiles(PackingType p, double volumeFraction, int* maxDepth);
std::vector<std::string> parse (int framesToIgnore, int framesToRecord, int* depth, double* volumeFraction, int* maxDepth);
void processCircles(const std::string& circleFilename, double squareLengthDecrement, double volumeFraction, int depth);

void readInput(int* framesToIgnore, int* framesToRecord, int* depth, double* squareLengthDecrement);
std::string writeOutput(std::string squareFilename, double areaFraction, double volumeFraction, int depth, std::string outputFilename = "output.txt");

int main(int argc, char const* argv[]) {
    std::cout << std::endl;
    try {
        run(argc, argv);
    } catch (malformed_command e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

void run(int argc, char const* argv[]) {
    // bool TEM = false;
    PackingType packing = NoCrystal;
    bool iterateThroughFiles = false;
    bool iterateThroughDepths = false;
    bool analyzeSimulation = false;

    // -1 will indicate later if it was not changed
    int framesToIgnore, framesToRecord, depth = -1, maxDepth = std::numeric_limits<int>::max();
    double volumeFraction = -1.0, squareLengthDecrement = 0.01;

    std::string inputtedFilename = "";

    /* Impossible combinations:
     * -s & -l
     * -s & -d
     * --sc / --bcc / --fcc & -f
     * --sc / --bcc / --fcc & -s
     * -f & -s
     * -v & !(--sc / --bcc / --fcc)
     * -i & -f
     * -m & -f
     * -a & -f
     */
    for (int a = 1; a < argc; a++) {
        std::string arg = std::string(argv[a]);
        // Convert the arg to lower case
        for (int b = 0; b < arg.size(); b++) {
            if (arg[b] >= 'A' && arg[b] <= 'Z') {
                arg[b] = (char)(arg[b] - 'A' + 'a');
            }
        }
        if (arg == "--sc") {
            packing = SC;
            std::cout << "Analyzing simple cubic" << std::endl;
        } else if (arg == "--bcc") {
            packing = BCC;
            std::cout << "Analyzing body centered cubic" << std::endl;
        } else if (arg == "--fcc") {
            packing = FCC;
            std::cout << "Analyzing face centered cubic" << std::endl;
        } else if (arg == "-d") {
            a += 1;
            std::cout << "The -d flag has no impact with the -s flag. Use the calcinput file with makecalcinput to specify the depth." << std::endl;
            std::string err = "The -d flag must be followed by an int representing the desired depth.";
            if (a < argc) {
                try {
                    depth = std::stoi(argv[a]);
                    std::cout << "depth set to " << depth << std::endl <<
                                 "Note that this will be overriden by the input from the settings file if parsing a simulation." << std::endl;
                } catch (std::invalid_argument e) {
                    throw malformed_command(err);
                }
            } else {
                throw malformed_command(err);
            }
        } else if (arg == "-m") {
            a += 1;
            std::string err = "The -m flag must be followed by an int representing the desired maximum depth.";
            if (a < argc) {
                try {
                    maxDepth = std::stoi(argv[a]);
                    iterateThroughDepths = true;
                    std::cout << "max depth to analyze set to " << depth << std::endl <<
                                 "Iterating through depths set" << std::endl;
                } catch (std::invalid_argument e) {
                    throw malformed_command(err);
                }
            } else {
                throw malformed_command(err);
            }
        } else if (arg == "-v") {
            a += 1;
            std::string err = "The -v flag must be followed by a double representing the volume fraction.";
            if (a < argc) {
                try {
                    if (packing == NoCrystal) {
                        std::cout << "Volume fraction is unused in cases other than with crystal lattices." << std::endl;
                    }
                    volumeFraction = std::stod(argv[a]);
                    std::cout << "volume fraction set to " << volumeFraction << std::endl;
                } catch (std::invalid_argument e) {
                    throw malformed_command(err);
                }
            } else {
                throw malformed_command(err);
            }
        } else if (arg == "-l") {
            a += 1;
            std::string err = "The -l flag must be followed by a double representing the length decrement per iteration.";
            std::cout << "The -l flag has no effect with the -s flag. Use the calcinput file with makecalcinput to specify the decrement." << std::endl;
            if (a < argc) {
                try {
                    squareLengthDecrement = std::stod(argv[a]);
                    std::cout << "Length decrement set to " << squareLengthDecrement << std::endl;
                } catch (std::invalid_argument e) {
                    throw malformed_command(err);
                }
            } else {
                throw malformed_command(err);
            }
        } else if (arg == "-i") {
            std::cout << "Iterating through depths set" << std::endl;
            iterateThroughDepths = true;
        } else if (arg == "-a") {
            std::cout << "All circle files created or parsed will be analyzed" << std::endl;
            iterateThroughFiles = true;
        } else if (arg == "-f") {
            a += 1;
            std::string err = "The -f flag must be followed by a string representing the crcl filename.\n";
            if (a < argc) {
                inputtedFilename = argv[a];
                std::cout << "File to analyze set to " << inputtedFilename << std::endl;
            } else {
                throw malformed_command(err);
            }
        } else if (arg == "-s") {
            std::cout << "Parsing, slicing, and analzing 3D simulation file \"movie_0.xyz\"" << std::endl;
            std::cout << "Note that the -d and -l flags will have no effect. The depth and decrement are taken from the file calcinput, which can be made with makecalcinput." << std::endl;
            analyzeSimulation = true;
        } else if (arg == "HELP") {
            std::cout << help << std::endl;
            return;
        } else {
            std::cout << "Unknown argument " << a << ": " << arg << std::endl;
        }
    }
    
    if (packing != NoCrystal && inputtedFilename != "") {
        throw malformed_command("Cannot both create a crystal lattice and analyze a separate inputted file.");
    } else if (packing != NoCrystal && analyzeSimulation) {
        throw malformed_command("Cannot both create a crystal lattice and use the hard sphere simulator.");
    } else if (inputtedFilename != "" && analyzeSimulation) {
        throw malformed_command("Cannot both analyze a separate inputted file and use the hard sphere simulator.");
    }

    // Catch all malformed commands that had an incorrect combination of flags
    if (packing == NoCrystal && !analyzeSimulation && inputtedFilename == "") {
        std::cout << help << std::endl << std::endl;
        throw malformed_command("No action specified. Use -f, -s, or a crystal packing flag (--SC, --BCC, --FCC) to indicate what to do.");
    } else if (packing != NoCrystal && (volumeFraction <= 0 || volumeFraction >= 1)) {
        throw malformed_command("Invalid volumeFraction. Must be set using the -v flag to a number between 0 and 1.");
    }
    
    if (analyzeSimulation) {
        std::cout << "Analyzing movie_0.xyz file using the specifications in the \"calcinput\".\n";
        readInput(&framesToIgnore, &framesToRecord, &depth, &squareLengthDecrement);
    }

    depth = inputtedFilename != "" ? -1 : depth != -1 ? depth : iterateThroughDepths ? 1 : 3; // Depth defaults to 1 if iterating through all depths, otherwise to 3
    maxDepth = iterateThroughDepths && inputtedFilename == "" ? maxDepth : depth; // maxDepth is the max int by default
    
    // If not already set, maxDepth is set to the largest that fits in the box in makeCircleFiles(...) or parse(...)
    for (/* depth is already initialized. 1 by default */; depth <= maxDepth; depth++) {
        std::vector<std::string> circleFilenames;
        int index = 0;
        // int prevDepth = depth;
        if (inputtedFilename != "") {
            circleFilenames.push_back(inputtedFilename);
        } else if (packing != NoCrystal) {
            circleFilenames = makeCircleFiles(packing, volumeFraction, &maxDepth);
            std::cout << "\nDepth: " << depth << std::endl;
            std::cout << "Square length decrement (per iteration): " << squareLengthDecrement << std::endl;
        } else {
            circleFilenames = parse(framesToIgnore, framesToRecord, &depth, &volumeFraction, &maxDepth);
            std::cout << "\nFrames to ignore: " << framesToIgnore << std::endl;
            std::cout << "Frames to record: " << framesToRecord << std::endl;
            std::cout << "Depth: " << depth << std::endl;
            std::cout << "Square length decrement (per iteration): " << squareLengthDecrement << std::endl;
        }

        do {
            std::string circleFilename = circleFilenames[index];

            processCircles(circleFilename, squareLengthDecrement, volumeFraction, depth);
            
            index++;
        } while (iterateThroughFiles && circleFilenames[index] != circleFilenames.back());
        // if (prevDepth != depth) break; // The parser changes depth only if the depth is the max box size. In that case, no need to keep iterating through depths.
    }
}

std::vector<std::string> makeCircleFiles(PackingType p, double volumeFraction, int* maxDepth) {
    std::cout << std::endl << "Initializing CrystalSimulator" << std::endl;

    // std::cout << "p: " << p << std::endl;

    int numParticles = p == FCC ? 500 : p == BCC ? 432 : 512;

    // std::cout << "numParticles: " << numParticles << std::endl;
    
    CrystalSimulator crystalSimulator(numParticles, volumeFraction, p);
    std::vector<std::string> circleFilenames = { crystalSimulator.printLattice() };
    std::cout << circleFilenames.size() << " circle files" << std::endl;

    *maxDepth = std::min(*maxDepth, (int)crystalSimulator.getSideLength());

    return circleFilenames;
}

std::vector<std::string> parse(int framesToIgnore, int framesToRecord, int* depth, double* volumeFraction, int* maxDepth) {
    std::cout << std::endl << "Initializing parser" << std::endl;

    std::cout << "Depth per layer: " << *depth << std::endl;
    Parser parser("movie_0.xyz", framesToIgnore, framesToRecord, depth);
    
    *volumeFraction = parser.getVolumeFraction();

    *maxDepth = std::min(*maxDepth, (int)parser.getBoxSize());
    
    std::cout << "Parsing" << std::endl;

    return parser.outputCircles();
}

void processCircles(const std::string& circleFilename, double squareLengthDecrement, double volumeFraction, int depth) {
    std::cout << "Initializing heuristic" << std::endl;
    SquareHeuristic algorithm;
    algorithm.setCircleFile(circleFilename);

    std::cout << "Using circle file: " << circleFilename << std::endl;
    std::string squareFilename = circleFilename;
    squareFilename.erase(squareFilename.size() - 4); // Erase "crcl" extension and replace with "sqr"
    algorithm.setOutputFilename(squareFilename + "sqr");
    
    std::cout << "Solving for square size" << std::endl;
    
    Visualizer visualizer(circleFilename, "");
    visualizer.saveImage();
    
    squareFilename = algorithm.outputSquares(squareLengthDecrement, 1, 20000, (pow(10.0, -0.399) * pow(visualizer.getAreaFraction(), -0.650)) * 1.25);
    // squareFilename = algorithm.outputSquares(squareLengthDecrement, 1, 20000, visualizer.getAreaFraction() * 2.0);
    
    std::cout << std::endl << "Area fraction: " << visualizer.getAreaFraction() << std::endl; 
    std::string outputFilename = writeOutput(squareFilename, visualizer.getAreaFraction(), volumeFraction, depth);
    
    std::cout << std::endl << "Circle filename: " << circleFilename << std::endl << "Square filename: " << squareFilename << std::endl << "Output filename: " << outputFilename << std::endl;
}

void readInput(int* framesToIgnore, int* framesToRecord, int* depth, double* squareLengthDecrement) {
    char buffer[BUFFER_SIZE];
    
    std::ifstream input("calcinput");
    
    input.getline(buffer, BUFFER_SIZE);
    *framesToIgnore = std::stoi(buffer);
    
    input.getline(buffer, BUFFER_SIZE);
    *framesToRecord = std::stoi(buffer);
    
    input.getline(buffer, BUFFER_SIZE);
    *depth = std::stoi(buffer);
    
    input.getline(buffer, BUFFER_SIZE);
    *squareLengthDecrement = std::stod(buffer);
    
    input.close();
}

/* Append to an existing file the following information on a new line:
 [boxSize] [squareSize] [area fraction] [volume fraction] [depth]
 */
std::string writeOutput(std::string squareFilename, double areaFraction, double volumeFraction, int depth, std::string outputFilename) {
    std::ifstream input;
    input.open(squareFilename);
    
    char buffer[BUFFER_SIZE];
    input.getline(buffer, BUFFER_SIZE);
    std::string line(buffer);
    line = line.substr(line.find_first_of(' ') + 1); // Ignore number of boxes
    double boxSize = std::stod(line);
    input.getline(buffer, BUFFER_SIZE);
    line = std::string(buffer);
    line = line.substr(line.find_first_of(' ') + 1); // Ignore x position of square
    line = line.substr(line.find_first_of(' ') + 1); // Ignore y position of square
    double squareSize = std::stod(line);
    
    std::ofstream file;
    file.open(outputFilename, std::ofstream::app);
    std::string str = std::to_string(boxSize) + " " + std::to_string(squareSize) + " " + std::to_string(areaFraction) + " " + std::to_string(volumeFraction) + " " + std::to_string(depth) + "\n";
    file.write(str.c_str(), str.size());
    file.close();
    
    return outputFilename;
}
