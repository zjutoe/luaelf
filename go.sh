#!/bin/sh

#gcc -fPIC -c libprog2.c
#gcc -shared -o libprog2.so libprog2.o -lelf -lbsd
#gcc -fPIC -c libtst.c
#gcc -shared -o libtst.so libtst.o -lelf -lbsd
gcc -fPIC -c libelfconn.c
gcc -shared -o libelfconn.so libelfconn.o -lelf -lbsd

#luajit test.lua
