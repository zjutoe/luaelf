local ffi = require 'ffi'


ffi.cdef[[

typedef struct {
	uint32_t sh_idx;
	uint32_t sh_name;
	uint32_t sh_type;
	uint64_t sh_flags;
	uint64_t sh_addr;
	uint64_t sh_offset;
	uint64_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint64_t sh_addralign;
	uint64_t sh_entsize;
	uint8_t* data;
        char* name;
} scn_hdr_t;

      int init(char* fname);
      void fini();
      int get_scn_num();
      size_t get_scn_size(int idx);
      scn_hdr_t* get_scn_data(int idx);
]]


ffi.load('elf', true)
ffi.load('bsd', true)
libshdr = ffi.load('./libshdr.so')

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


function loadelf()

   local fname = ffi.new(string.format("char[%d]", string.len(arg[1])+1), arg[1])

   libshdr.init(fname)
   local n = tonumber(libshdr.get_scn_num())
   for idx=0, n do      
      local scn_hdr = libshdr.get_scn_data(idx)
      print(string.format('section %d %s size 0x%x', 
			  idx,
			  tostring(scn_hdr.name), 
			  tonumber(scn_hdr.sh_size)))
      local sz = tonumber(scn_hdr.sh_size)
      for i=0, sz-1 do 
      	 if i % 16 == 0 then
      	    io.write("\n")
      	 end
      	 io.write(string.format("%02x ", tonumber(scn_hdr.data[i])))
      end
      io.write("\n")
   end  

   do return end;

   local shdr_num = libshdr.shdr_num(fname)
   print('shdr_num: ', shdr_num)
   local shdrs = libshdr.get_shdr(fname, shdr_num)

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
end

loadelf()



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

