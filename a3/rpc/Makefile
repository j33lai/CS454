CXX = g++-5 -std=c++14
CXXFLAGS = -g -Wall -MMD
ARCHIVE = ar rcs

SRCDIR = ./src
LIBRPCSRCDIR = $(SRCDIR)/librpc
BINDERSRCDIR = $(SRCDIR)/binder

BUILDDIR = ./build
LIBRPCBUILDDIR = $(BUILDDIR)/librpc
BINDERBUILDDIR = $(BUILDDIR)/binder

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRC))
DEPENDS = ${OBJECTS:.o=.d}

LIBRPCSRC = $(wildcard $(LIBRPCSRCDIR)/*.cpp)
LIBRPCOBJECTS = $(patsubst $(LIBRPCSRCDIR)/%.cpp, $(LIBRPCBUILDDIR)/%.o, $(LIBRPCSRC))
LIBRPCDEPENDS = ${LIBRPCOBJECTS:.o=.d}

BINDERSRC = $(wildcard $(BINDERSRCDIR)/*.cpp)
BINDEROBJECTS = $(patsubst $(BINDERSRCDIR)/%.cpp, $(BINDERBUILDDIR)/%.o, $(BINDERSRC))
BINDERDEPENDS = ${BINDEROBJECTS:.o=.d}


LIB = ./librpc.a
BINDER = ./binder

MKDIRP = mkdir -p

ifeq (,$(wildcard $(LIBRPCBUILDDIR)))
$(shell $(MKDIRP) $(LIBRPCBUILDDIR))
endif

ifeq (,$(wildcard $(BINDERBUILDDIR)))
$(shell $(MKDIRP) $(BINDERBUILDDIR))
endif

all: $(LIB) $(BINDER)

$(LIB) : $(OBJECTS) $(LIBRPCOBJECTS)
	$(ARCHIVE) $(LIB) $(OBJECTS) $(LIBRPCOBJECTS)
$(BINDER) : $(OBJECTS) $(BINDEROBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(BINDEROBJECTS) -o $(BINDER)
$(OBJECTS) : $(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(LIBRPCOBJECTS) : $(LIBRPCBUILDDIR)/%.o : $(LIBRPCSRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BINDEROBJECTS) : $(BINDERBUILDDIR)/%.o : $(BINDERSRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
-include $(DEPENDS) $(LIBRPCDEPENDS) $(BINDERDEPENDS))

.PHONY: clean
clean:
	rm  -rf $(BUILDDIR) $(LIB) $(BINDER)

