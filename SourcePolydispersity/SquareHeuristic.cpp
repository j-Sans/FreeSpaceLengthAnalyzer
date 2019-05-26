//
//  SquareHeuristic.cpp
//  SquareCalculator
//
//  Created by Jake Sanders on 6/29/17.
//  Copyright © 2017 Jake Sanders. All rights reserved.
//

#include "SquareHeuristic.hpp"

// Constructor

SquareHeuristic::SquareHeuristic() {
	srand((unsigned int)time(NULL));
}

// Public

void SquareHeuristic::setCircleFile(std::string circleFilename) {
	circles = getCircles(circleFilename);
}

void SquareHeuristic::setOutputFilename(std::string filename) {
	outputFilename = filename;
}

std::string SquareHeuristic::outputSquares(double boxLengthDecrement, int numCases, int numTests, double startingLength, /*bool nonVisual,*/ bool test) {
	Point square;
	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	std::ofstream output;
	std::string str;
	if (!test) {
		/*if (!nonVisual) output.open(outputFilename);
		else*/ output.open(outputFilename);
		str = std::to_string(numCases) + " " + std::to_string(boxSize) + "\n";
		output.write(str.c_str(), str.length());
	}
	
	for (int a = 0; a < numCases; a++) {
		double sideLength = heuristic(circles, boxSize, &square, boxLengthDecrement, numTests, startingLength);
		if (!test) {
			/*if (!nonVisual)*/ str = std::to_string(square.x) + " " + std::to_string(square.y) + " " + std::to_string(sideLength) + "\n";
			/*else str = std::to_string(volumeFraction) + " " + std::to_string(sideLength);*/
			output.write(str.c_str(), str.length());
		}
		std::cout << std::endl << "Trial " << a + 1 << ": " << sideLength << std::endl;
		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		double time = ((std::chrono::duration<double>)(now - start)).count();
		std::cout << "\tTime: " << time << " seconds" << std::endl;
		start = std::chrono::high_resolution_clock::now();
	}
	
	if (!test) output.close();
	
	return outputFilename;
}

double SquareHeuristic::getBoxSize() {
	return boxSize;
}

// Private

/*
 * A function to find the largest square in which the mode number of particles contained is 0.
 *
 * @param circles A vector of Points representing the centers of the particles.
 *
 * @return The largest square length, as a double.
 */
