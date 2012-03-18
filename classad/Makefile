
all: config 
	cd src; $(MAKE); $(MAKE) install; cd ..;
config:
	cd src;  ./configure --prefix ..; cd ..;
clean:
	cd src;  $(MAKE) clean; cd ..; rm -rf bin lib include;
