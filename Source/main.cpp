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
#define MAX_DEPTH 10
// #define TEM_IMAGE

#define BUFFER_SIZE 256

struct malformed_command : public std::exception{};

std::string help =
    (std::string)"This program calculates the free space length of a 2D image of particle locations. The program can analyze files of particle locations (-f), or it can simulate and analyze slices of a crystalline lattice of particles (--SC, --BCC, --FCC), or it can parse and analyze a simulation of nanoparticles in 3D (-s).\n" +
    (std::string)"Expected: ./SquareCalculator [flags]\n" +
    (std::string)"flags:\n" +
    (std::string)"-f      sets the FILE to be analyzed. Must be followed by a string representing valid filepath. This flag cannot be set with a crystal lattice flag or with the -s flag.\n" +
    (std::string)"--SC    indicates that a SIMPLE CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.\n" +
    (std::string)"--BCC   indicates that a BODY CENTERED CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.\n" +
    (std::string)"--FCC   indicates that a FACE CENTERED CUBIC lattice should be created and analyzed. This flag cannot be set with -f or -s.\n" +
    (std::string)"-d      sets the DEPTH. Must be followed by a positive int. Note that it has no effect if the -f flag is also set.\n" +
    (std::string)"-m      sets the MAX DEPTH and automatically indicates that all depths up to the max depth should be analyzed. Must be followed by a positive int. Note that it has no effect if the -f flag is also set.\n" +
    (std::string)"-v      sets the VOLUME FRACTION. Must be followed by a positive double. Note that it has no effect if no lattice flag has been set.\n" +
    (std::string)"-i      indicates that the program should cut slices of every depth up to the max depth from the set depth or 1, if no depth was set. The program will ITERATE through each depth. Note that it has no effect if the -f flag is also set.\n" +
    (std::string)"-a      indicates that ALL circle files that are created or parsed will be analyzed. Note that it has no effect if the -f flag is also set.\n" +
    (std::string)"-s      indicates that a movie_0.xyz file from a SIMULATOR should be parsed and analyzed. This flag cannot be set with a crystal lattice flag or with the -f flag.\n" +
    (std::string)"help    You seemed to have figured this one out already : )";

std::vector<std::string> makeCircleFiles(PackingType p, double volumeFraction);
std::vector<std::string> parse (int framesToIgnore, int framesToRecord, bool doSquares, int* depth, double* volumeFraction);
void processCircles(const std::string& circleFilename, double squareLengthDecrement, double volumeFraction, int depth);

void readInput(int* framesToIgnore, int* framesToRecord, int* depth, double* squareLengthDecrement);
std::string writeOutput(std::string squareFilename, double areaFraction, double volumeFraction, int depth, std::string outputFilename = "output.txt");

