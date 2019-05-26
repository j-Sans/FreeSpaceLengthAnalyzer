//
//  Visualizer.cpp
//  SquareCalculator
//
//  Created by Jake Sanders on 6/30/17.
//  Copyright © 2017 Jake Sanders. All rights reserved.
//

#include "Visualizer.hpp"

Visualizer::Visualizer(bool fromDistributionFile, std::string squareFilename, std::string circleFilename, std::string secondCircleFilename): window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Square Simulator", sf::Style::Default, sf::ContextSettings(0, 0, 0)) { // 0 anti-aliasing because it would interfear with area checking. Lower antialiasing is better
    
    setCircleFile(circleFilename);
    setSquareFile(squareFilename, fromDistributionFile);
    if (secondCircleFilename != "") setCircleFile(secondCircleFilename, true);
}

// Public

void Visualizer::saveImage() {
    window.clear(sf::Color::White);
    
    for (auto circle = circles.begin(); circle != circles.end(); circle++) {
        window.draw(*circle);
    }

    for (auto circle = secondCircles.begin(); circle != secondCircles.end(); circle++) {
        window.draw(*circle);
    }
    
//    window.display();
    
    sf::Texture screen;
    screen.create(WINDOW_SIZE, WINDOW_SIZE);
    screen.update(window);
    
    screenImage = screen.copyToImage();
}

double Visualizer::getAreaFraction() {
    sf::Vector2u size = screenImage.getSize();
    if (size.x <= 0 || size.y <= 0) throw std::logic_error("ERROR: Image not saved");
    int pixels = 0;
    int nonWhitePixels = 0;
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            pixels++;
            
            if (screenImage.getPixel(x, y) != sf::Color::White) {
                nonWhitePixels++;
            }
        }
    }
    return (double)nonWhitePixels / (double)pixels;
}

void Visualizer::setCircleFile(std::string filename, bool secondFile) {
    if (secondFile) secondCircles = getCircles(filename);
    else circles = getCircles(filename);
}

void Visualizer::setSquareFile(std::string filename, bool fromDistributionFile) {
    usingDistributionFile = fromDistributionFile;
    squares = getSquares(filename);
}

void Visualizer::render(bool* leftPressed, bool* rightPressed) {
    sf::Event event;
    while (window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        // Arrow keys pressed: signal to main
        if (event.type == sf::Event::KeyPressed) {
            if (leftPressed && event.key.code == sf::Keyboard::Left) *leftPressed = true;
            else if (rightPressed && event.key.code == sf::Keyboard::Right) *rightPressed = true;
        }
    }
    
    // Clear screen
    window.clear(sf::Color::White);
    
    for (auto circle = circles.begin(); circle != circles.end(); circle++) {
        window.draw(*circle);
    }
    
    for (auto circle = secondCircles.begin(); circle != secondCircles.end(); circle++) {
        window.draw(*circle);
    }

    for (auto square = squares.begin(); square != squares.end(); square++) {
        window.draw(*square);
    }
    
    // Update the window
    window.display();
}

bool Visualizer::isOpen() {
    return window.isOpen();
}

// Private

