#ifndef _LIBSHDR_H
#define _LIBSHDR_H

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


typedef unsigned long bfd_vma;

/* .tbss is special.  It doesn't contribute memory space to normal
   segments and it doesn't take file space in normal segments.  */
#define ELF_TBSS_SPECIAL(sec_hdr, segment)			\
  (((sec_hdr)->sh_flags & SHF_TLS) != 0				\
   && (sec_hdr)->sh_type == SHT_NOBITS				\
   && (segment)->p_type != PT_TLS)

#define ELF_SECTION_SIZE(sec_hdr, segment)			\
  (ELF_TBSS_SPECIAL(sec_hdr, segment) ? 0 : (sec_hdr)->sh_size)

/* Decide if the section SEC_HDR is in SEGMENT.  If CHECK_VMA, then
   VMAs are checked for alloc sections.  If STRICT, then a zero size
   section won't match at the end of a segment, unless the segment
   is also zero size.  Regardless of STRICT and CHECK_VMA, zero size
   sections won't match at the start or end of PT_DYNAMIC, unless
   PT_DYNAMIC is itself zero sized.  */
#define ELF_SECTION_IN_SEGMENT_1(sec_hdr, segment, check_vma, strict)	\
  ((/* Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain	\
       SHF_TLS sections.  */						\
    ((((sec_hdr)->sh_flags & SHF_TLS) != 0)				\
     && ((segment)->p_type == PT_TLS					\
	 || (segment)->p_type == PT_GNU_RELRO				\
	 || (segment)->p_type == PT_LOAD))				\
    /* PT_TLS segment contains only SHF_TLS sections, PT_PHDR no	\
       sections at all.  */						\
    || (((sec_hdr)->sh_flags & SHF_TLS) == 0				\
	&& (segment)->p_type != PT_TLS					\
	&& (segment)->p_type != PT_PHDR))				\
   /* Any section besides one of type SHT_NOBITS must have file		\
      offsets within the segment.  */					\
   && ((sec_hdr)->sh_type == SHT_NOBITS					\
       || ((bfd_vma) (sec_hdr)->sh_offset >= (segment)->p_offset	\
	   && (!(strict)						\
	       || ((sec_hdr)->sh_offset - (segment)->p_offset		\
		   <= (segment)->p_filesz - 1))				\
	   && (((sec_hdr)->sh_offset - (segment)->p_offset		\
		+ ELF_SECTION_SIZE(sec_hdr, segment))			\
	       <= (segment)->p_filesz)))				\
   /* SHF_ALLOC sections must have VMAs within the segment.  */		\
   && (!(check_vma)							\
       || ((sec_hdr)->sh_flags & SHF_ALLOC) == 0			\
       || ((sec_hdr)->sh_addr >= (segment)->p_vaddr			\
	   && (!(strict)						\
	       || ((sec_hdr)->sh_addr - (segment)->p_vaddr		\
		   <= (segment)->p_memsz - 1))				\
	   && (((sec_hdr)->sh_addr - (segment)->p_vaddr			\
		+ ELF_SECTION_SIZE(sec_hdr, segment))			\
	       <= (segment)->p_memsz)))					\
   /* No zero size sections at start or end of PT_DYNAMIC.  */		\
   && ((segment)->p_type != PT_DYNAMIC					\
       || (sec_hdr)->sh_size != 0					\
       || (segment)->p_memsz == 0					\
       || (((sec_hdr)->sh_type == SHT_NOBITS				\
	    || ((bfd_vma) (sec_hdr)->sh_offset > (segment)->p_offset	\
	        && ((sec_hdr)->sh_offset - (segment)->p_offset		\
		    < (segment)->p_filesz)))				\
	   && (((sec_hdr)->sh_flags & SHF_ALLOC) == 0			\
	       || ((sec_hdr)->sh_addr > (segment)->p_vaddr		\
		   && ((sec_hdr)->sh_addr - (segment)->p_vaddr		\
		       < (segment)->p_memsz))))))

#define ELF_SECTION_IN_SEGMENT(sec_hdr, segment)			\
  (ELF_SECTION_IN_SEGMENT_1 (sec_hdr, segment, 1, 0))

#define ELF_SECTION_IN_SEGMENT_STRICT(sec_hdr, segment)			\
  (ELF_SECTION_IN_SEGMENT_1 (sec_hdr, segment, 1, 1))



#endif	/* _LIBSHDR_H */

