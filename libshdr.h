#ifndef _LIBSHDR_H
#define _LIBSHDR_H

typedef struct _shdr_t {
	uint32_t sh_name; /* Section name (index into the section header
			     string table). */
	uint32_t sh_type;	
	uint64_t sh_addr;
	uint64_t sh_offset;
} shdr_t;

#endif	/* _LIBSHDR_H */

