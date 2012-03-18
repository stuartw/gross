CPP = g++
CPPFLAGS = -g -ansi -O2
#CPPFLAGS = -ansi -O2 
OFLAG = -o
.SUFFIXES : .o .cpp
.cpp.o :
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<
.cc.o :
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

#BOSS stuff (including Classad, MySQL)
TOPDIR=${BOSSDIR}
CMSDIR=${CMS_PATH}
BINDIR=$(GROSSDIR)/bin
CLADINC=$(TOPDIR)/ClassAdLite/include
BOSSINC=$(TOPDIR)/src
MYSQLINC=$(TOPDIR)/mysql/include
CLADLIB=$(TOPDIR)/ClassAdLite/lib
BOSSLIB=$(TOPDIR)/lib
MYSQLLIB=$(TOPDIR)/mysql/lib
#GROSSINC=${GROSSDIR}/include
OSUTILSINC=$(TOPDIR)/OSUtils/include
OSUTILSLIB=$(TOPDIR)/OSUtils/lib
DBINC=$(TOPDIR)/SirDB/include
DBLIB=$(TOPDIR)/SirDB/lib
PYTHONINC=/stage/exp_software/cms/lcg/external/python/2.3.4/rh73_gcc32/include/python2.3 #needs to be compilled with gcc3.2+

EXES = gross
SOURCES = $(wildcard *.cc)
OBJS := $(SOURCES:.cc=.o)
INCLUDES += -I$(CLADINC) -I$(BOSSINC) -I$(MYSQLINC) -I$(OSUTILSINC) -I$(DBINC)
LIB_DIRS = -L$(CLADLIB) -L$(BOSSLIB) -L$(MYSQLLIB) -L$(OSUTILSLIB) -L$(DBLIB)
LIB_FILES +=  -lBossComm -lBossBase  -lClassAdLite -lmysqlclient -lz -lcurl -lOSUtils -lSirDBMySQL

all: $(EXES) 

gross: gross.o $(OBJS)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(OFLAG) $(BINDIR)/gross gross.o $(OBJS) $(LIB_DIRS) $(LIB_FILES)

gross: gross.cpp $(SOURCES) 

test: test.o $(OBJS)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(OFLAG) test test.o $(OBJS) $(LIB_DIRS) $(LIB_FILES)

test: test.cpp $(SOURCES)

# create python bindings for API - needs swig installed
python:
	swig -python -c++ CommandInterpreter.i
	$(CPP) -c CommandInterpreter_wrap.cxx -I$(PYTHONINC)
	$(CPP) -shared gross.o $(OBJS) $(LIB_DIRS) $(LIB_FILES) CommandInterpreter_wrap.o -o _gross.so

clean:
	rm -f *.o *.so *.py *.pyc CommandInterpreter_wrap.cxx $(EXES)
depend: 
	makedepend -- $(CPP) $(CPPFLAGS) -- $(INCLUDES) gross.cpp $(SOURCES) $(LIB_DIRS) $(LIB_FILES)

# DO NOT DELETE

