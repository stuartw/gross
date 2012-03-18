
all: database

config:
	@ ./CONFIGURE

compile:
	@ cd OSUtils; $(MAKE); cd ..;
	@ cd ClassAdLite; $(MAKE); cd ..;
	@ cd SirDB; $(MAKE); cd ..;
	@ . bossenv.sh; cd src; $(MAKE) -k; cd ..;

database: compile
	@. bossenv.sh;  boss configureDB

gross: database
	@ . bossenv.sh; cd gross; $(MAKE); cd ..;
	@ gross configureDB


clean:
	@ cd OSUtils; $(MAKE) clean; cd ..;
	@ cd ClassAdLite; $(MAKE) clean; cd ..;
	@ cd SirDB; $(MAKE) clean; cd ..;
	@ cd src; $(MAKE) clean ; cd ..;
	@ cd jobtest; rm -rf *.log out* err* Boss*; cd ..;
	@ rm -f BossConfig.clad MySQLconfig.sql MySQLGridConfig.sql bossenv.sh bossenv.csh
	@ echo "You need to manually delete the BOSS database in MySQL"

