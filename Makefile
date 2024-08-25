CXX      = clang++
CXXFLAGS = -O2 -Wall -Wextra -Wpedantic -Wshadow
LDFLAGS  = -g3 

# Target executable
sim: main.o NFLSim.o Game.o Team.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Object files
main.o: main.cpp NFLSim.h
	$(CXX) $(CXXFLAGS) -c main.cpp

NFLSim.o: NFLSim.cpp NFLSim.h Game.h Team.h
	$(CXX) $(CXXFLAGS) -c NFLSim.cpp

Team.o: Team.cpp Team.h
	$(CXX) $(CXXFLAGS) -c Team.cpp

Game.o: Game.cpp Game.h Team.h
	$(CXX) $(CXXFLAGS) -c Game.cpp

# Clean rule
clean:
	@rm -f *.o sim
