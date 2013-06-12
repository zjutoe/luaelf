local ffi = require 'ffi'

-- TODO shall we distinguish 32 bit and 64 bit machines?

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

typedef struct {
	uint64_t p_idx;
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_flags;
	uint64_t p_align;
} prog_hdr_t;

      int init(char* fname);
      void fini();
      int get_scn_num();
      int get_seg_num();
      size_t get_scn_size(int idx);
      scn_hdr_t* get_scn_hdr(int idx);
      prog_hdr_t* get_prog_hdr(int idx);
]]


ffi.load('elf', true)
ffi.load('bsd', true)
libshdr = ffi.load('./libshdr.so')

function init_elf(fname)
   libshdr.init(fname)
end

function load_scns()

   local fname = ffi.new(string.format("char[%d]", string.len(arg[1])+1), arg[1])
   libshdr.init(fname)

   local n = tonumber(libshdr.get_scn_num())
   local scns = {}
   for idx=0, n-2 do
      local scn_hdr = libshdr.get_scn_hdr(idx)
      scns[#scns + 1] = scn_hdr
      print(string.format('section %d of %d, %s size 0x%x', 
      			  idx, n,
      			  tostring(scn_hdr.name), 
      			  tonumber(scn_hdr.sh_size)))
      local sz = tonumber(scn_hdr.sh_size)
      for i=0, sz-1 do 
      	 if i % 16 == 0 then
      	    io.write("\n")
      	 end
      	 io.write(string.format("%02x ", tonumber(scn_hdr.data[i])))
	 if i > 128 then break end
      end
      io.write("\n")
   end  

   return scns
end

function load_segs()
   local n = libshdr.get_seg_num()
   print(n, "segments")
   for i=0, n do
      local ph = libshdr.get_prog_hdr(i)
      print(string.format("    idx %x off %X va %X pa %X fsz %X msz %X", 
			  tonumber(ph.p_idx),
			  tonumber(ph.p_offset),
			  tonumber(ph.p_vaddr),
			  tonumber(ph.p_paddr),
			  tonumber(ph.p_filesz),
			  tonumber(ph.p_memsz)
			 ))
   end
end

load_scns()
load_segs()


