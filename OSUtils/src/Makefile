#
# Make file valid only on linux
#

#####################################

# find out in which directory we are
SRCDIR = .
INCLUDEDIR = ../include
LIBDIR = ../lib

# defines the include path
INCLUDES += -I$(INCLUDEDIR)

# define the compiler
CXX = c++ 

# define compilation flags to
CXXFLAGS = -Wall $(INCLUDES)

#define the program to use to make archive library (c++ does it)
MKARLIB = $(AR) r 

# define the program used to build dependencies (again gnu c++ -M(M) -MG)
DEPEND = $(CXX) -M -MG

# define library directories 
LIB_DIR = -L$(LIBDIR)


# define the library to build
BASELIB = $(LIBDIR)/libOSUtils.a

# define sources to be all file with extention .cc
SOURCES           := $(wildcard *.cc)

#define objects 
OBJECTS           := $(SOURCES:.cc=.o)

#####################################

# define our libraries 

# build the option for the loader (essentially the list of libraries)
LDFLAGS =  $(BASELIB)

#________________________________________________________________
#               R U L E S
#________________________________________________________________

# default is build shared library for a particular package
all: $(BASELIB)

# build static libraries out of all objects
$(BASELIB) : $(OBJECTS)
	$(MKARLIB) $@ $(OBJECTS)

# compile a .cc
%.o : %.cc %.d
	$(CXX) $(CXXFLAGS) -c $<  -o $@

#---------------------------------------

# creates dependencies file
%.d : %.cc
	touch $@ 
	$(DEPEND) $(CXXFLAGS) $< > $@

# clean file left by emacs...
rmold:
	@ rm -f *~ ../include/*~

clean: rmold 
	rm -f *.o; rm -f *.d
#	cd $(BINDIR); rm -f $(EXES); cd $(SRCDIR) 
	rm -f $(BASELIB)

#  include dependences

include $(SOURCES:.cc=.d)
