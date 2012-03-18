#
# Make file valid only on linux
#

#####################################

# find out in which directory we are
TOPDIR = ../..
SRCDIR = .
INCLUDEDIR = ../include
LIBDIR = ../lib
OSUDIR = ${TOPDIR}/OSUtils
OSUINC = $(OSUDIR)/include
OSULIB = $(OSUDIR)/lib
CLADDIR = ${TOPDIR}/ClassAdLite
CLADINC = $(CLADDIR)/include
CLADLIB = $(CLADDIR)/lib
MYSQLDIR = ${TOPDIR}/mysql
MYSQLINC = $(MYSQLDIR)/include
MYSQLLIB = $(MYSQLDIR)/lib
CLARENSDIR = ${TOPDIR}/Clarens
CLARENSINC = $(CLARENSDIR)/include
CLARENSLIB = $(CLARENSDIR)/lib
# defines the include path
INCLUDES += -I$(INCLUDEDIR) -I$(OSUINC) -I$(CLADINC)

# define the compiler
CXX = c++ 

# define compilation flags to
CXXFLAGS = -Wall $(INCLUDES)

#define the program to use to make archive library (c++ does it)
MKARLIB = $(AR) r 

# define the program used to build dependencies (again gnu c++ -M(M) -MG)
DEPEND = $(CXX) -M -MG

# define library directories 
LIB_DIR = -L$(OSULIB) -L$(CLADLIB) -L$(LIBDIR)

CXXFLMYSQL = $(CXXFLAGS) -I$(MYSQLINC) -DMYSQL_BACKEND
CXXFLCLARENS = $(CXXFLAGS) -I$(CLARENSINC) -DCLARENS_BACKEND

OBJMYSQL = SirDBBackendMySQL.o MySQLBackend.o MySQLBackendContact.o
OBJCLARENS = SirDBBackendClarens.o ClarensBackend.o ClarensBackendContact.o 

LDFLMYSQL = $(LIBDIR)/libSirDBMyDQL.a $(MYSQLLIB)/libmysqlclient.a $(MYSQLLIB)/libz.a
LDFLCLARENS = $(LIBDIR)/libSirDBClarens.a -L$(CLARENSLIB) -lclarens_client -lcrypto -lpthread

#####################################

#________________________________________________________________
#               R U L E S
#________________________________________________________________

# default is build shared library for a particular package
#all: $(LIBDIR)/libSirDBMySQL.a $(LIBDIR)/libSirDBClarens.a
all: $(LIBDIR)/libSirDBMySQL.a

# build static libraries out of all objects
$(LIBDIR)/libSirDBMySQL.a : $(OBJMYSQL)
	$(MKARLIB) $@ $(OBJMYSQL)

$(LIBDIR)/libSirDBClarens.a : $(OBJCLARENS)
	$(MKARLIB) $@ $(OBJCLARENS)

# compile a .cc
SirDBBackendMySQL.o : SirDBBackend.cc SirDBBackendMySQL.d
	$(CXX) $(CXXFLMYSQL) -c SirDBBackend.cc -o $@

MySQLBackend.o : MySQLBackend.cc MySQLBackend.d
	$(CXX) $(CXXFLMYSQL) -c $<  -o $@

MySQLBackendContact.o : MySQLBackendContact.cc MySQLBackendContact.d
	$(CXX) $(CXXFLMYSQL) -c $<  -o $@

SirDBBackendClarens.o : SirDBBackend.cc SirDBBackendClarens.d
	$(CXX) $(CXXFLCLARENS) -c SirDBBackend.cc -o $@

ClarensBackend.o : ClarensBackend.cc ClarensBackend.d
	$(CXX) $(CXXFLCLARENS) -c $<  -o $@

ClarensBackendContact.o : ClarensBackendContact.cc ClarensBackendContact.d
	$(CXX) $(CXXFLCLARENS) -c $<  -o $@

#---------------------------------------

# creates dependencies file
SirDBBackendMySQL.d : SirDBBackend.cc
	touch $@ 
	$(DEPEND) $(CXXFLMYSQL) SirDBBackend.cc > $@

MySQLBackend.d : MySQLBackend.cc
	touch $@ 
	$(DEPEND) $(CXXFLMYSQL) $< > $@

MySQLBackendContact.d : MySQLBackendContact.cc
	touch $@ 
	$(DEPEND) $(CXXFLMYSQL) $< > $@
 
SirDBBackendClarens.d : SirDBBackend.cc
	touch $@ 
	$(DEPEND) $(CXXFLCLARENS) SirDBBackend.cc > $@

ClarensBackend.d : ClarensBackend.cc
	touch $@ 
	$(DEPEND) $(CXXFLCLARENS) $< > $@

ClarensBackendContact.d : ClarensBackendContact.cc
	touch $@ 
	$(DEPEND) $(CXXFLCLARENS) $< > $@

# clean file left by emacs...
rmold:	
	@ rm -f *~ $(INCLUDEDIR)/*~

clean: rmold 
	rm -f *.o; rm -f *.d
#	cd $(BINDIR); rm -f $(EXES); cd $(SRCDIR) 
	rm -f $(LIBDIR)/libSirDBMySQL.a $(LIBDIR)/libSirDBClarens.a

#  include dependences

include $(SOURCES:.cc=.d)



