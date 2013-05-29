local ffi = require 'ffi'

ffi.cdef[[
	    int prog2(char* fname);
typedef struct { uint8_t red, green, blue, alpha; } rgba_pixel;
rgba_pixel* init(int n);
]]

ffi.load('elf', true)
ffi.load('bsd', true)
local libprog2=ffi.load('./libprog2.so')

local libtst = ffi.load('./libtst.so')
local img = libtst.init(4)
print(ffi.typeof(img))
for i=0, 5 do
   print(img[i].red)
end

--libprog2.prog2(ffi.new('char[13]', './libprog2.so'))


