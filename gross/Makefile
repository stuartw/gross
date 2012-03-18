CPP = g++
CPPFLAGS = -g -ansi
#CPPFLAGS = -ansi
OFLAG = -o
.SUFFIXES : .o .cpp
.cpp.o :
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<
.cc.o :
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

#BOSS stuff (including Classad, MySQL)
TOPDIR=${BOSSDIR}
CMSDIR=${CMS_PATH}
BINDIR=$(TOPDIR)/bin
CLADINC=$(TOPDIR)/ClassAdLite/include
BOSSINC=$(TOPDIR)/src
MYSQLINC=$(TOPDIR)/mysql/include
CLADLIB=$(TOPDIR)/ClassAdLite/lib
BOSSLIB=$(TOPDIR)/lib
MYSQLLIB=$(TOPDIR)/mysql/lib
#PSTREAMINC=$(TOPDIR)/pstream
OSUTILSINC=$(TOPDIR)/OSUtils/include
OSUTILSLIB=$(TOPDIR)/OSUtils/lib
DBINC=$(TOPDIR)/SirDB/include
DBLIB=$(TOPDIR)/SirDB/lib


#RLS stuff
SINC=$(CMSDIR)/lcg/app/releases/SEAL/SEAL_1_3_3/rh73_gcc32/include/
BOOSTINC=$(CMSDIR)/lcg/external/boost/1.30.2/rh73_gcc32/
POOLINC=$(CMSDIR)/lcg/app/releases/POOL/POOL_1_6_1/include/
SEALBASEINC =$(CMSDIR)/lcg/app/releases/SEAL/SEAL_1_3_3/include/
POOLLIB=$(CMSDIR)/lcg/app/releases/POOL/POOL_1_6_1/rh73_gcc32/lib
PLUGINLIB=$(CMSDIR)/lcg/app/releases/SEAL/SEAL_1_3_3/rh73_gcc32/lib/


EXES = gross
SOURCES = $(wildcard *.cc)
OBJS := $(SOURCES:.cc=.o)
INCLUDES += -I$(CLADINC) -I$(BOSSINC) -I$(MYSQLINC) -I$(POOLINC) -I$(BOOSTINC) -I$(SEALBASEINC) -I$(SINC) -I$(OSUTILSINC)
LIB_DIRS = -L$(CLADLIB) -L$(BOSSLIB) -L$(MYSQLLIB) -L$(POOLLIB) -L$(PLUGINLIB) -L$(OSUTILSLIB) -L$(DBLIB)
LIB_FILES +=  -lBossComm -lBossBase  -lClassAdLite -lmysqlclient -lz -llcg_FileCatalog -llcg_AttributeList -lcurl -lOSUtils -lSirDBMySQL

all: $(EXES)

gross: gross.o $(OBJS)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(OFLAG) $(BINDIR)/gross gross.o $(OBJS) $(LIB_DIRS) $(LIB_FILES)

gross: gross.cpp $(SOURCES) 

test: test.o $(OBJS)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(OFLAG) test test.o $(OBJS) $(LIB_DIRS) $(LIB_FILES)

test: test.cpp $(SOURCES)


clean:
	rm -f *.o $(EXES)
depend: 
	makedepend -- $(CPP) $(CPPFLAGS) -- $(INCLUDES) gross.cpp $(SOURCES) $(LIB_DIRS) $(LIB_FILES)

# DO NOT DELETE
