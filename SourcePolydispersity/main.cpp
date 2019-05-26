#include <iostream>
#include <string>
#include <fstream>

#include "CrystalSimulator.hpp"
#include "Parser.hpp"
#include "SquareHeuristic.hpp"
#include "Visualizer.hpp"

//#define PACKING_TYPE SC
//#define ITERATE_THROUGH_FILES
//#define MAX_DEPTH 19
#define MAX_DEPTH 5

#define BUFFER_SIZE 256

#define DIAMETER 1.0

void readInput(int* framesToIgnore, int* framesToRecord, int* depth, double* squareLengthDecrement);
std::string writeOutput(std::string squareFilename, double areaFraction, double volumeFraction, int depth, std::string outputFilename = "output.txt");

int main(int argc, char const* argv[]) {
	int framesToIgnore, framesToRecord, depth;
	double squareLengthDecrement, deviation = argc > 1 ? std::stod(argv[1]) : 0.0;
	
	readInput(&framesToIgnore, &framesToRecord, &depth, &squareLengthDecrement);

#ifndef PACKING_TYPE		
	if (argc > 1 && std::stod(argv[1]) != 0) { //If first command line argument isn't 0 treat it as the depth
		depth = std::abs(std::stod(argv[1]));
	}
#endif

#ifdef MAX_DEPTH
	for (depth = argc > 2 ? std::stoi(argv[2]) : 1; depth <= MAX_DEPTH; depth++) {
#endif
		std::cout << std::endl << "Frames to ignore: " << framesToIgnore << std::endl;
		std::cout << "Frames to record: " << framesToRecord << std::endl;
		std::cout << "Square length decrement (per iteration): " << squareLengthDecrement << std::endl;
		
		std::cout << std::endl << "Mean diameter: " << DIAMETER << std::endl;
		std::cout << "Standard deviation: " << deviation << std::endl;
#ifdef PACKING_TYPE
		std::cout << std::endl << "Initializing CrystalSimulator" << std::endl;
		
		PackingType p = PACKING_TYPE;
		double volumeFraction = std::stod(argv[1]);
		int numParticles = p == FCC ? 500 : p == BCC ? 432 : 512;
		
		CrystalSimulator crystalSimulator(numParticles, volumeFraction, p);
		std::vector<std::string> circleFilenames = { crystalSimulator.printLattice() };
		std::cout << circleFilenames.size() << " circle files" << std::endl;
#endif
#ifndef PACKING_TYPE
		std::cout << std::endl << "Initializing parser" << std::endl;

		std::cout << "Depth per layer: " << depth << std::endl;
		Parser parser("movie_0.xyz", framesToIgnore, framesToRecord, &depth, DIAMETER, deviation);
		
		double volumeFraction = parser.getVolumeFraction();
		
		std::cout << "Parsing" << std::endl;

		std::vector<std::string> circleFilenames = parser.outputCircles();
#endif
		
		int index = 0;
#ifndef PACKING_TYPE		
		/*if (argc > 1 && std::stoi(argv[1]) < 1) { //If first command line argument is 0 or less don't do squares
			std::cout << "Skipping square heuristic \n\t(command line argument of 0)" << std::endl << "Circle filename: " << circleFilenames[index] << std::endl;
			return 0;
		}*/
#endif
		SquareHeuristic algorithm;
	  
#ifdef ITERATE_THROUGH_FILES
	do {
#endif
		std::cout << "Initializing heuristic" << std::endl;
		std::cout << "Using circle file: " << circleFilenames[index] << std::endl;
		
		algorithm.setCircleFile(circleFilenames[index]);
		
		std::string squareFilename = circleFilenames[index];
		squareFilename.erase(squareFilename.size() - 4); // Erase "crcl" extension and replace with "sqr"
		algorithm.setOutputFilename(squareFilename + "sqr");
		
		std::cout << "Solving for square size" << std::endl;
		
		Visualizer visualizer(circleFilenames[index], "squares.txt");
		visualizer.saveImage();
		
		squareFilename = algorithm.outputSquares(squareLengthDecrement, 1, 20000, (pow(10.0, -0.399) * pow(visualizer.getAreaFraction(), -0.650)) * 1.25);
	   
		std::cout << std::endl << "Area fraction: " << visualizer.getAreaFraction() << std::endl; 
		std::string outputFilename = writeOutput(squareFilename, visualizer.getAreaFraction(), volumeFraction, depth);
		
		std::cout << std::endl << "Circle filename: " << circleFilenames[index] << std::endl << "Square filename: " << squareFilename << std::endl << "Output filename: " << outputFilename << std::endl;
		
		circleFilenames[index];
#ifdef ITERATE_THROUGH_FILES
	} while (filename != circleFilenames.end());
#endif
#ifdef MAX_DEPTH
	}
#endif

	return 0;
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