std::vector<sf::CircleShape> Visualizer::getCircles(std::string filename) {
    std::ifstream input;
    input.open(filename);
    
    char buffer[BUFFER_SIZE];
    
    input.getline(buffer, BUFFER_SIZE);
    std::string str(buffer); // Get each line as a new string
    
    // Extract the number of circles and their radii
    int separationPos = str.find_first_of(SEPARATING_CHARACTER);
    int numCircles = std::stod(str.substr(0, separationPos));
    str = str.substr(separationPos + 1);
    separationPos = str.find_first_of(SEPARATING_CHARACTER);
    double temp = std::stod(str.substr(0, separationPos)); // Unused
    double radius = 0.5;
    str = str.substr(separationPos + 1);
    separationPos = str.find_first_of(SEPARATING_CHARACTER);
    double boxSide = std::stod(str);
    
    double multiplier = WINDOW_SIZE / boxSide;
    radius *= multiplier;
    double avgRadius = radius;

    bool constRadius = true;
    
    //Extract each of the circles' positions and add them to a vector
    std::vector<sf::CircleShape> vec;
    for (int a = 0; a < numCircles; a++) {
        input.getline(buffer, BUFFER_SIZE);
        str = std::string(buffer); // Get each line as a new string
        
        // Extract the number of circles and their radii
        separationPos = str.find_first_of(SEPARATING_CHARACTER);
        double x = multiplier * std::stod(str.substr(0, separationPos));
        str = str.substr(separationPos + 1);
        separationPos = str.find_first_of(SEPARATING_CHARACTER);
        double y = multiplier * std::stod(str.substr(0, separationPos));
        str = str.substr(separationPos + 1);
	separationPos = str.find_first_of(SEPARATING_CHARACTER);
        double transparency = std::stod(str.substr(0, separationPos));
	 
	if (separationPos != std::string::npos) { // If the radius is also present
            str = str.substr(separationPos + 1);
            separationPos = str.find_first_of(SEPARATING_CHARACTER);
	    radius = multiplier * std::stod(str) / 2.0;
	    if (constRadius && (int)(radius / 0.01) != (int)(avgRadius / 0.01)) {
	        constRadius = false;
		std::cout << "Using non-constant radius value" << std::endl;
            }
	}

        sf::CircleShape circle;
        circle.setRadius(radius);
        circle.setOrigin(radius, radius);
        circle.setPosition(x, y);
        circle.setFillColor(sf::Color(0.0, 0.0, 0.0, transparency * (constRadius ? 255.0 : 0.0))); // If non const radii, make circles have no fill and a red border
        if (!constRadius) {
            circle.setOutlineColor(sf::Color::Red);
            circle.setOutlineThickness(4.0);
        }
        vec.push_back(circle);
        
        // Check if circle goes over the edges of the screen. If periodic, draw them on the opposite sides as well
#ifdef PERIODIC
        if (x + radius > WINDOW_SIZE) { // Horizontal periodicity
            sf::CircleShape circleCopy = circle;
            circleCopy.setPosition(x - WINDOW_SIZE, y);
            vec.push_back(circleCopy);
            
            if (y + radius > WINDOW_SIZE) { // Opposite corner periodicity
                circleCopy.setPosition(x - WINDOW_SIZE, y - WINDOW_SIZE);
                vec.push_back(circleCopy);
            } else if (y - radius < 0) {
                circleCopy.setPosition(x - WINDOW_SIZE, y + WINDOW_SIZE);
                vec.push_back(circleCopy);
            }
        } else if (x - radius < 0) {
            sf::CircleShape circleCopy = circle;
            circleCopy.setPosition(x + WINDOW_SIZE, y);
            vec.push_back(circleCopy);
            
            if (y + radius > WINDOW_SIZE) { // Opposite corner periodicity
                circleCopy.setPosition(x + WINDOW_SIZE, y - WINDOW_SIZE);
                vec.push_back(circleCopy);
            } else if (y - radius < 0) {
                circleCopy.setPosition(x + WINDOW_SIZE, y + WINDOW_SIZE);
                vec.push_back(circleCopy);
            }
        }
        if (y + radius > WINDOW_SIZE) { // Vertical periodicity
            sf::CircleShape circleCopy = circle;
            circleCopy.setPosition(x, y - WINDOW_SIZE);
            vec.push_back(circleCopy);
        } else if (y - radius < 0) {
            sf::CircleShape circleCopy = circle;
            circleCopy.setPosition(x, y + WINDOW_SIZE);
            vec.push_back(circleCopy);
        }
#endif
    }
    
    input.close();
    return vec;
}

