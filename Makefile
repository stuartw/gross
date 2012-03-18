
all: database

config:
	@ ./CONFIGURE

compile:
	@ cd classad; $(MAKE); cd ..;
	@ . bossenv.sh; cd src; $(MAKE) -k; cd ..;

database: compile
	@. bossenv.sh;  boss configureDB

gross: database
	@ . bossenv.sh; cd gross; $(MAKE); cd ..;

clean:
	@ cd classad; $(MAKE) clean; cd ..;
	@ cd src; $(MAKE) clean ; cd ..;
	@ cd jobtest; rm -rf *.log out* err* Boss*; cd ..;
	@ cd gross; $(MAKE) clean; cd ..;
	@ rm -f BossConfig.clad MySQLconfig.sql MySQLGridConfig.sql bossenv.sh bossenv.csh
	@ echo "You need to manually delete the BOSS database in MySQL"

