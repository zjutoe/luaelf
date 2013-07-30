local ffi = require 'ffi'
local pairs = pairs
local string = string
local tonumber = tonumber
local print = print

module(...)

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
	 uint64_t p_type;
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
      size_t get_e_entry();
      size_t get_scn_size(int idx);
      scn_hdr_t* get_scn_hdr(int idx);
      prog_hdr_t* get_prog_hdr(int idx);
      int sec_in_seg_strict(int sec_hdr_idx, int seg_hdr_idx);

]]

local _m = {}

function init()
   local m = {}
   for k, v in pairs(_m) do
      m[k] = v
   end
   return m   
end

ffi.load('elf', true)
ffi.load('bsd', true)
local elfconn = ffi.load('elfconn')

function init_elf(fname)
   elfconn.init(ffi.new("char[?]", string.len(fname), fname))
end

function load_scns()

--   local fname = ffi.new(string.format("char[%d]", string.len(arg[1])+1), arg[1])
--   elfconn.init(fname)

   local n = tonumber(elfconn.get_scn_num())
   local scns = {}
   print("Index name addr size")
   for idx=0, n-1 do
      local scn_hdr = elfconn.get_scn_hdr(idx)
      scns[#scns + 1] = scn_hdr
      print(string.format('%d     %s 0x%x 0x%x', 
      			  idx,
      			  ffi.string(scn_hdr.name), 
			  tonumber(scn_hdr.sh_addr),
      			  tonumber(scn_hdr.sh_size)))

      -- local sz = tonumber(scn_hdr.sh_size)
      -- for i=0, sz-1 do 
      -- 	 if i % 16 == 0 then	    
      -- 	    io.write("\n")
      -- 	 end
      -- 	 io.write(string.format("%02x ", tonumber(scn_hdr.data[i])))
      -- 	 if i > 128 then break end
      -- end
      -- io.write("\n")
   end  

   return scns
end

function load_segs()
   local n = elfconn.get_seg_num()
   local segs = {}
   print("segments:")
   print("idx   offset  virtual physical filesize  memsize Flg Align")
   for i=0, n-1 do
      local ph = elfconn.get_prog_hdr(i)
      segs[#segs+1] = ph
      print(string.format("%3x %08x %08x %08x %08x %08x %3x %x", 
			  tonumber(ph.p_idx),
			  tonumber(ph.p_offset),
			  tonumber(ph.p_vaddr),
			  tonumber(ph.p_paddr),
			  tonumber(ph.p_filesz),
			  tonumber(ph.p_memsz),
			  tonumber(ph.p_flags),
			  tonumber(ph.p_align)
		    ))
   end

   return segs
end

local PTYPE = {
    PT_NULL=0,
    PT_LOAD=1,
    PT_DYNAMIC=2,
    PT_INTERP=3,
    PT_NOTE=4,
    PT_SHLIB=5,
    PT_PHDR=6,
    PT_TLS=7,
    PT_LOPROC=0x70000000,
    PT_HIPROC=0x7fffffff,
    PT_GNU_EH_FRAME=0x6474e550,
    PT_GNU_STACK=0x6474e551,
    PT_GNU_RELRO=0x6474e552,
    PT_ARM_ARCHEXT=0x70000000,
    PT_ARM_EXIDX=0x70000001,
    PT_ARM_UNWIND=0x70000001,
    PT_AARCH64_ARCHEXT=0x70000000,
    PT_AARCH64_UNWIND=0x70000001,
}

local STYPE = {
    SHT_NULL=0,
    SHT_PROGBITS=1,
    SHT_SYMTAB=2,
    SHT_STRTAB=3,
    SHT_RELA=4,
    SHT_HASH=5,
    SHT_DYNAMIC=6,
    SHT_NOTE=7,
    SHT_NOBITS=8,
    SHT_REL=9,
    SHT_SHLIB=10,
    SHT_DYNSYM=11,
    SHT_INIT_ARRAY=14,
    SHT_FINI_ARRAY=15,
    SHT_PREINIT_ARRAY=16,
    SHT_GROUP=17,
    SHT_SYMTAB_SHNDX=18,
    SHT_NUM=19,
    SHT_LOOS=0x60000000,
    SHT_GNU_HASH=0x6ffffff6,
    SHT_GNU_verdef=0x6ffffffd,  -- also SHT_SUNW_verdef
    SHT_GNU_verneed=0x6ffffffe, -- also SHT_SUNW_verneed
    SHT_GNU_versym=0x6fffffff,  -- also SHT_SUNW_versym
    SHT_LOPROC=0x70000000,
    SHT_HIPROC=0x7fffffff,
    SHT_LOUSER=0x80000000,
    SHT_HIUSER=0xffffffff,
    SHT_AMD64_UNWIND=0x70000001,
    SHT_SUNW_syminfo=0x6ffffffc,
    SHT_ARM_EXIDX=0x70000001,
    SHT_ARM_PREEMPTMAP=0x70000002,
    SHT_ARM_ATTRIBUTES=0x70000003,
    SHT_ARM_DEBUGOVERLAY=0x70000004,
}

local SH_FLAGS = {
    SHF_WRITE=0x1,
    SHF_ALLOC=0x2,
    SHF_EXECINSTR=0x4,
    SHF_MERGE=0x10,
    SHF_STRINGS=0x20,
    SHF_INFO_LINK=0x40,
    SHF_LINK_ORDER=0x80,
    SHF_OS_NONCONFORMING=0x100,
    SHF_GROUP=0x200,
    SHF_TLS=0x400,
    SHF_MASKOS=0x0ff00000,
    SHF_EXCLUDE=0x80000000,
    SHF_MASKPROC=0xf0000000,
}

function elf_section_size(scn, seg)
   local elf_tbss_special = (bit.band(tonumber(scn.sh_flags), SH_FLAGS.SHF_TLS) ~= 0) and
                            (tonumber(scn.sh_type) == STYPE.SHT_NOBITS) and
			    (tonumber(seg.p_tpye) ~= PTYPE.PT_TLS)			 
   if elf_tbss_special then
      return 0
   end

   return scn.sh_size
end


function load_scn_mem(mem, scn_hdr)
   -- local scn_name = ffi.string(scn_hdr.name)
   local scn_addr = tonumber(scn_hdr.sh_addr)
   local sz = tonumber(scn_hdr.sh_size)

   for i=0, sz-1 do 
      mem[scn_addr+i] = tonumber(scn_hdr.data[i])
   end

   return mem
end

function _m.load(fname)

   init_elf(fname)
   
   local scns = load_scns()
   local segs = load_segs()
   local mem = {}
   mem.scns = scns
   mem.e_entry = tonumber(elfconn.get_e_entry())

   for i=0, elfconn.get_seg_num()-1 do
      -- io.write(string.format("%d: ", i))
      for j=0, elfconn.get_scn_num()-1 do
	 local segtype = tonumber(segs[i+1].p_type)
	 if segtype == PTYPE.PT_LOAD and tonumber(elfconn.sec_in_seg_strict(j, i)) == 1 then
	    -- io.write(string.format("%d, ", j))
	    load_scn_mem(mem, scns[j+1])
	 end
      end
      -- print("")
   end

   return mem
end

function _m.get_e_entry()
   
end
