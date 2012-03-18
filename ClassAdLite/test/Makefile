#
# Make file valid only on linux
#

#####################################

# find out in which directory we are
SRCDIR = .
INCLUDEDIR = ../include
LIBDIR = ../lib
BINDIR = ../bin

# defines the include path
INCLUDES += -I. -I$(INCLUDEDIR)

# define the compiler
CXX = c++ 
AR = ar

# define compilation flags to
CXXFLAGS = -Wall $(INCLUDES)

#define the program to use to make archive library (c++ does it)
MKARLIB = $(AR) r 

# define the program used to build dependencies (again gnu c++ -M(M) -MG)
DEPEND = $(CXX) -M -MG

# define the loader, and the loading flags
LD = $(CXX)

# define library directories 
LIB_DIR = -L$(LIBDIR)


# define the library to build
BASELIB = $(LIBDIR)/libClassAdLite.a

# define sources to be all file with extention .cc
SOURCES           := $(wildcard *.cc)

#define objects 
#define objects to be all fies with the same anme of sources but extention .o
OBJECTS           := $(SOURCES:.cc=.o)

# executables
EXESRC            := $(wildcard *.cpp)
EXES              := $(EXESRC:.cpp=.exe)
EXEFILES          := $(EXESRC:.cpp=)

#####################################

# define our libraries 

# build the option for the loader (essentially the list of libraries)
LDFLAGS =  $(BASELIB)

#________________________________________________________________
#               R U L E S
#________________________________________________________________

# default is build shared library for a particular package
all: $(EXES)

# build an exe from a .cpp and the archive libraries
%.exe : %.cpp $(BASELIB)
	${LD} -static $(CXXFLAGS) $< -o  $(BINDIR)/$* $(OBJECTS) $(LDFLAGS) 


#---------------------------------------

# creates dependencies file
%.d : %.cc
	touch $@ 
	$(DEPEND) $(CXXFLAGS) $< > $@

# clean file left by emacs...
rmold:
	@ rm -f *~

clean: rmold 
	rm -f *.o; rm -f *.d
	cd $(BINDIR); rm -f $(EXEFILES); cd $(SRCDIR) 

#  include dependences

include $(SOURCES:.cc=.d)



