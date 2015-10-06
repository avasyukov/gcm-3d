#!/bin/bash
rm -R bin/ lib/ include/ share/
./waf distclean
./waf configure --prefix=. --optimize --logging-level=info --without-tests 
./waf build
./waf install
