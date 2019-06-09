//
//  Parser.cpp
//  SquareCalculator
//
//  Created by Jake Sanders on 6/29/17.
//

#include "Parser.hpp"

// Constructor

Parser::Parser(std::string filename, int framesToIgnore, int framesToRecord, int* depthPerSlice) {
    std::ifstream file;
    file.open(filename);
    
    const double pi = 3.14159265358979323846264338327950;
    
    // Extract frame information from the first frame, assuming the information stays constant (number of particles and box size)
    char buffer[256];
    file.getline(buffer, 256);
    numParticles = std::stoi(buffer);
    std::cout << "Number of particles: " << numParticles << std::endl;
    file.get(buffer, 256, ' ');
    boxSize = std::stod(buffer);
    std::cout << "Box side length: " << boxSize << std::endl << "Box volume: " << (boxSize * boxSize * boxSize) << std::endl;
    file.get(buffer, 256);
    double temperature = std::stod(buffer);
    //if (temperature > 0.5) {
        diameter = 1.0;
    //} else {
    //    diameter = pow(2.0, 1.0 / 6.0);
    //}
    volumeFraction = (numParticles * 4.0 / 3.0 * pi * (diameter / 2.0) * (diameter / 2.0) * (diameter / 2.0) / boxSize / boxSize / boxSize);
    std::cout << "Diameter: " << diameter << std::endl << "Volume fraction: " << volumeFraction << std::endl << std::endl;
    
    // Ignore the first few frames before equilibrium has been established
    firstUsedFrame = framesToIgnore;
    int linesToIgnore = framesToIgnore * (2 + numParticles);
    auto max = std::numeric_limits<std::streamsize>::max();
    for (int a = 0; a < linesToIgnore; a++) file.ignore(max, '\n');
    
    if (!file.good()) {
        if (file.eof()) std::cout << "ERROR: Parser::Parser() eof flag set" << std::endl;
        if (file.fail()) std::cout << "ERROR: Parser::Parser() fail flag set" << std::endl;
        if (file.bad()) std::cout << "ERROR: Parser::Parser() bad flag set" << std::endl;
    }
    
    for (int frame = 0; frame < framesToRecord && file.good(); frame++) {
        
        std::vector<Point> particles(numParticles);
        
        for (int particle = 0; particle < numParticles; particle++) {
            file.ignore(256, '\t'); // Ignore characters before the first coordinate
            file.get(buffer, 256, '\t');
            particles[particle].x = std::stod(buffer);
            file.ignore(256, '\t'); // Ignore spacing
            file.get(buffer, 256, '\t');
            particles[particle].y = std::stod(buffer);
            file.ignore(256, '\t'); // Ignore spacing
            file.get(buffer, 256, '\t'); // Skips until the next tab, ignoring the two lines of frame information
            particles[particle].z = std::stod(buffer);
        }
        
        std::vector<Slice> newSlices = getSlices(particles, depthPerSlice, boxSize);
        
        for (Slice slice : newSlices) {
            slices.push_back(slice);
        }
    }
    
    file.close();
}

// Public

std::vector<std::string> Parser::outputCircles() {
    std::vector<std::string> names;
    for (int frame = 0; frame < slices.size(); frame++) {
        names.push_back(outputSingleSlice(slices[frame]));
    }
    return names;
}

int Parser::getNumParticles() {
    return numParticles;
}

double Parser::getBoxSize() {
    return boxSize;
}

double Parser::getVolumeFraction() {
    return volumeFraction;
}

// Private

