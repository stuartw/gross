CPP = g++
CPPFLAGS = -g -ansi
OFLAG = -o
.SUFFIXES : .o .cpp
.cpp.o :
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<
.cc.o :
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

#BOSS stuff (including Classad, MySQL)
TOPDIR=${BOSSDIR}
BINDIR=$(TOPDIR)/bin
CLADINC=$(TOPDIR)/classad/include
BOSSINC=$(TOPDIR)/src
MYSQLINC=$(TOPDIR)/mysql/include
CLADLIB=$(TOPDIR)/classad/lib
BOSSLIB=$(TOPDIR)/lib
MYSQLLIB=$(TOPDIR)/mysql/lib
PSTREAMINC=$(TOPDIR)/pstream

#RLS stuff - how do we avoid hard coding here??
SINC=/opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/rh73_gcc32/include/
BOOSTINC=/opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/
POOLINC=/opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/
SEALBASEINC =/opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/
POOLLIB=/opt/cms/lcg/app/releases/POOL/POOL_1_6_1/rh73_gcc32/lib
PLUGINLIB=/opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/rh73_gcc32/lib/

EXES = gross
SOURCES = $(wildcard *.cc)
OBJS := $(SOURCES:.cc=.o)
INCLUDES += -I$(CLADINC) -I$(BOSSINC) -I$(MYSQLINC) -I$(POOLINC) -I$(BOOSTINC) -I$(SEALBASEINC) -I$(SINC) -I$(PSTREAMINC)
LIB_DIRS = -L$(CLADLIB) -L$(BOSSLIB) -L$(MYSQLLIB) -L$(POOLLIB) -L$(PLUGINLIB)
LIB_FILES +=  -lBossComm -lBossBase  -lclassad -lmysqlclient -lz -llcg_FileCatalog -llcg_AttributeList

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

