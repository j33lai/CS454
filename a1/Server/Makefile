CXX = g++-5 -std=c++14
CXXFLAGS = -g -Wall -MMD
OBJECTS = main.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = stringServer

${EXEC} : ${OBJECTS}
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

-include ${DEPENDS}

.PHONY: clean
clean:
	rm  -f $(OBJECTS) $(DEPENDS) $(EXEC)