int main(int argc, char const* argv[]) {
    // bool TEM = false;
    PackingType packing = NoCrystal;
    bool iterateThroughFiles = false;
    bool iterateThroughDepths = false;
    bool analyzeSimulation = false;

    int framesToIgnore, framesToRecord, depth = 1, maxDepth = MAX_DEPTH;
    double volumeFraction = -1.0, squareLengthDecrement;

    std::string inputtedFilename = "";

    for (int a = 0; a < argc; a++) {
        std::string arg = std::string(argv[a]);
        /*if (arg == "--TEM") {
            TEM = true;
            setModeFlag(&modeFlag);
            std::cout << "Analyzing TEM image file" << std::endl;
        } else*/ if (arg == "--SC") {
            packing = SC;
            if (packing != NoCrystal) {
                std::cout << "Cannot analyze a crystal lattice if -f has been set." << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "Analyzing simple cubic" << std::endl;
        } else if (arg == "--BCC") {
            packing = BCC;
            if (packing != NoCrystal) {
                std::cout << "Cannot analyze a crystal lattice if -f has been set." << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "Analyzing body centered cubic" << std::endl;
        } else if (arg == "--FCC") {
            packing = FCC;
            if (packing != NoCrystal) {
                std::cout << "Cannot analyze a crystal lattice if -f has been set." << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "Analyzing face centered cubic" << std::endl;
        } else if (arg == "-d") {
            a += 1;
            std::string err = "The -d flag must be followed by an int representing the desired depth.";
            if (a < argc) {
                try {
                    depth = std::stoi(argv[a]);
                    std::cout << "depth set to " << depth << std::endl <<
                                 "Note that this will be overriden by the input from the settings file if analyzing a non-TEM non-crystal file." << std::endl;
                } catch (std::invalid_argument e) {
                    std::cout << err << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                std::cout << err << std::endl;
                exit(EXIT_FAILURE);
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
                    std::cout << err << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                std::cout << err << std::endl;
                exit(EXIT_FAILURE);
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
                    std::cout << err << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                std::cout << err << std::endl;
                exit(EXIT_FAILURE);
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
                if (packing != NoCrystal) {
                    std::cout << "Cannot use a chosen file with the specified packing type." << std::endl;
                    exit(EXIT_FAILURE);
                }
                inputtedFilename = argv[a];
                std::cout << "File to analyze set to " << inputtedFilename << std::endl;
            } else {
                std::cout << err << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (arg == "-s") {
            std::cout << "Analyzing simulation file \"movie_0.xyz\"" << std::endl;
            analyzeSimulation = true;
        } else if (arg == "help") {
            std::cout << help << std::endl;
            exit(EXIT_SUCCESS);
        }
    }

    if (packing == NoCrystal && !analyzeSimulation && inputtedFilename == "") {
        std::cout << "No action specified. Use -f, -s, or a crystal packing flag (--SC, --BCC, --FCC) to indicate what to do." << std::endl << help << std::endl;
        exit(EXIT_FAILURE);
    }
    
    if (analyzeSimulation) {
        std::cout << "No inputted file and no crystal structure specified.\n" <<
                     "Analyzing movie_0.xyz file using the specifications in the \"calcinput\".";
        readInput(&framesToIgnore, &framesToRecord, &depth, &squareLengthDecrement);
    }

    maxDepth = iterateThroughDepths ? maxDepth : depth; /* maxDepth is MAX_DEPTH by default */

    for (/* depth is already initialized. 1 by default */; depth <= maxDepth; depth++) {
        std::vector<std::string> circleFilenames;
        int index = 0;
        int prevDepth = depth;
        if (inputtedFilename != "") {
            circleFilenames.push_back(inputtedFilename);
        } else if (packing != NoCrystal) {
            if (volumeFraction < 0) {
                std::cout << "Invalid volumeFraction. Must be set with the -v flag to a positive number." << std::endl;
            }
            circleFilenames = makeCircleFiles(packing, volumeFraction);
        } else {
            circleFilenames = parse(framesToIgnore, framesToRecord, &depth, &volumeFraction);

            std::cout << std::endl << "Frames to ignore: " << framesToIgnore << std::endl;
            std::cout << "Frames to record: " << framesToRecord << std::endl;
            std::cout << "Square length decrement (per iteration): " << squareLengthDecrement << std::endl;
        }

        do {
            std::string circleFilename = circleFilenames[index];

            processCircles(circleFilename, squareLengthDecrement, volumeFraction, depth);
            
            index++;
        } while (iterateThroughFiles && circleFilenames[index] != circleFilenames.back());
        if (prevDepth != depth) break; // The parser changes depth only if the depth is the max box size. In that case, no need to keep iterating through depths.
    }

    return 0;
}

std::vector<std::string> makeCircleFiles(PackingType p, double volumeFraction) {
    std::cout << std::endl << "Initializing CrystalSimulator" << std::endl;

    int numParticles = p == FCC ? 500 : p == BCC ? 432 : 512;
    
    CrystalSimulator crystalSimulator(numParticles, volumeFraction, p);
    std::vector<std::string> circleFilenames = { crystalSimulator.printLattice() };
    std::cout << circleFilenames.size() << " circle files" << std::endl;
    return circleFilenames;
}

std::vector<std::string> parse(int framesToIgnore, int framesToRecord, int* depth, double* volumeFraction) {
    std::cout << std::endl << "Initializing parser" << std::endl;

    std::cout << "Depth per layer: " << *depth << std::endl;
    Parser parser("movie_0.xyz", framesToIgnore, framesToRecord, depth);
    
    *volumeFraction = parser.getVolumeFraction();
    
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
    
    Visualizer visualizer(circleFilename, "squares.txt");
    visualizer.saveImage();
    
    squareFilename = algorithm.outputSquares(squareLengthDecrement, 1, 20000, (pow(10.0, -0.399) * pow(visualizer.getAreaFraction(), -0.650)) * 1.25);
    
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
