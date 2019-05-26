//
//  SquareHeuristic.hpp
//  SquareHeuristic
//
//  Created by Jake Sanders on 6/29/17.
//  Copyright © 2017 Jake Sanders. All rights reserved.
//

//#define NUM_THREADS 3.0

#ifdef NUM_THREADS
#include <thread>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>

#include "Point.hpp"

#define OUTPUT_SQUARE_FILE

#define BUFFER_SIZE 256
#define UPDATE_SECONDS 3
#define AMOUNT_DECREMENT_PER_OUTPUT 8 // Output the distribution whenever the box length has been decremented by 1
#define SEPARATING_CHARACTER ' ' // Which character separates the data in the .txt files

class SquareHeuristic {
public:
	SquareHeuristic();
	
	void setCircleFile(std::string circleFilename);
	void setOutputFilename(std::string filename);
	std::string outputSquares(double boxLengthDecrement, int numCases, int numTests, double startingLength, /*bool nonVisual = true,*/ bool test = false);
	
	double getRadius();
	double getBoxSize();
	
private:
#ifdef NUM_THREADS
	std::vector<std::thread> threads;
#endif
	
	std::vector<Point> circles;
	double boxSize, volumeFraction;
	std::string outputFilename = "squares.txt";
	
	double heuristic(const std::vector<Point>& circles, double boxSize, Point* exampleSquare, double boxLengthDecrement, int numTests, double startingLength);
	bool isContained(const Point& circle, Point square, double sideLength, double regionLength, bool advancedAlgorithm = true);
	std::vector<Point> getCircles(const std::string& filename);
};
