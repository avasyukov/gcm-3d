#!/bin/bash
rm -R bin/ lib/ include/ share/
./waf distclean
./waf configure --prefix=. --optimize
./waf build
./waf install
