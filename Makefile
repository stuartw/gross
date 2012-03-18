
all: database

config:
	@ ./CONFIGURE

compile:
	@ . grossenv.sh; $(MAKE) -k -C src

database: compile
	@. grossenv.sh;  gross configureDB

clean:
	$(MAKE) -C src clean
	@ rm -f grossenv.sh grossenv.csh bin/*
	@ echo "You need to manually delete the GROSS database in MySQL"

