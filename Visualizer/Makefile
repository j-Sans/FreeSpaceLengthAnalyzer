CXX = g++
CXXLINKER = g++
CXXFLAGS = -std=c++11

OBJS = main.o Visualizer.o
EXECS = Visualizer

PATH = /usr/local/

$(EXECS): $(OBJS)
	$(CXXLINKER) -o $(EXECS) $(OBJS) -L$(PATH)lib -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp Visualizer.hpp
	$(CXX) -c -I$(PATH)include main.cpp
Visualizer.o: Visualizer.cpp Visualizer.hpp
	$(CXX) -c -I$(PATH)include Visualizer.cpp

.PHONY: clean copy

clean:
	-rm $(EXECS) $(OBJS)

copy:
	cp ./$(EXECS) ../Run/
