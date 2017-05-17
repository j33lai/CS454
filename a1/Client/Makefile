CXX = g++-5 -std=c++14
CXXFLAGS = -g -Wall -MMD -pthread
OBJECTS = main.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = stringClient

${EXEC} : ${OBJECTS}
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

-include ${DEPENDS}

.PHONY: clean
clean:
	rm  -f $(OBJECTS) $(DEPENDS) $(EXEC)
