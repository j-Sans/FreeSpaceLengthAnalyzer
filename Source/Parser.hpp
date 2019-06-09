//
//  Parser.hpp
//  Parser
//
//  Created by Jake Sanders on 6/28/17.
//

#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>

#include "Point.hpp"

enum direction_t {
    x,
    y,
    z,
};

struct Slice: public std::vector<Point> {
    direction_t direction;
    int frame;
};

/* Parses movie.xyz by cutting a number of frames into slices of particles which are converted to 2D and saved as output files
 * A number of frames are ignored before enough have passed that particles are in random locations and no longer ordered
 * Then, a number of frames are cut into slices along the xy, xz, and yz planes
 */
class Parser {
public:
    Parser(std::string filename, int framesToIgnore, int framesToRecord, int* depthPerSlice);
    
    /*
     * @return The names of each of the files outputted
     */
    std::vector<std::string> outputCircles();
    
    int getNumParticles();
    double getBoxSize();
    double getVolumeFraction();
    
private:
    int numParticles;
    double boxSize, diameter, volumeFraction;
    std::vector<Slice> slices;
    int firstUsedFrame = 0;
    
    std::vector<Slice> getSlices(const std::vector<Point>& particles, int* thickness, double height);
    
    std::string outputSingleSlice(const Slice& slice);
    std::string outputSingleSlice(const Slice& slice, std::string filename);
};
