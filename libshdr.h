#ifndef _LIBSHDR_H
#define _LIBSHDR_H

typedef struct _shdr_t {
	uint32_t sh_idx;
	uint32_t sh_name; /* Section name (index into the section header
			     string table). */
	//char* sh_name_str;
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

#endif	/* _LIBSHDR_H */