double SquareHeuristic::heuristic(const std::vector<Point>& circles, double boxSize, Point* square, double boxLengthDecrement, int numTests, double startingLength) {
	std::chrono::time_point<std::chrono::high_resolution_clock> prevTime = std::chrono::high_resolution_clock::now();
	int recordedBoxNum = 0;
	std::ofstream modeOutput;
	modeOutput.open("mode.txt");
	std::string str = std::to_string(boxSize) + "\n";
	modeOutput.write(str.c_str(), str.length());
	modeOutput.close();
	int recordBox = startingLength / boxLengthDecrement;
	double squareLength = startingLength;
	std::cout << "Starting with square length " << startingLength << std::endl;
	for (; squareLength > 0; squareLength -= boxLengthDecrement) {
		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		if ((int)((std::chrono::duration<double>)(now - prevTime)).count() > UPDATE_SECONDS) {
			std::cout << "Current square length: " << squareLength << std::endl;
			prevTime = std::chrono::high_resolution_clock::now();
		}
		
		int modeNumParticles[circles.size() + 1]; // Plus one to include zero circles and all circles
		memset(modeNumParticles, 0, sizeof(modeNumParticles));
		
#ifdef NUM_THREADS
		auto trialFunc = [](std::vector<Point> circles, double boxSize, Point* square, double numTests, int* modeNumParticles) {
			
			Point newSquare;
			if (square == nullptr) square = &newSquare;
			
			for (int trial = 0; trial < numTests / (NUM_THREADS + 1.0); trial++) {
				*square = Point(boxSize * (double)rand() / (double)RAND_MAX, boxSize * (double)rand() / (double)RAND_MAX);
				
				int numCircles = 0;
				for (auto circle = circles.begin(); circle != circles.end(); circle++) {
					if (isContained(*circle, *square, squareLength, boxSize)) numCircles++;
				}
				
				modeNumParticles[numCircles]++;
			}
		}
		
		for (int a = 0; a < NUM_THREADS; a++) {
			threads.push_back(std::thread(trialFunc, circles, boxSize, nullptr, numTests, modeNumParticles));
		}
		trailFunc(circles, boxSize, square, numTests, modeNumParticles);
		for (int a = 0; a < NUM_THREADS; a++) {
			threads[a].join();
		}
		
#endif
#ifndef NUM_THREADS
		
		for (int trial = 0; trial < numTests; trial++) {
			*square = Point(boxSize * (double)rand() / (double)RAND_MAX, boxSize * (double)rand() / (double)RAND_MAX);
			
			int numCircles = 0;
			for (auto circle = circles.begin(); circle != circles.end(); circle++) {
				if (isContained(*circle, *square, squareLength, boxSize)) numCircles++;
			}
			
			modeNumParticles[numCircles]++;
		}
#endif
		/*
		bool zeroHighestMode = true; // If true, then this square size works
		for (int a = 0; a < circles.size() + 1; a++) {
			if (modeNumParticles[a] > modeNumParticles[0]) {
				zeroHighestMode = false; // Another mode is greater than zero, so this is false
				break;
			}
		}
		if (zeroHighestMode) return squareLength;
		*/
		int mode = 0;
		for (int a = 0; a < circles.size() + 1; a++) {
			if (modeNumParticles[mode] < modeNumParticles[a]) mode = a;
		}

		modeOutput.open("mode.txt", std::ofstream::app);
		str = std::to_string(squareLength) + " " + std::to_string(mode) + "\n";
		modeOutput.write(str.c_str(), str.length());
		modeOutput.close();
		
		str = std::to_string(boxSize) + " " + std::to_string(squareLength) + " " + std::to_string(mode) + "\n";
		for (int a = 0; a < circles.size() + 1; a++) {
			str += std::to_string(a) + " " + std::to_string(modeNumParticles[a]) + "\n";
		}
		
		if (mode == 0) {
			std::ofstream file("distribution_" + std::to_string(recordedBoxNum++) + ".txt");
			file.write(str.c_str(), str.size());
			file.close();
			return squareLength;
		} else if (recordBox == (int)(squareLength / boxLengthDecrement)) {
			recordBox -= AMOUNT_DECREMENT_PER_OUTPUT / boxLengthDecrement;
			std::ofstream file("distribution_" + std::to_string(recordedBoxNum++) + ".txt");
			file.write(str.c_str(), str.size());
			file.close();
		}
	}
	
	if (squareLength == startingLength) {
		 std::cout << "Starting length too small. Trying length " << startingLength * 2.0 << std::endl;
		 return heuristic(circles, boxSize, square, boxLengthDecrement, numTests, startingLength * 2.0);
	}
   
	return 0;
}

/*
 * Checks if a given circle is contained within the given square. If advancedAlgorithm is manually set to false, a simpler algorithm that checks if the circle's center lies within the square. Otherwise, the algorithm checks if any part of the circle is within the square. This is done by checking if the center of the circle lies within the box, or is above, below, left, or right of the box less than it's radius from the box, or if the center is less than it's radius distance from the corner of the box.
 *
 * @param circle The center of the circle.
 * @param square The center of the square.
 * @param sideLength The side length of the square.
 * @param advancedAlgorithm An optional boolean automatically set to true that changes the algorithm used to calculate if the particles are in the square.
 *
 * @return If the given circle is contained within the square.
 */