gross.o: CommandInterpreter.hh Log.hh Command.hh CommandContainer.hh
gross.o: FileSys.hh /usr/include/sys/stat.h /usr/include/features.h
gross.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
gross.o: /usr/include/bits/types.h
gross.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
gross.o: /usr/include/bits/stat.h
BossIf.o: BossIf.hh Log.hh QInfoTask.hh Error.hh Job.hh Task.hh JDL.hh
BossIf.o: LocalDb.hh /home/tallini/boss-v3_3_9/mysql/include/mysql.h
BossIf.o: /usr/include/sys/types.h /usr/include/features.h
BossIf.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
BossIf.o: /usr/include/bits/types.h
BossIf.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
BossIf.o: /usr/include/time.h
BossIf.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
BossIf.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
BossIf.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h
BossIf.o: /home/tallini/boss-v3_3_9/src/BossCommandInterpreter.h
BossIf.o: /home/tallini/boss-v3_3_9/src/BossCommandContainer.h
CladLookup.o: CladLookup.hh Log.hh
CladLookup.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/common.h
CladLookup.o: /usr/include/stdio.h /usr/include/features.h
CladLookup.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
CladLookup.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
CladLookup.o: /usr/include/bits/types.h /usr/include/libio.h
CladLookup.o: /usr/include/_G_config.h /usr/include/wchar.h
CladLookup.o: /usr/include/bits/wchar.h /usr/include/gconv.h
CladLookup.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
CladLookup.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
CladLookup.o: /usr/include/time.h /usr/include/math.h
CladLookup.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
CladLookup.o: /usr/include/bits/mathcalls.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
CladLookup.o: /usr/include/errno.h /usr/include/bits/errno.h
CladLookup.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
CladLookup.o: /usr/include/ctype.h /usr/include/endian.h
CladLookup.o: /usr/include/bits/endian.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/value.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/util.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/source.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/view.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
CladLookup.o: /home/tallini/boss-v3_3_9/classad/include/query.h
Command.o: Command.hh Log.hh LocalDb.hh
Command.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
Command.o: /usr/include/sys/types.h /usr/include/features.h
Command.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Command.o: /usr/include/bits/types.h
Command.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Command.o: /usr/include/time.h
Command.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
Command.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
Command.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h DbSpec.hh
Command.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/common.h
Command.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
Command.o: /usr/include/wchar.h /usr/include/bits/wchar.h
Command.o: /usr/include/gconv.h
Command.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
Command.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
Command.o: /usr/include/math.h /usr/include/bits/huge_val.h
Command.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
Command.o: /usr/include/errno.h /usr/include/bits/errno.h
Command.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Command.o: /usr/include/ctype.h /usr/include/endian.h
Command.o: /usr/include/bits/endian.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/value.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/util.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/source.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/view.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
Command.o: /home/tallini/boss-v3_3_9/classad/include/query.h File.hh
CommandContainer.o: CommandContainer.hh Log.hh Prepare.hh Command.hh
CommandContainer.o: Submit.hh Register.hh Query.hh Output.hh Delete.hh
CommandInterpreter.o: CommandInterpreter.hh Log.hh Command.hh
CommandInterpreter.o: CommandContainer.hh
DbJob.o: DbJob.hh Log.hh Job.hh
DbJob.o: /home/tallini/boss-v3_3_9/src/BossCommandInterpreter.h
DbJob.o: /home/tallini/boss-v3_3_9/src/BossCommandContainer.h Wrapper.hh
DbJob.o: JDL.hh LocalDb.hh /home/tallini/boss-v3_3_9/mysql/include/mysql.h
DbJob.o: /usr/include/sys/types.h /usr/include/features.h
DbJob.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
DbJob.o: /usr/include/bits/types.h
DbJob.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
DbJob.o: /usr/include/time.h
DbJob.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
DbJob.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
DbJob.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h FileSys.hh
DbJob.o: /usr/include/sys/stat.h /usr/include/bits/stat.h Task.hh File.hh
DbJob.o: CladLookup.hh /home/tallini/boss-v3_3_9/src/BossClassAd.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/common.h
DbJob.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
DbJob.o: /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h
DbJob.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
DbJob.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
DbJob.o: /usr/include/math.h /usr/include/bits/huge_val.h
DbJob.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
DbJob.o: /usr/include/errno.h /usr/include/bits/errno.h
DbJob.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
DbJob.o: /usr/include/ctype.h /usr/include/endian.h
DbJob.o: /usr/include/bits/endian.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/value.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/util.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/source.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/view.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
DbJob.o: /home/tallini/boss-v3_3_9/classad/include/query.h
DbSpec.o: DbSpec.hh Log.hh /home/tallini/boss-v3_3_9/src/BossClassAd.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/common.h
DbSpec.o: /usr/include/stdio.h /usr/include/features.h
DbSpec.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
DbSpec.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
DbSpec.o: /usr/include/bits/types.h /usr/include/libio.h
DbSpec.o: /usr/include/_G_config.h /usr/include/wchar.h
DbSpec.o: /usr/include/bits/wchar.h /usr/include/gconv.h
DbSpec.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
DbSpec.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
DbSpec.o: /usr/include/time.h /usr/include/math.h
DbSpec.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
DbSpec.o: /usr/include/bits/mathcalls.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
DbSpec.o: /usr/include/errno.h /usr/include/bits/errno.h
DbSpec.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
DbSpec.o: /usr/include/ctype.h /usr/include/endian.h
DbSpec.o: /usr/include/bits/endian.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/value.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/util.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/source.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/view.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
DbSpec.o: /home/tallini/boss-v3_3_9/classad/include/query.h
DbTask.o: DbTask.hh Log.hh Task.hh BossIf.hh QInfoTask.hh TaskFactory.hh
DbTask.o: Job.hh CladLookup.hh /home/tallini/boss-v3_3_9/src/BossClassAd.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/common.h
DbTask.o: /usr/include/stdio.h /usr/include/features.h
DbTask.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
DbTask.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
DbTask.o: /usr/include/bits/types.h /usr/include/libio.h
DbTask.o: /usr/include/_G_config.h /usr/include/wchar.h
DbTask.o: /usr/include/bits/wchar.h /usr/include/gconv.h
DbTask.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
DbTask.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
DbTask.o: /usr/include/time.h /usr/include/math.h
DbTask.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
DbTask.o: /usr/include/bits/mathcalls.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
DbTask.o: /usr/include/errno.h /usr/include/bits/errno.h
DbTask.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
DbTask.o: /usr/include/ctype.h /usr/include/endian.h
DbTask.o: /usr/include/bits/endian.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/value.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/util.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/source.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/view.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
DbTask.o: /home/tallini/boss-v3_3_9/classad/include/query.h File.hh
DbTask.o: LocalDb.hh /home/tallini/boss-v3_3_9/mysql/include/mysql.h
DbTask.o: /usr/include/sys/types.h
DbTask.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
DbTask.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
DbTask.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h
Delete.o: Delete.hh Log.hh Command.hh LocalDb.hh
Delete.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
Delete.o: /usr/include/sys/types.h /usr/include/features.h
Delete.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Delete.o: /usr/include/bits/types.h
Delete.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Delete.o: /usr/include/time.h
Delete.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
Delete.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
Delete.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h
EDGIf.o: EDGIf.hh Log.hh /home/tallini/boss-v3_3_9/pstream/pstream.h
EDGIf.o: /usr/include/sys/types.h /usr/include/features.h
EDGIf.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
EDGIf.o: /usr/include/bits/types.h
EDGIf.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
EDGIf.o: /usr/include/time.h /usr/include/sys/wait.h /usr/include/signal.h
EDGIf.o: /usr/include/bits/sigset.h /usr/include/bits/signum.h
EDGIf.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
EDGIf.o: /usr/include/bits/time.h /usr/include/bits/waitflags.h
EDGIf.o: /usr/include/bits/waitstatus.h /usr/include/unistd.h
EDGIf.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
EDGIf.o: StringSpecial.hh
Error.o: Error.hh
File.o: File.hh Log.hh /usr/include/sys/stat.h /usr/include/features.h
File.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
File.o: /usr/include/bits/types.h
File.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
File.o: /usr/include/bits/stat.h
FileSys.o: FileSys.hh Log.hh /usr/include/sys/stat.h /usr/include/features.h
FileSys.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
FileSys.o: /usr/include/bits/types.h
FileSys.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
FileSys.o: /usr/include/bits/stat.h
JDL.o: JDL.hh Log.hh CladLookup.hh
JDL.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/common.h
JDL.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
JDL.o: /usr/include/gnu/stubs.h
JDL.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
JDL.o: /usr/include/bits/types.h /usr/include/libio.h
JDL.o: /usr/include/_G_config.h /usr/include/wchar.h
JDL.o: /usr/include/bits/wchar.h /usr/include/gconv.h
JDL.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
JDL.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
JDL.o: /usr/include/time.h /usr/include/math.h /usr/include/bits/huge_val.h
JDL.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/debug.h /usr/include/errno.h
JDL.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
JDL.o: /usr/include/asm/errno.h /usr/include/ctype.h /usr/include/endian.h
JDL.o: /usr/include/bits/endian.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/value.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/util.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/source.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/view.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
JDL.o: /home/tallini/boss-v3_3_9/classad/include/query.h Job.hh File.hh
Job.o: Job.hh Log.hh File.hh Task.hh Wrapper.hh JDL.hh LocalDb.hh
Job.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
Job.o: /usr/include/sys/types.h /usr/include/features.h
Job.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Job.o: /usr/include/bits/types.h
Job.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Job.o: /usr/include/time.h
Job.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
Job.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
Job.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h DeletePtr.hh
LocalDb.o: LocalDb.hh Log.hh /home/tallini/boss-v3_3_9/mysql/include/mysql.h
LocalDb.o: /usr/include/sys/types.h /usr/include/features.h
LocalDb.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
LocalDb.o: /usr/include/bits/types.h
LocalDb.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
LocalDb.o: /usr/include/time.h
LocalDb.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
LocalDb.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
LocalDb.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h DbSpec.hh
LocalDb.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/common.h
LocalDb.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
LocalDb.o: /usr/include/wchar.h /usr/include/bits/wchar.h
LocalDb.o: /usr/include/gconv.h
LocalDb.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
LocalDb.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
LocalDb.o: /usr/include/math.h /usr/include/bits/huge_val.h
LocalDb.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
LocalDb.o: /usr/include/errno.h /usr/include/bits/errno.h
LocalDb.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
LocalDb.o: /usr/include/ctype.h /usr/include/endian.h
LocalDb.o: /usr/include/bits/endian.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/value.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/util.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/source.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/view.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
LocalDb.o: /home/tallini/boss-v3_3_9/classad/include/query.h
Log.o: Log.hh
MySQLDb.o: MySQLDb.hh Log.hh /home/tallini/boss-v3_3_9/mysql/include/mysql.h
MySQLDb.o: /usr/include/sys/types.h /usr/include/features.h
MySQLDb.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
MySQLDb.o: /usr/include/bits/types.h
MySQLDb.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
MySQLDb.o: /usr/include/time.h
MySQLDb.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
MySQLDb.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
MySQLDb.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h DbSpec.hh
MySQLDb.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/common.h
MySQLDb.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
MySQLDb.o: /usr/include/wchar.h /usr/include/bits/wchar.h
MySQLDb.o: /usr/include/gconv.h
MySQLDb.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
MySQLDb.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
MySQLDb.o: /usr/include/math.h /usr/include/bits/huge_val.h
MySQLDb.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
MySQLDb.o: /usr/include/errno.h /usr/include/bits/errno.h
MySQLDb.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
MySQLDb.o: /usr/include/ctype.h /usr/include/endian.h
MySQLDb.o: /usr/include/bits/endian.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/value.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/util.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/source.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/view.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
MySQLDb.o: /home/tallini/boss-v3_3_9/classad/include/query.h
NewJob.o: NewJob.hh Log.hh Job.hh
NewJob.o: /home/tallini/boss-v3_3_9/src/BossCommandInterpreter.h
NewJob.o: /home/tallini/boss-v3_3_9/src/BossCommandContainer.h Task.hh
NewJob.o: Wrapper.hh JDL.hh LocalDb.hh
NewJob.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
NewJob.o: /usr/include/sys/types.h /usr/include/features.h
NewJob.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
NewJob.o: /usr/include/bits/types.h
NewJob.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
NewJob.o: /usr/include/time.h
NewJob.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
NewJob.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
NewJob.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h FileSys.hh
NewJob.o: /usr/include/sys/stat.h /usr/include/bits/stat.h File.hh
NewJob.o: CladLookup.hh /home/tallini/boss-v3_3_9/src/BossClassAd.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/common.h
NewJob.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
NewJob.o: /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h
NewJob.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
NewJob.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
NewJob.o: /usr/include/math.h /usr/include/bits/huge_val.h
NewJob.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
NewJob.o: /usr/include/errno.h /usr/include/bits/errno.h
NewJob.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
NewJob.o: /usr/include/ctype.h /usr/include/endian.h
NewJob.o: /usr/include/bits/endian.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/value.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/util.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/source.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/view.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
NewJob.o: /home/tallini/boss-v3_3_9/classad/include/query.h PhysCat.hh
NewTask.o: NewTask.hh Log.hh Task.hh TaskFactory.hh PhysCat.hh CladLookup.hh
NewTask.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/common.h
NewTask.o: /usr/include/stdio.h /usr/include/features.h
NewTask.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
NewTask.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
NewTask.o: /usr/include/bits/types.h /usr/include/libio.h
NewTask.o: /usr/include/_G_config.h /usr/include/wchar.h
NewTask.o: /usr/include/bits/wchar.h /usr/include/gconv.h
NewTask.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
NewTask.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
NewTask.o: /usr/include/time.h /usr/include/math.h
NewTask.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
NewTask.o: /usr/include/bits/mathcalls.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
NewTask.o: /usr/include/errno.h /usr/include/bits/errno.h
NewTask.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
NewTask.o: /usr/include/ctype.h /usr/include/endian.h
NewTask.o: /usr/include/bits/endian.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/value.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/util.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/source.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/view.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
NewTask.o: /home/tallini/boss-v3_3_9/classad/include/query.h Job.hh File.hh
Output.o: Output.hh Log.hh Command.hh TaskOutput.hh Task.hh FileSys.hh
Output.o: /usr/include/sys/stat.h /usr/include/features.h
Output.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Output.o: /usr/include/bits/types.h
Output.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Output.o: /usr/include/bits/stat.h Range.hh
PhysCat.o: PhysCat.hh Log.hh RLSquery.hh
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFileCatalog.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCSystemTools.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCEntry.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeList.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/Attribute.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeTypes.h
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/shared_ptr.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/user.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/select_compiler_config.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/compiler/gcc.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/select_stdlib_config.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/select_platform_config.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/suffix.hpp
PhysCat.o: /usr/include/limits.h /usr/include/features.h
PhysCat.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
PhysCat.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/detail/shared_ptr_nmt.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/assert.hpp
PhysCat.o: /usr/include/assert.h
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/checked_delete.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/throw_exception.hpp
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config.hpp
PhysCat.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/exception
PhysCat.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/detail/atomic_count.hpp
PhysCat.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new
PhysCat.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/POOLCore/Token.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/POOLCore/Guid.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/Attribute.inl
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeExceptions.h
PhysCat.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealKernel/Exception.h
PhysCat.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealBase/Error.h
PhysCat.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealBase/config.h
PhysCat.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealPlatform/system.h
PhysCat.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/rh73_gcc32/include/SealPlatform/config.h
PhysCat.o: /usr/include/sys/types.h /usr/include/bits/types.h
PhysCat.o: /usr/include/time.h
PhysCat.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealKernel/Status.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeSpecification.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeSpecification.inl
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeListSpecification.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/URIParser.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCException.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFCAction.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFCContainer.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCBuf.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCLeaf.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCImpl.h
PhysCat.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFCContainer.inl
PhysCat.o: File.hh FileSys.hh /usr/include/sys/stat.h
PhysCat.o: /usr/include/bits/stat.h
Prepare.o: Prepare.hh Log.hh Command.hh TaskFactory.hh Task.hh FileSys.hh
Prepare.o: /usr/include/sys/stat.h /usr/include/features.h
Prepare.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Prepare.o: /usr/include/bits/types.h
Prepare.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Prepare.o: /usr/include/bits/stat.h File.hh
QInfoTask.o: QInfoTask.hh Log.hh Task.hh Job.hh TaskFactory.hh File.hh
QInfoTask.o: BossIf.hh CladLookup.hh
QInfoTask.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/common.h
QInfoTask.o: /usr/include/stdio.h /usr/include/features.h
QInfoTask.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
QInfoTask.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
QInfoTask.o: /usr/include/bits/types.h /usr/include/libio.h
QInfoTask.o: /usr/include/_G_config.h /usr/include/wchar.h
QInfoTask.o: /usr/include/bits/wchar.h /usr/include/gconv.h
QInfoTask.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
QInfoTask.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
QInfoTask.o: /usr/include/time.h /usr/include/math.h
QInfoTask.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
QInfoTask.o: /usr/include/bits/mathcalls.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
QInfoTask.o: /usr/include/errno.h /usr/include/bits/errno.h
QInfoTask.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
QInfoTask.o: /usr/include/ctype.h /usr/include/endian.h
QInfoTask.o: /usr/include/bits/endian.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/value.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/util.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/source.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/view.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
QInfoTask.o: /home/tallini/boss-v3_3_9/classad/include/query.h Range.hh
Query.o: Query.hh Log.hh Command.hh QInfoTask.hh Task.hh LocalDb.hh
Query.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
Query.o: /usr/include/sys/types.h /usr/include/features.h
Query.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Query.o: /usr/include/bits/types.h
Query.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Query.o: /usr/include/time.h
Query.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
Query.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
Query.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h Range.hh
RLSquery.o: RLSquery.hh
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFileCatalog.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCSystemTools.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCEntry.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeList.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/Attribute.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeTypes.h
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/shared_ptr.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/user.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/select_compiler_config.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/compiler/gcc.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/select_stdlib_config.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/select_platform_config.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config/suffix.hpp
RLSquery.o: /usr/include/limits.h /usr/include/features.h
RLSquery.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
RLSquery.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/detail/shared_ptr_nmt.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/assert.hpp
RLSquery.o: /usr/include/assert.h
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/checked_delete.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/throw_exception.hpp
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/config.hpp
RLSquery.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/exception
RLSquery.o: /opt/cms/lcg/external/boost/1.30.2/rh73_gcc32/boost/detail/atomic_count.hpp
RLSquery.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new
RLSquery.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/POOLCore/Token.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/POOLCore/Guid.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/Attribute.inl
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeExceptions.h
RLSquery.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealKernel/Exception.h
RLSquery.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealBase/Error.h
RLSquery.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealBase/config.h
RLSquery.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealPlatform/system.h
RLSquery.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/rh73_gcc32/include/SealPlatform/config.h
RLSquery.o: /usr/include/sys/types.h /usr/include/bits/types.h
RLSquery.o: /usr/include/time.h
RLSquery.o: /opt/cms/lcg/app/releases/SEAL/SEAL_1_3_3/include/SealKernel/Status.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeSpecification.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeSpecification.inl
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/AttributeList/AttributeListSpecification.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/URIParser.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCException.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFCAction.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFCContainer.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCBuf.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCLeaf.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/FCImpl.h
RLSquery.o: /opt/cms/lcg/app/releases/POOL/POOL_1_6_1/include/FileCatalog/IFCContainer.inl
RLSquery.o: File.hh Log.hh
Range.o: Range.hh Log.hh Job.hh
Register.o: Register.hh Log.hh Command.hh File.hh DbSpec.hh
Register.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/common.h
Register.o: /usr/include/stdio.h /usr/include/features.h
Register.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Register.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Register.o: /usr/include/bits/types.h /usr/include/libio.h
Register.o: /usr/include/_G_config.h /usr/include/wchar.h
Register.o: /usr/include/bits/wchar.h /usr/include/gconv.h
Register.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
Register.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
Register.o: /usr/include/time.h /usr/include/math.h
Register.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Register.o: /usr/include/bits/mathcalls.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
Register.o: /usr/include/errno.h /usr/include/bits/errno.h
Register.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Register.o: /usr/include/ctype.h /usr/include/endian.h
Register.o: /usr/include/bits/endian.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/value.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/util.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/source.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/view.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
Register.o: /home/tallini/boss-v3_3_9/classad/include/query.h LocalDb.hh
Register.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
Register.o: /usr/include/sys/types.h
Register.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
Register.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
Register.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h
StandardTFactory.o: StandardTFactory.hh Log.hh TaskFactory.hh NewTask.hh
StandardTFactory.o: Task.hh NewJob.hh Job.hh DbTask.hh DbJob.hh
StringSpecial.o: StringSpecial.hh
Submit.o: Submit.hh Log.hh Command.hh TaskFactory.hh BossIf.hh QInfoTask.hh
Submit.o: Task.hh FileSys.hh /usr/include/sys/stat.h /usr/include/features.h
Submit.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Submit.o: /usr/include/bits/types.h
Submit.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Submit.o: /usr/include/bits/stat.h
Task.o: Task.hh Log.hh CladLookup.hh
Task.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/common.h
Task.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
Task.o: /usr/include/gnu/stubs.h
Task.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Task.o: /usr/include/bits/types.h /usr/include/libio.h
Task.o: /usr/include/_G_config.h /usr/include/wchar.h
Task.o: /usr/include/bits/wchar.h /usr/include/gconv.h
Task.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
Task.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
Task.o: /usr/include/time.h /usr/include/math.h /usr/include/bits/huge_val.h
Task.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
Task.o: /usr/include/errno.h /usr/include/bits/errno.h
Task.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Task.o: /usr/include/ctype.h /usr/include/endian.h /usr/include/bits/endian.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/value.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/util.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/source.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/view.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
Task.o: /home/tallini/boss-v3_3_9/classad/include/query.h PhysCat.hh Job.hh
Task.o: BossIf.hh QInfoTask.hh File.hh LocalDb.hh
Task.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
Task.o: /usr/include/sys/types.h
Task.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
Task.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
Task.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h DeletePtr.hh
TaskFactory.o: TaskFactory.hh Log.hh StandardTFactory.hh
TaskOutput.o: TaskOutput.hh Log.hh Task.hh Job.hh TaskFactory.hh File.hh
TaskOutput.o: BossIf.hh QInfoTask.hh EDGIf.hh Error.hh LocalDb.hh
TaskOutput.o: /home/tallini/boss-v3_3_9/mysql/include/mysql.h
TaskOutput.o: /usr/include/sys/types.h /usr/include/features.h
TaskOutput.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TaskOutput.o: /usr/include/bits/types.h
TaskOutput.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
TaskOutput.o: /usr/include/time.h
TaskOutput.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
TaskOutput.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
TaskOutput.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h Range.hh
Wrapper.o: Wrapper.hh Log.hh CladLookup.hh
Wrapper.o: /home/tallini/boss-v3_3_9/src/BossClassAd.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/classad_distribution.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/common.h
Wrapper.o: /usr/include/stdio.h /usr/include/features.h
Wrapper.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Wrapper.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
Wrapper.o: /usr/include/bits/types.h /usr/include/libio.h
Wrapper.o: /usr/include/_G_config.h /usr/include/wchar.h
Wrapper.o: /usr/include/bits/wchar.h /usr/include/gconv.h
Wrapper.o: /usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
Wrapper.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
Wrapper.o: /usr/include/time.h /usr/include/math.h
Wrapper.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Wrapper.o: /usr/include/bits/mathcalls.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/debug.h
Wrapper.o: /usr/include/errno.h /usr/include/bits/errno.h
Wrapper.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Wrapper.o: /usr/include/ctype.h /usr/include/endian.h
Wrapper.o: /usr/include/bits/endian.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/classadErrno.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/classad.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/classad_stl.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/exprTree.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/value.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/util.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/literals.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/attrrefs.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/operators.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/exprList.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/fnCall.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/classad_features.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/classadItor.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/source.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/lexer.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/lexerSource.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/sink.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/xmlSource.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/xmlLexer.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/xmlSink.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/matchClassad.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/collection.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/view.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/collectionBase.h
Wrapper.o: /home/tallini/boss-v3_3_9/classad/include/query.h Job.hh
Wrapper.o: LocalDb.hh /home/tallini/boss-v3_3_9/mysql/include/mysql.h
Wrapper.o: /usr/include/sys/types.h
Wrapper.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_com.h
Wrapper.o: /home/tallini/boss-v3_3_9/mysql/include/mysql_version.h
Wrapper.o: /home/tallini/boss-v3_3_9/mysql/include/my_alloc.h File.hh
