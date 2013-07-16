#!/bin/bash
rm -R bin/ lib/ include/ share/
./waf distclean
./waf configure --prefix=. --cxxflags=-O3 --without-headers --without-resources
./waf install
