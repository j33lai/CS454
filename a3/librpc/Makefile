CXX = g++-5 -std=c++14
CXXFLAGS = -g -Wall -MMD
SRCDIR = ./src
BUILDDIR = ./build
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRC))
DEPENDS = ${OBJECTS:.o=.d}
LIB = ./lib/librpc.a

${LIB} : ${OBJECTS}
	ar rcs ${LIB} ${OBJECTS}
$(OBJECTS) : $(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
-include ${DEPENDS}

.PHONY: clean
clean:
	rm  -f $(OBJECTS) $(DEPENDS) $(LIB)

