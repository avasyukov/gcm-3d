#!/bin/bash
rm -R bin/ lib/ include/ share/
./waf distclean
./waf configure --prefix=. --optimize --static --logging-level=info --disable-assertions --without-tests 
./waf build
./waf install
