local ffi = require 'ffi'


ffi.cdef[[
      typedef struct _shdr_t {
	uint32_t sh_idx;
	uint32_t sh_name; /* Section name (index into the section header
			     string table). */
	uint32_t sh_type;
	uint64_t sh_flags;
	uint64_t sh_addr;
	uint64_t sh_offset;
	uint64_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint64_t sh_addralign;
	uint64_t sh_entsize;
      } shdr_t;

      int shdr_num(char* fname);
      shdr_t* get_shdr(char* fname, int num);
]]

--ffi.include 'libshdr.h'


ffi.load('elf', true)
ffi.load('bsd', true)
libshdr = ffi.load('./libshdr.so')
local shdr_num = libshdr.shdr_num(ffi.new('char[13]', './libshdr.so'))
print('shdr_num: ', shdr_num)
local shdrs = libshdr.get_shdr(ffi.new('char[13]', './libshdr.so'), shdr_num)

function ld_scn(fname, shdr)
   local f = assert(io.open(fname, "rb"))

   local offset = tonumber(shdr.sh_offset)   
   f:seek('set', offset)

   local buf = {}
   while true do
      -- local bytes = f:read(tonumber(shdr.sh_size))      
      local bytes = f:read(2048)
      if not bytes then break end
      
      for _, b in pairs{string.byte(bytes, 1, -1)} do
	 buf[#buf + 1] = b
      end
   end

   -- local buf = {}
   -- --if not bytes then break end
   --  for _, b in pairs{string.byte(bytes, 1, -1)} do
   --     buf[#buf + 1] = b
   --  end
   -- --end   

   local mem = {}
   mem.buf = buf
   local addr = tonumber(shdr.sh_addr)
   mem.addr = addr

   return mem
end


-- so here we've got the section headers
for i=0, shdr_num do
   local h = shdrs[i]
   local m = ld_scn('./libshdr.so', h)
   print(string.format("Section %2d addr %4X offset %4X size %4X", 
		       tonumber(h.sh_idx), 
		       tonumber(h.sh_addr), 
		       tonumber(h.sh_offset),
		       tonumber(h.sh_size)
		    ))

   local buf = m.buf
   io.write(string.format("\n%06X: ", m.addr))
   if buf then
      for i, v in ipairs(buf) do
	 if (m.addr + i - 1) % 16 == 0 then
	    io.write(string.format("\n%06X: ", m.addr+i-1))
	 end
	 io.write(string.format("%02X ", v))
      end
      io.write("\n")
   end
end




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
   --io.write("\n")

   return mem
end

