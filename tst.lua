local ffi = require 'ffi'

ffi.cdef[[
      int init(int);
      int set_buf(int, int);
      int get_buf(int);
      int fini();
]]

-- ffi.load('elf', true)
-- ffi.load('bsd', true)
-- local libprog2=ffi.load('./libprog2.so')

local libtst = ffi.load('./libtst.so')
libtst.init(10)
-- print(ffi.typeof(img))
for i=0, 10 do
   -- print(img[i].red)
   libtst.set_buf(i, i*2)
end

for i=0, 10 do
   -- print(img[i].red)
   print(libtst.get_buf(i))
end

libtst.init(5)

print('-----')
for i=0, 10 do
   -- print(img[i].red)
   print(libtst.get_buf(i))
end


--libprog2.prog2(ffi.new('char[13]', './libprog2.so'))


