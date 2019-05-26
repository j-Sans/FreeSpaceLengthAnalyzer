//
//  Visualizer.hpp
//  SquareCalculator
//
//  Created by Jake Sanders on 6/30/17.
//  Copyright © 2017 Jake Sanders. All rights reserved.
//

#ifndef Visualizer_hpp
#define Visualizer_hpp

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

#define PERIODIC
#define BUFFER_SIZE 256

#define OUTLINE_THICKNESS 2
#define WINDOW_SIZE 1500
#define SEPARATING_CHARACTER ' ' // Which character separates the data in the .txt files

class Visualizer {
public:
    Visualizer(std::string circleFilename = "", std::string squareFilename = "");
    
    void saveImage();
    double getAreaFraction(); // Returns the area of particles among the total area. Must be called after saveImage()
    
    void setCircleFile(std::string filename);
    void setSquareFile(std::string filename);
    void render(bool* leftPressed = nullptr, bool* rightPressed = nullptr);
    bool isOpen();
    
private:
    sf::RenderWindow window;
    std::vector<sf::CircleShape> circles;
    std::vector<sf::RectangleShape> squares;
    sf::Image screenImage;
    
    std::vector<sf::CircleShape> getCircles(std::string filename);
    std::vector<sf::RectangleShape> getSquares(std::string filename);
};

#endif /* Visualizer_hpp */