std::vector<Slice> Parser::getSlices(const std::vector<Point>& particles, int* thickness, double height) {
    if (*thickness > height) *thickness = height;
    std::vector<Slice> slicesX((int)(height / *thickness));
    for (int frame = 0; frame < slicesX.size(); frame++) {
        slicesX[frame].direction = x;
        slicesX[frame].frame = frame;
    }
    for (int particle = 0; particle < particles.size(); particle++) { // yz dimension slices
        if (particles[particle].x >= slicesX.size() * *thickness) continue;
        slicesX[(int)(particles[particle].x / *thickness)].push_back(particles[particle]);
        double sliceBack = (int)(particles[particle].x / *thickness) * (*thickness);
        double sliceFront = sliceBack + *thickness;
        slicesX[(int)(particles[particle].x / *thickness)].back().depth = (sliceFront - particles[particle].x) / (sliceFront - sliceBack);
    }
    
    std::vector<Slice> slicesY(height / *thickness);
    for (int frame = 0; frame < slicesY.size(); frame++) {
        slicesY[frame].direction = y;
        slicesY[frame].frame = frame;
    }
    for (int particle = 0; particle < particles.size(); particle++) { // xz dimension slices
        if (particles[particle].y >= slicesY.size() * *thickness) continue;
        slicesY[(int)(particles[particle].y / *thickness)].push_back(particles[particle]);
        double sliceBack = (int)(particles[particle].y / *thickness) * (*thickness);
        double sliceFront = sliceBack + *thickness;
        slicesY[(int)(particles[particle].y / *thickness)].back().depth = (sliceFront - particles[particle].y) / (sliceFront - sliceBack);
    }
    
    std::vector<Slice> slicesZ(height / *thickness);
    for (int frame = 0; frame < slicesZ.size(); frame++) {
        slicesZ[frame].direction = z;
        slicesZ[frame].frame = frame;
    }
    for (int particle = 0; particle < particles.size(); particle++) { // xy dimension slices
        if (particles[particle].z >= slicesZ.size() * *thickness) continue;
        slicesZ[(int)(particles[particle].z / *thickness)].push_back(particles[particle]);
        double sliceBack = (int)(particles[particle].z / *thickness) * (*thickness);
        double sliceFront = sliceBack + *thickness;
        slicesZ[(int)(particles[particle].z / *thickness)].back().depth = (sliceFront - particles[particle].z) / (sliceFront - sliceBack);
    }
    
    std::vector<Slice> allSlices;
    for (int x = 0; x < slicesX.size(); x++) allSlices.push_back(slicesX[x]);
    for (int y = 0; y < slicesY.size(); y++) allSlices.push_back(slicesY[y]);
    for (int z = 0; z < slicesZ.size(); z++) allSlices.push_back(slicesZ[z]);
    
    return allSlices;
}

/* Output file name:
 *  "project_[frame]_[direction].xy"
 *      with [frame] being the frame number and [direction] being 0-2, with 0 representing using the x coordinates, 1 for y, and 2 for z
 *  "project_21_1.xy (example)
 *
 * The file format contains the number of particles in the slice followed by the 2D coordinates in the respective plane along with a third coordinate
 * The third coordinate is from 0 to 1 and represents the depth of the particle within the plane, with 1 being closest and 0 being furthest
 * These three coordinates are separated by a space
 */
std::string Parser::outputSingleSlice(const Slice& slice) {
    return outputSingleSlice(slice, "project_" + std::to_string(slice.frame) + "_" + std::to_string(slice.direction) + ".crcl");
}

std::string Parser::outputSingleSlice(const Slice& slice, std::string filename) {
    std::ofstream file;
    file.open(filename);
    
    std::string str = std::to_string(slice.size()) + " " + std::to_string(diameter) + " " + std::to_string(boxSize) + " " + std::to_string(volumeFraction) + "\n"; // Write the number of particles in the slice
    file.write(str.c_str(), str.length());
    
    for (auto particle = slice.begin(); particle != slice.end(); particle++) {
        if (slice.direction == x) {
            /*bool newPoint = true;
            for (auto particleToCompare = slice.begin(); particleToCompare != slice.end(); particleToCompare++) {
                if (particle->y == particleToCompare->y && particle->z == particleToCompare->z && particle != particleToCompare) {
                    newPoint = false;
                    break;
                }
            }
            if (newPoint)*/ str = std::to_string(particle->y) + " " + std::to_string(particle->z) + " " + std::to_string(particle->depth) + "\n";
        } else if (slice.direction == y) {
            /*bool newPoint = true;
            for (auto particleToCompare = slice.begin(); particleToCompare != slice.end(); particleToCompare++) {
                if (particle->x == particleToCompare->x && particle->z == particleToCompare->z && particle != particleToCompare) {
                    newPoint = false;
                    break;
                }
            }
            if (newPoint)*/ str = std::to_string(particle->x) + " " + std::to_string(particle->z) + " " + std::to_string(particle->depth) + "\n";
        } else if (slice.direction == z) {
            /*bool newPoint = true;
            for (auto particleToCompare = slice.begin(); particleToCompare != slice.end(); particleToCompare++) {
                if (particle->x == particleToCompare->x && particle->y == particleToCompare->y && particle != particleToCompare) {
                    newPoint = false;
                    break;
                }
            }
            if (newPoint)*/ str = std::to_string(particle->x) + " " + std::to_string(particle->y) + " " + std::to_string(particle->depth) + "\n";
        }
        file.write(str.c_str(), str.length());
    }
    
    file.close();
    
    return filename;
}
