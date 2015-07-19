#!/bin/bash
rm -R bin/ lib/ include/ share/
./waf distclean
./waf configure --prefix=. --optimize --without-tests --logging-level=info --disable-assertions
./waf build
./waf install
