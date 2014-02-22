# Makefile wrapper for waf

all:
	./waf

configure:
	./waf configure --prefix=. --cxxflags=-O3 --without-headers --without-resources	

build:
	./waf build

install:
	./waf install

clean:
	./waf clean

distclean:
	./waf distclean