std::vector<sf::RectangleShape> Visualizer::getSquares(std::string filename) {
    std::ifstream input;
    input.open(filename);
    
    char buffer[BUFFER_SIZE];
    
    input.getline(buffer, BUFFER_SIZE);
    std::string str(buffer); // Get each line as a new string
    
    double boxSide, sideLength, x, y, multiplier;
    double color[3] = {1.0, 0.0, 0.0};
    
    std::vector<sf::RectangleShape> vec;
    
    if (usingDistributionFile) {
        int separationPos = str.find_first_of(SEPARATING_CHARACTER);
        boxSide = std::stod(str.substr(0, separationPos));
        multiplier = WINDOW_SIZE / boxSide;
        str = str.substr(separationPos + 1);
        separationPos = str.find_first_of(SEPARATING_CHARACTER);
        sideLength = multiplier * std::stod(str.substr(0, separationPos));
        
        x = multiplier * boxSide * (double)rand() / (double)RAND_MAX;
        y = multiplier * boxSide * (double)rand() / (double)RAND_MAX;
        x = multiplier * boxSide * (double)rand() / (double)RAND_MAX;
        
        double halfLength = sideLength / 2.0;
        sf::RectangleShape square(sf::Vector2f(sideLength, sideLength));
        square.setOrigin(halfLength, halfLength);
        square.setPosition(x, y);
        square.setOutlineThickness(OUTLINE_THICKNESS);
        square.setOutlineColor(sf::Color(color[0] * 255.0, color[1] * 255.0, color[2] * 255.0));
        square.setFillColor(sf::Color::Transparent);
        
        vec.push_back(square);
        
        // Check if circle goes over the edges of the screen. If periodic, draw them on the opposite sides as well
#ifdef PERIODIC
        if (x + halfLength > WINDOW_SIZE) { // Horizontal periodicity
            sf::RectangleShape squareCopy = square;
            squareCopy.setPosition(x - WINDOW_SIZE, y);
            vec.push_back(squareCopy);
            
            if (y + halfLength > WINDOW_SIZE) { // Opposite corner periodicity
                squareCopy.setPosition(x - WINDOW_SIZE, y - WINDOW_SIZE);
                vec.push_back(squareCopy);
            } else if (y - halfLength < 0) {
                squareCopy.setPosition(x - WINDOW_SIZE, y + WINDOW_SIZE);
                vec.push_back(squareCopy);
            }
            
        } else if (x - halfLength < 0) {
            sf::RectangleShape squareCopy = square;
            squareCopy.setPosition(x + WINDOW_SIZE, y);
            vec.push_back(squareCopy);
            
            if (y + halfLength > WINDOW_SIZE) { // Opposite corner periodicity
                squareCopy.setPosition(x + WINDOW_SIZE, y - WINDOW_SIZE);
                vec.push_back(squareCopy);
            } else if (y - halfLength < 0) {
                squareCopy.setPosition(x + WINDOW_SIZE, y + WINDOW_SIZE);
                vec.push_back(squareCopy);
            }
            
        }
        if (y + halfLength > WINDOW_SIZE) { // Vertical periodicity
            sf::RectangleShape squareCopy = square;
            squareCopy.setPosition(x, y - WINDOW_SIZE);
            vec.push_back(squareCopy);
        } else if (y - halfLength < 0) {
            sf::RectangleShape squareCopy = square;
            squareCopy.setPosition(x, y + WINDOW_SIZE);
            vec.push_back(squareCopy);
        }
#endif
    } else {
    
        // Extract the number of squares
        int separationPos = str.find_first_of(SEPARATING_CHARACTER);
        int numSquares = std::stod(str.substr(0, separationPos));
        str = str.substr(separationPos + 1);
        boxSide = std::stod(str);
        
        multiplier = WINDOW_SIZE / boxSide;
        
        bool dataContainsSquareColor = true;
        
        //Extract the center of each square
        for (int a = 0; a < numSquares; a++) {
            input.getline(buffer, BUFFER_SIZE);
            str = std::string(buffer); // Get each line as a new string
            
            int separationPos = str.find_first_of(SEPARATING_CHARACTER);
            x = multiplier * std::stod(str.substr(0, separationPos));
            str = str.substr(separationPos + 1);
            separationPos = str.find_first_of(SEPARATING_CHARACTER);
            y = multiplier * std::stod(str.substr(0, separationPos));
            str = str.substr(separationPos + 1);
            separationPos = str.find_first_of(SEPARATING_CHARACTER);
            sideLength = multiplier * std::stod(str.substr(0, separationPos));
            
            if (separationPos == std::string::npos) {
                dataContainsSquareColor = false;
            } else {
                str = str.substr(separationPos + 1);
                separationPos = str.find_first_of(SEPARATING_CHARACTER);
            }
            
            // Only extract the color if it is there. Otherwise defaults to black
            if (dataContainsSquareColor) {
                color[0] = std::stod(str.substr(0, separationPos));
                str = str.substr(separationPos + 1);
                separationPos = str.find_first_of(SEPARATING_CHARACTER);
                color[1] = std::stod(str.substr(0, separationPos));
                str = str.substr(separationPos + 1);
                color[2] = std::stod(str);
            }
        
            double halfLength = sideLength / 2.0;
            sf::RectangleShape square(sf::Vector2f(sideLength, sideLength));
            square.setOrigin(halfLength, halfLength);
            square.setPosition(x, y);
            square.setOutlineThickness(OUTLINE_THICKNESS);
            square.setOutlineColor(sf::Color(color[0] * 255.0, color[1] * 255.0, color[2] * 255.0));
            square.setFillColor(sf::Color::Transparent);
            
            vec.push_back(square);
            
            // Check if circle goes over the edges of the screen. If periodic, draw them on the opposite sides as well
#ifdef PERIODIC
            if (x + halfLength > WINDOW_SIZE) { // Horizontal periodicity
                sf::RectangleShape squareCopy = square;
                squareCopy.setPosition(x - WINDOW_SIZE, y);
                vec.push_back(squareCopy);
                
                if (y + halfLength > WINDOW_SIZE) { // Opposite corner periodicity
                    squareCopy.setPosition(x - WINDOW_SIZE, y - WINDOW_SIZE);
                    vec.push_back(squareCopy);
                } else if (y - halfLength < 0) {
                    squareCopy.setPosition(x - WINDOW_SIZE, y + WINDOW_SIZE);
                    vec.push_back(squareCopy);
                }
                
            } else if (x - halfLength < 0) {
                sf::RectangleShape squareCopy = square;
                squareCopy.setPosition(x + WINDOW_SIZE, y);
                vec.push_back(squareCopy);
                
                if (y + halfLength > WINDOW_SIZE) { // Opposite corner periodicity
                    squareCopy.setPosition(x + WINDOW_SIZE, y - WINDOW_SIZE);
                    vec.push_back(squareCopy);
                } else if (y - halfLength < 0) {
                    squareCopy.setPosition(x + WINDOW_SIZE, y + WINDOW_SIZE);
                    vec.push_back(squareCopy);
                }
                
            }
            if (y + halfLength > WINDOW_SIZE) { // Vertical periodicity
                sf::RectangleShape squareCopy = square;
                squareCopy.setPosition(x, y - WINDOW_SIZE);
                vec.push_back(squareCopy);
            } else if (y - halfLength < 0) {
                sf::RectangleShape squareCopy = square;
                squareCopy.setPosition(x, y + WINDOW_SIZE);
                vec.push_back(squareCopy);
            }
#endif
        }
    }
    
    input.close();
    return vec;
}