bool SquareHeuristic::isContained(const Point& circle, Point square, double sideLength, double regionLength, bool advancedAlgorithm) {
	double halfSide = sideLength / 2.0;
	if (advancedAlgorithm) {
		double largerHalfSide = halfSide + (circle.diameter / 2.0);
		
		/*
		 * This finds the closest copy of the circle to the square, including periodic copies.
		 * If the circle (center) is more than half of the entire region away from the square (center), then there is a periodic copy closer.
		 * The round() quantity becomes 0 if the circle is closer, so no periodic copy is analyzed
		 * Otherwise, it becomes either -1 or 1, depending on which direction the circle is. It is negated so the point is moved back, closer to the square, by one region length.
		 * The displacement from the center to the new point must be between one half length in both the positive and negative directions.
		 * The quantity newCircle represents the position of the new copy of the circle.
		 */
		Point newCircle(circle.x - (regionLength * round((circle.x - square.x) / regionLength)), circle.y - (regionLength * round((circle.y - square.y) / regionLength)));
		
		if (newCircle.x < square.x - largerHalfSide || newCircle.x > square.x + largerHalfSide || newCircle.y < square.y - largerHalfSide || newCircle.y > square.y + largerHalfSide) return false; // If the circle's center is outside a larger box of side length equal to the regular side length plus the radius on each side, then it can't be within the smaller subregion. This should remove most particles from further review
		
		if (newCircle.x > square.x - largerHalfSide && newCircle.x < square.x + largerHalfSide && newCircle.y > square.y - halfSide && newCircle.y < square.y + halfSide) return true; // If the circle's center is within a radius' distance from the left or right side of the box (or is in the center box) it is contained
		
		if (newCircle.x > square.x - halfSide && newCircle.x < square.x + halfSide && newCircle.y > square.y - largerHalfSide && newCircle.y < square.y + largerHalfSide) return true; // If the circle's center is within a radius' distance from the top or bottom of the box it is contained
		
		// Check if the circle's center is within one radius distance from the corners of the box
		if (Point::getDistance(newCircle, Point(square.x - halfSide, square.y - halfSide)) < (circle.diameter / 2.0)) return true;
		if (Point::getDistance(newCircle, Point(square.x + halfSide, square.y - halfSide)) < (circle.diameter / 2.0)) return true;
		if (Point::getDistance(newCircle, Point(square.x + halfSide, square.y + halfSide)) < (circle.diameter / 2.0)) return true;
		if (Point::getDistance(newCircle, Point(square.x - halfSide, square.y + halfSide)) < (circle.diameter / 2.0)) return true;
		
		return false;
	} else {
		/*
		 * This finds the closest copy of the circle to the square, including periodic copies.
		 * If the circle (center) is more than half of the entire region away from the square (center), then there is a periodic copy closer.
		 * The round() quantity becomes 0 if the circle is closer, so no periodic copy is analyzed
		 * Otherwise, it becomes either -1 or 1, depending on which direction the circle is. It is negated so the point is moved back, closer to the square, by one region length.
		 * The displacement from the center to the new point must be between one half length in both the positive and negative directions.
		 * The quantity newCircle represents the displacement vector from the center of the square.
		 */
		Point newCircle(circle.x - square.x - (regionLength * round((circle.x - square.x) / regionLength)), circle.y - square.y - (regionLength * round((circle.y - square.y) / regionLength)));
		
		if (newCircle.x > -halfSide && newCircle.x < halfSide && newCircle.y > -halfSide && newCircle.y < halfSide) return true;
		return false;
	}
}

std::vector<Point> SquareHeuristic::getCircles(const std::string& filename) {
	std::ifstream input;
	input.open(filename);
	
	char buffer[BUFFER_SIZE];
	
	input.getline(buffer, BUFFER_SIZE);
	std::string str(buffer); // Get each line as a new string
	
	// Extract the number of circles and their radii
	unsigned long commaPos = str.find_first_of(SEPARATING_CHARACTER);
	int numCircles = std::stod(str.substr(0, commaPos));
	str = str.substr(commaPos + 1);
	commaPos = str.find_first_of(SEPARATING_CHARACTER);
	str = str.substr(commaPos + 1); // Skip the average diameter
	commaPos = str.find_first_of(SEPARATING_CHARACTER);
	boxSize = std::stod(str.substr(0, commaPos));
	str = str.substr(commaPos + 1);
	volumeFraction = std::stod(str);
	
	//Extract each of the circles' positions and add them to a vector
	std::vector<Point> vec;
	for (int a = 0; a < numCircles; a++) {
		input.getline(buffer, BUFFER_SIZE);
		str = std::string(buffer); // Get each line as a new string
		
		// Extract the number of circles and their radii
		commaPos = str.find_first_of(SEPARATING_CHARACTER);
		double x = std::stod(str.substr(0, commaPos));
		str = str.substr(commaPos + 1);
		commaPos = str.find_first_of(SEPARATING_CHARACTER);
		double y = std::stod(str.substr(0, commaPos));
		str = str.substr(commaPos + 1);
		commaPos = str.find_first_of(SEPARATING_CHARACTER);
		// Skip the depth	
		//		double transparency = std::stod(str.substr(commaPos + 1));
		str = str.substr(commaPos + 1);
		commaPos = str.find_first_of(SEPARATING_CHARACTER);
		double diameter = std::stod(str.substr(0, commaPos));
		std::cout << "Diameter: " << diameter << std::endl;	
		Point p(x, y);
        p.diameter = diameter;
		vec.push_back(p);
	}
	
	input.close();
	return vec;
}
