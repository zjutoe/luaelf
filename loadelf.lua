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
   print("segments:")
   print("idx   offset  virtual physical filesize  memsize Flg Align")
   for i=0, n-1 do
      local ph = libshdr.get_prog_hdr(i)
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
   return 0 			-- FIXME
end

function scn_in_seg(scn, seg)
   local strict    = true
   local check_vma = true

   local segtype  = seg.p_type
   local sectype  = scn.sh_type
   local secflags = scn.sh_flags
   local secoff   = scn.sh_offset
   local segoff   = seg.p_offset
   local secaddr  = scn.sh_addr
   local segaddr  = seg.p_vaddr
   local segfsize = seg.p_filesz
   local segmsize = seg.p_memsz
   local secsize  = scn.sh_size

   --Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain
   --SHF_TLS sections
   local cond1 = ( bit.band(secflags, SH_FLAGS.SHF_TLS) ~= 0 
		and ( segtype == PTYPE.PT_TLS or
		      segtype == PTYPE.PT_LOAD or
		      segtype == PTYPE.PT_GNU_RELRO))

   --PT_TLS segment contains only SHF_TLS sections, PT_PHDR no
   --sections at all.
   local cond2 = ( bit.band(secflags, SH_FLAGS.SHF_TLS) == 0 
		and segtype ~= PTYPE.PT_TLS
		and segtype ~= PTYPE.PT_PHDR )

   if not(cond1 or cond2) then return false end

   -- Any section besides one of type SHT_NOBITS must have file
   -- offsets within the segment.
   local cond3 = ( sectype == STYPE.SHT_NOBITS or
		   ( secoff >= segoff and
		     ( (not strict) or secoff - segoff <= segfsize - 1) and
		  ( secoff - segoff + elf_section_size(scn, seg) <= segfsize)))
	       
   if not cond3 then return false end   
      

   -- SHF_ALLOC sections must have VMAs within the segment.
   local cond4 = ( (not check_vma) or
		   (bit.band(secflags, SH_FLAGS.SHF_ALLOC) == 0) or
		   (secaddr >= segaddr and
		      ((not strict) or secaddr - segaddr <= segmsize - 1) and
		      (secaddr - segaddr + slf_section_size(scn, seg) <= segmsize)) )
		 
   if not cond4 then return false end

   -- No zero size sections at start or end of PT_DYNAMIC.
   local cond5 = (segtype ~= PTYPE.PT_DYNAMIC or 
		  secsize ~= 0 or
		  segmsize == 0 or
		  ((sectype == STYPE.SHT_NOBITS or
		    (secoff > segoff and secoff - segoff < segfsize)) and
		   (bit.band(secflags, SH_FLAGS.SHF_ALLOC) == 0 or
		   (secaddr > segaddr and secaddr - segaddr < segmsize))))

   if not cond5 then return false end

   return true
end

load_scns()
load_segs()


