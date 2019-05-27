#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Point.hpp"

#define PI 3.14159265359

enum PackingType {
    SC,
    BCC,
    FCC,
    NoCrystal
};

class CrystalSimulator {
public:
    CrystalSimulator(int numParticles, double volumeConcentration, PackingType packing);
    
    std::string printLattice(std::string filename = "");
    double getSideLength();
    
private:
    double sideLength;
    double spaceBetweenLattices;
    double volumeConcentration;
    int numParticles;
    int latticeSitesPerRow;
    PackingType packing;
    
    std::vector<Point> particles;
    
    void buildLattice();
    void makeLatticeSite(double xStart, double yStart);
};
