local ffi = require 'ffi'

ffi.cdef[[
	    int get_shdr(char * fname);
]]

ffi.load('elf', true)
ffi.load('bsd', true)
libshdr = ffi.load('./libshdr.so')
libshdr.get_shdr(ffi.new('char[13]', './libshdr.so'))


function load_file(fname) 

   local f = assert(io.open(fname, "rb"))
   local block = 4096
   local mem = {}
   local addr = 0
   while true do
      local bytes = f:read(block)
      if not bytes then break end
      for _, b in pairs{string.byte(bytes, 1, -1)} do
	 --io.write(string.format("%02X ", b))
	 mem[addr] = b
	 addr = addr + 1
      end
      --io.write(string.rep(" ", block - string.len(bytes)))
      --io.write(" ", string.gsub(bytes, "%c", "."), "\n")
   end

   -- for addr=0, #mem do
   --    io.write(string.format("%02X ", mem[addr]))
   --    addr = addr + 1
   --    if addr % 16 == 0 then
   --       io.write("\n")
   --    end
   -- end
   -- io.write("\n")

   return mem
end

