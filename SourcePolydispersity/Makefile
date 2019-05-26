PATH = /usr/local/

CXX = g++
CXXLINKER = g++
CXXFLAGS = -std=c++11 -I$(PATH)include

OBJS = main.o Point.o CrystalSimulator.o Parser.o SquareHeuristic.o Visualizer.o
EXECS = SquareCalculator

$(EXECS): $(OBJS)
	$(CXXLINKER) -o $(EXECS) $(OBJS) -L$(PATH)lib -lsfml-graphics -lsfml-window -lsfml-system

main.o: CrystalSimulator.hpp Parser.hpp SquareHeuristic.hpp Visualizer.hpp
Point.o: Point.hpp
CrystalSimulator.o: CrystalSimulator.hpp Point.hpp
Parser.o: Parser.hpp Point.hpp
SquareHeuristic.o: SquareHeuristic.hpp Point.hpp
Visualizer.o: Visualizer.hpp

.PHONY: clean copy

clean:
	-rm $(EXECS) $(OBJS)

copy:
	cp ./$(EXECS) ../Run/
