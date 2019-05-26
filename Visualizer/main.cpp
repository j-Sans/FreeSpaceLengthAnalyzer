//
// main.cpp
// Visualizer
//
// Created by Jake Sanders on 7/5/17
// Copyright ® 2017 Jake Sanders. All rights reserved.
//

#include <string>

#include "Visualizer.hpp"

int main(int argc, char* argv[]) {
    srand(time(NULL));
    
    std::string squareFilename = "project_0_0.sqr", circleFilename = "project_0_0.crcl", secondCircleFilename = "";   

    bool fromDistributionFile = false;
    
    if (argc > 2) {
        std::cout << "Manual filename input" << std::endl;
        squareFilename = argv[1];
        circleFilename = argv[2];
        if (argc > 3) secondCircleFilename = argv[3];
    } else if (argc > 1) {
        std::cout << "Using distribution file" << std::endl;
        squareFilename = "distribution_" + std::string(argv[1]) + ".txt";
        fromDistributionFile = true;
    }

    std::cout << "Initializing Visualizer" << std::endl;
    
    Visualizer visualizer(fromDistributionFile, squareFilename, circleFilename, secondCircleFilename);
    
//    bool leftPressed = false, rightPressed = false;
    
    visualizer.saveImage();
    std::cout << "Area fraction: " << visualizer.getAreaFraction() << std::endl;
    
    while (visualizer.isOpen()) {
        visualizer.render();
        
        /* Uncomment this section to be able to move between frames. Moving between frames takes a few seconds to recalculate new sqaures for that frame. Move using arrow keys */
//        visualizer.render(&leftPressed, &rightPressed);
//
//        // Shift index based on arrow key buttons to see previous/next frame
//        if (leftPressed && index > 0) {
//            index--;
//            squareFilename = algorithm.outputSquares();
//            visualizer.setCircleFile(circleFilenames[index]);
//            visualizer.setSquareFile(squareFilename);
//            std::cout << "\nRendering:\n\tCircles: \"" << circleFilenames[index] << "\"\n\tSquares: \"" << squareFilename << "\"" << std::endl;
//        } else if (rightPressed && index + 1 < circleFilenames.size()) {
//            index++;
//            algorithm.setCircleFile(circleFilenames[index]);
//            squareFilename = algorithm.outputSquares();
//            visualizer.setCircleFile(circleFilenames[index]);
//            visualizer.setSquareFile(squareFilename);
//            std::cout << "\nRendering:\n\tCircles: \"" << circleFilenames[index] << "\"\n\tSquares: \"" << squareFilename << "\"" << std::endl;
//        }
//        
//        leftPressed = false;
//        rightPressed = false;
    }
    
    return 0;
}
