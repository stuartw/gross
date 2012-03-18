#
# Make file valid only on linux
#

#####################################

# find out in which directory we are
TOPDIR = ..
SRCDIR = .
INCLUDEDIR = $(SRCDIR)
LIBDIR = ../lib
BINDIR = ../bin
DBDIR = $(TOPDIR)/SirDB
DBINC = $(DBDIR)/include
DBLIB = $(DBDIR)/lib
OSUDIR = $(TOPDIR)/OSUtils
OSUINC = $(OSUDIR)/include
OSULIB = $(OSUDIR)/lib
CLADDIR = $(TOPDIR)/ClassAdLite
CLADINC = $(CLADDIR)/include
CLADLIB = $(CLADDIR)/lib

# defines the include path
INCLUDES += -DLOGL1 -DLOGL2 -DLOGL3 -I$(INCLUDEDIR) -I$(OSUINC) -I$(CLADINC) -I$(DBINC)

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
# LIB_DIR = -L$(DBLIB) -L$(OSULIB) -L$(CLADLIB) -L$(LIBDIR)

# define the library to build
BASELIB = $(LIBDIR)/libBossBase.a
COMMLIB = $(LIBDIR)/libBossComm.a

# define sources to be all file with extention .cc
# SOURCES           := $(wildcard *.cc)

#define objects 
BASEOBJ := BossConfiguration.o BossJob.o BossJobData.o BossJobElement.o BossUpdateElement.o BossDatabase.o

COMMOBJ := BossKernel.o BossCommandInterpreter.o BossCommandContainer.o BossCommand.o BossScheduler.o BossDeclare.o BossDelete.o BossKill.o BossPurge.o BossQuery.o BossRecoverJob.o BossRegisterJob.o BossRegisterScheduler.o BossSQL.o BossShowJobTypes.o BossShowSchedulers.o BossSubmit.o BossVersion.o BossDeleteJobType.o BossDeleteScheduler.o BossConfigureDB.o

# executables
EXES              := boss jobExecutor dbUpdator

#####################################

# define our libraries 

# build the option for the loader (essentially the list of libraries)
LDFLAGS = $(BASELIB) $(CLADLIB)/libClassAdLite.a $(OSULIB)/libOSUtils.a
# add the appropriate list of libraries for the boss executable
LDFLAGSCLI =  $(LDFLAGS)
CLI_DB_BACKEND = ${CLIENT_DATABASE_BACKEND}
ifeq ($(CLI_DB_BACKEND),clarens)
  CLARENSDIR = $(TOPDIR)/Clarens
  CLARENSLIB = $(CLARENSDIR)/lib
  LDFLAGSCLI += $(DBLIB)/libSirDBClarens.a -L$(TOPDIR)/clarens -lclarens_client -lcrypto -lpthread
else 
  MYSQLDIR = $(TOPDIR)/mysql
  MYSQLLIB = $(MYSQLDIR)/lib
  LDFLAGSCLI += $(DBLIB)/libSirDBMySQL.a $(MYSQLLIB)/libmysqlclient.a $(MYSQLLIB)/libz.a
endif
#  add the appropriate list of libraries for the dbupdator executable
LDFLAGSRT =  $(LDFLAGS)
RT_DB_BACKEND = ${RUNTIME_DATABASE_BACKEND}
ifeq ($(RT_DB_BACKEND),clarens)
  CLARENSDIR = $(TOPDIR)/Clarens
  CLARENSLIB = $(CLARENSDIR)/lib
  LDFLAGSRT += $(DBLIB)/libSirDBClarens.a -L$(TOPDIR)/clarens -lclarens_client -lcrypto -lpthread
else
  MYSQLDIR = $(TOPDIR)/mysql
  MYSQLLIB = $(MYSQLDIR)/lib
  LDFLAGSRT += $(DBLIB)/libSirDBMySQL.a $(MYSQLLIB)/libmysqlclient.a $(MYSQLLIB)/libz.a
endif


#________________________________________________________________
#               R U L E S
#________________________________________________________________

# default is build shared library for a particular package
all: $(EXES)

# build an exe from a .cpp and the archive libraries
boss : boss.cpp boss.d $(BASELIB) $(COMMLIB)
	${LD} -static $(CXXFLAGS) $< -o $(BINDIR)/boss $(COMMLIB) $(LDFLAGSCLI)

jobExecutor : jobExecutor.cpp jobExecutor.d $(BASELIB)
	${LD} -static $(CXXFLAGS) $< -o  $(BINDIR)/jobExecutor $(LDFLAGSRT)

dbUpdator : dbUpdator.cpp dbUpdator.d $(BASELIB) BossRealTimeUpdator.o
	${LD} -static $(CXXFLAGS) $< -o  $(BINDIR)/dbUpdator BossRealTimeUpdator.o $(LDFLAGSRT)

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
%.d : %.cpp
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



