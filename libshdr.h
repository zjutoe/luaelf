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

#endif	/* _LIBSHDR_H */

