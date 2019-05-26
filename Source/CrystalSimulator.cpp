#include "CrystalSimulator.hpp"

// Constructor

CrystalSimulator::CrystalSimulator(int numParticles, double volumeConcentration, PackingType packing) {
    this->volumeConcentration = volumeConcentration;
    this->packing = packing;

    if (packing == NoCrystal) {
        throw std::logic_error("CrystalSimulator initialied with packing of NoCrystal");
    }
    
    double temp = pow(numParticles / (packing == FCC ? 4.0 : packing == BCC ? 2.0 : 1.0), 1.0 / 3.0);
    latticeSitesPerRow = (int)(temp + 0.001); // For an unknown reason temp became 6 but when it was cast to an int, became 5. Adding a small decimal makes it large enough that casting does not decrement it.
 
    sideLength = 0.5 * pow(4.0 / 3.0 * (double)numParticles * PI / (double)volumeConcentration, 1.0 / 3.0);
    
    spaceBetweenLattices = sideLength / (double)latticeSitesPerRow;
    std::cout << "Initialized CrystalSimulator with values:\n\tVolume concentration: " << volumeConcentration << "\n\tLattice sites per row: " << latticeSitesPerRow << "\n\tBox side length: " << sideLength << "\n\tSpace between lattice sites: " << spaceBetweenLattices << std::endl;
}

// Public

std::string CrystalSimulator::printLattice(std::string filename) {
    std::cout << "Building lattice" << std::endl;
    buildLattice();
    std::cout << "Printing lattice" << std::endl;
    std::ofstream file;

    filename = filename != "" ? filename : packing == FCC ? "FCC.crcl" : packing == BCC ? "BCC.crcl" : "SC.crcl";
    file.open(filename);
    std::string str = std::to_string(numParticles) + " 1.0 " + std::to_string(sideLength) + " " + std::to_string(volumeConcentration) + "\n";
    file.write(str.c_str(), str.length());
    
    for (Point particle : particles) {
        str = std::to_string(particle.x) + " " + std::to_string(particle.y) + " " + std::to_string(particle.depth) + "\n";
        file.write(str.c_str(), str.length());
    }
    
    return filename;
}

// Private

void CrystalSimulator::buildLattice() {
    for (int x = 0; x < latticeSitesPerRow; x++) {
        for (int y = 0; y < latticeSitesPerRow; y++) {
            makeLatticeSite(x * spaceBetweenLattices, y * spaceBetweenLattices);
        }
    }
}

void CrystalSimulator::makeLatticeSite(double xStart, double yStart) {
    //For SC, BCC, FCC
    Point corner(xStart, yStart);
    corner.depth = 1.0;
    particles.push_back(corner);
    numParticles++;   
 
    if (packing == BCC || packing == FCC) {
        Point middle(xStart + (spaceBetweenLattices / 2.0), yStart + (spaceBetweenLattices / 2.0));
        middle.depth = 0.5;
        particles.push_back(middle);
        numParticles++;
    }
    
    if (packing == FCC) {
        Point topSide(xStart + (spaceBetweenLattices / 2.0), yStart);
        topSide.depth = 1.0;
        Point leftSide(xStart, yStart + (spaceBetweenLattices / 2.0));
        leftSide.depth = 1.0;
        particles.push_back(topSide);
        particles.push_back(leftSide);
        numParticles += 2;
    }
}
