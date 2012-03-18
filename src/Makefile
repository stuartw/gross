#
# Make file valid only on linux
#

#####################################

# find out in which directory we are
TOPDIR = ${BOSSDIR}
SRCDIR = $(TOPDIR)/src
LIBDIR = $(TOPDIR)/lib
TMPDIR = $(TOPDIR)/tmp
BINDIR = $(TOPDIR)/bin
MYSQLINC = $(TOPDIR)/mysql/include
MYSQLLIB = $(TOPDIR)/mysql/lib
CLADINC = $(TOPDIR)/ClassAdLite/include
CLADLIB = $(TOPDIR)/ClassAdLite/lib
PSTREAMINC = $(TOPDIR)/pstream

# defines the include path
INCLUDES += -DLOGL1 -DLOGL2 -DLOGL3 -I$(SRCDIR) $(CC_INCLUDES) -I$(MYSQLINC) -I$(CLADINC) -I$(PSTREAMINC)
#INCLUDES += -DLOGL1 -I$(SRCDIR) $(CC_INCLUDES) -I$(MYSQLINC) -I$(CLADINC) -I$(PSTREAMINC)

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
LIB_DIR = -L$(MYSQLLIB) -L$(CLADLIB) -L$(LIBDIR)


# define the library to build
BASELIB = $(LIBDIR)/libBossBase.a
COMMLIB = $(LIBDIR)/libBossComm.a

# define sources to be all file with extention .cc
SOURCES           := $(wildcard *.cc)

#define objects 
BASEOBJ := BossOperatingSystem.o BossConfiguration.o BossJob.o BossJobData.o BossJobElement.o BossDatabase.o BossUpdateElement.o

COMMOBJ := BossKernel.o BossCommandInterpreter.o BossCommandContainer.o BossCommand.o BossScheduler.o BossDeclare.o BossDelete.o BossKill.o BossPurge.o BossQuery.o BossRecoverJob.o BossRegisterJob.o BossRegisterScheduler.o BossSQL.o BossShowJobTypes.o BossShowSchedulers.o BossSubmit.o BossVersion.o BossDeleteJobType.o BossDeleteScheduler.o BossConfigureDB.o

# executables
EXES              := boss jobExecutor dbUpdator

#####################################

# define our libraries 

# build the option for the loader (essentially the list of libraries)
LDFLAGSA =  $(BASELIB) $(MYSQLLIB)/libmysqlclient.a $(MYSQLLIB)/libz.a $(CLADLIB)/libClassAdLite.a

#________________________________________________________________
#               R U L E S
#________________________________________________________________

# default is build shared library for a particular package
all: $(EXES)

# build an exe from a .cpp and the archive libraries
boss : boss.cpp $(BASELIB) $(COMMLIB)
	${LD} -static $(CXXFLAGS) $< -o  $(BINDIR)/boss $(COMMLIB) $(LDFLAGSA) 

jobExecutor : jobExecutor.cpp $(BASELIB) 
	${LD} -static $(CXXFLAGS) $< -o  $(BINDIR)/jobExecutor  $(LDFLAGSA) 

dbUpdator : dbUpdator.cpp $(BASELIB) BossRealTimeMySQLUpdator.o
	${LD} -static $(CXXFLAGS) $< -o  $(BINDIR)/dbUpdator BossRealTimeMySQLUpdator.o $(LDFLAGSA)

# build static libraries out of all objects
$(BASELIB) : $(BASEOBJ)
	$(MKARLIB) $@ $(BASEOBJ)

$(COMMLIB) : $(COMMOBJ) 
	$(MKARLIB) $@ $(COMMOBJ)

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
	@ rm -f *~

clean: rmold 
	rm -f *.o; rm -f *.d
	cd $(BINDIR); rm -f $(EXES); cd $(SRCDIR) 
	rm -f $(BASELIB) $(COMMLIB)

#  include dependences

include $(SOURCES:.cc=.d)



