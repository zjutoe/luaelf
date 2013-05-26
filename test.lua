local ffi = require 'ffi'

ffi.cdef[[
	    int prog2(char* fname);
]]

ffi.load('elf', true)
ffi.load('bsd', true)
local libprog2=ffi.load('./libprog2.so')

libprog2.prog2(ffi.new('char[13]', './libprog2.so'))


