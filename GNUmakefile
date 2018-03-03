
CXX=g++
CXXFLAGS=-g -fPIC

INCDIRS=`root-config --cflags`
INCDIRS += -I`larcv-config --incdir`
CXXFLAGS += $(INCDIRS)

LDFLAGS=`root-config --ldflags --libs`
LDFLAGS += `larcv-config --libs`

all: cell2cv

cell2cv: cell2cv.cxx
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) cell2cv.cxx -o cell2cv $(LDFLAGS)
