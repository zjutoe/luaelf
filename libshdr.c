/*
 * Load sections of ELF
 * Author: Zju Toe <zjutoe@gmail.com>
 *
 * Referenced code snippets in "libelf by example"
 * http://sourceforge.net/projects/elftoolchain/files/Documentation/libelf-by-example/
 */
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <bsd/vis.h>
#include <string.h>

#include "libshdr.h"

static struct {
	int fd;
	Elf* e;
	GElf_Ehdr ehdr;
	size_t shdrnum;
	size_t phdrnum;
	size_t shstrndx;
	Elf_Scn** scns;
	GElf_Phdr* phdrs;
	GElf_Shdr* shdrs;
} g;

static int init_phdrs()
{
	Elf* e = g.e;
	size_t phdr_num = g.phdrnum;
	GElf_Phdr* buf = (GElf_Phdr*) malloc(phdr_num * sizeof(GElf_Phdr));
	if (buf == NULL) {
		errx (EXIT_FAILURE , "malloc failed with GElf_Phdr*");
		return -1;
	}

	int i;
	for (i=0; i<phdr_num; i++) {
		//GElf_Phdr* phdr = (GElf_Phdr*) malloc(sizeof(GElf_Phdr));
		GElf_Phdr* phdr = buf + i;
		if (gelf_getphdr (e, i, phdr) != phdr) {
			errx(EXIT_FAILURE, " gelf_getphdr() failed : %s.",
			       elf_errmsg(-1));
		}
	}

	g.phdrs = buf;
	return 0;
}

static int init_scns()
{
	Elf* e = g.e;
	size_t shstrndx = g.shstrndx;
	size_t shdrnum = g.shdrnum;

	int i = 0;
	GElf_Shdr shdr;
	Elf_Scn** buf   = (Elf_Scn**)malloc(shdrnum * sizeof(Elf_Scn*));
	GElf_Shdr* buf2 = (GElf_Shdr*)malloc(shdrnum * sizeof(GElf_Shdr));

	if (buf == NULL) {
		errx (EXIT_FAILURE , "malloc failed");
		return -1;
	}
	Elf_Scn *scn = NULL;
	while ((scn = elf_nextscn(e, scn)) != NULL && i < shdrnum) {
		if (gelf_getshdr (scn , & shdr ) != & shdr) {
			errx ( EXIT_FAILURE , " getshdr ()  failed : %s.",
			       elf_errmsg ( -1));
			free(buf);
			return -1;
		}
		buf[i]  = scn;
		buf2[i] = shdr;
		i++;
	}

	if (i < shdrnum) {
	  /* the .shstrtab section*/
	  /* size_t shstrndx; */
	  /* if (elf_getshdrstrndx (e, &shstrndx) != 0) */
	  /*   errx (EXIT_FAILURE, " getshdrstrndx ()  failed : %s.", */
	  /* 	  elf_errmsg (-1)); */
	  if ((scn = elf_getscn (e, shstrndx)) == NULL )
	    errx ( EXIT_FAILURE , " getscn ()  failed : %s.",
		   elf_errmsg ( -1));
	  if (gelf_getshdr(scn , &shdr) != & shdr )
	    errx ( EXIT_FAILURE , " getshdr ( shstrndx )  failed : %s.",
		   elf_errmsg ( -1));
	  buf[i] = scn;
	  buf2[i] = shdr;
	}

	// printf("[D] No. of sections:%d\n", i-1);
	
	g.scns  = buf;
	g.shdrs = buf2;
	
	return 0;
}

static int init_segs()
{
	return 0;
}

int init(char* fname)
{
	char *id, bytes[5];

	if (elf_version (EV_CURRENT) == EV_NONE) {
		errx (EXIT_FAILURE, " ELF  library   initialization  "
		      " failed : %s", elf_errmsg (-1));
		return -1;
	}

	int fd;
	if ((fd = open (fname, O_RDONLY, 0)) < 0) {
		err (EXIT_FAILURE, " open  \"%s\"  failed ", fname);
		return -1;
	}
	g.fd = fd;

	Elf *e;
	if ((e = elf_begin (fd, ELF_C_READ, NULL)) == NULL) {
		errx (EXIT_FAILURE, " elf_begin ()  failed : %s.",
		      elf_errmsg (-1));
		return -1;
	}

	if (elf_kind (e) != ELF_K_ELF) {
		errx (EXIT_FAILURE, " \"%s\" is  not an  ELF  object .",
		      fname);
		return -1;
	}
	g.e = e;

	GElf_Ehdr ehdr;
	if (gelf_getehdr (e, &ehdr) == NULL) {
		errx (EXIT_FAILURE, " getehdr ()  failed : %s.",
		      elf_errmsg (-1));
		return -1;
	}
	g.ehdr = ehdr;

	int i;
	if ((i = gelf_getclass (e)) == ELFCLASSNONE) {
		errx (EXIT_FAILURE, " getclass ()  failed : %s.",
		      elf_errmsg (-1));
		return -1;
	}

	if ((id = elf_getident (e, NULL)) == NULL) {
		errx (EXIT_FAILURE, " getident ()  failed : %s.",
		      elf_errmsg (-1));
		return -1;
	}

	size_t n;
	if (elf_getshdrnum (e, &n) != 0) {
		errx (EXIT_FAILURE, " getshdrnum ()  failed : %s.",
		      elf_errmsg (-1));
		return -1;
	}
	g.shdrnum = n;

	if (elf_getshdrstrndx (e, &n) != 0) {
		errx (EXIT_FAILURE, " getshdrstrndx ()  failed : %s.",
		      elf_errmsg (-1));
		return -1;
	}
	g.shstrndx = n;

	if (elf_getphdrnum (e, &n) != 0) {
		errx (EXIT_FAILURE, " getphdrnum ()  failed : %s.",
		      elf_errmsg (-1));
		return -1;
	}
	g.phdrnum = n;

	init_scns();
	init_phdrs();

	return 0;
}


void fini()
{
	free(g.phdrs);

	int i;
	for (i=0; i<g.shdrnum; i++)
		free(g.scns[i]);
	free(g.scns);

	elf_end(g.e);
	close(g.fd);
}


int get_scn_num()
{
	return g.shdrnum;
}

int get_seg_num()
{
	return g.phdrnum;
}

size_t get_scn_size(int idx)
{
	Elf_Scn* scn = g.scns[idx];
	GElf_Shdr shdr;
	if ( gelf_getshdr (scn , & shdr ) != & shdr ) {
		errx ( EXIT_FAILURE , " getshdr ( shstrndx )  failed : %s.",
		       elf_errmsg ( -1));
		return -1;
	}

	return shdr.sh_size;
}

prog_hdr_t* get_prog_hdr(int idx)
{
	GElf_Phdr* h = g.phdrs + idx;
	prog_hdr_t* ph = (prog_hdr_t*)malloc(sizeof(prog_hdr_t));
	// printf("[D] idx %x off %x va %x pa %x fsz %x msz %x\n",
	//        idx, h->p_offset, h->p_vaddr, h->p_paddr, h->p_filesz,
	//        h->p_memsz);
	ph->p_idx    = idx;
	ph->p_offset = h->p_offset;
	ph->p_vaddr  = h->p_vaddr;
	ph->p_paddr  = h->p_paddr;
	ph->p_filesz = h->p_filesz;
	ph->p_memsz  = h->p_memsz;
	ph->p_flags  = h->p_flags;
	ph->p_align  = h->p_align;

	return ph;
}

scn_hdr_t* get_scn_hdr(int idx)
{
	Elf_Scn* scn = g.scns[idx];

	Elf_Data* data;
	size_t n;

	GElf_Shdr shdr;
	if ( gelf_getshdr(scn , &shdr) != &shdr ) {
		errx ( EXIT_FAILURE,
		       " getshdr( shstrndx ) failed : %s.",
		       elf_errmsg ( -1));
		return NULL;
	}

	// data = NULL; size_t buf_sz = 0;
	// while ((data=elf_getdata(scn, data)) != NULL) {
	// 	printf("+%d", data->d_size);
	// 	buf_sz += data->d_size;
	// }

	uint8_t* buf = (uint8_t*) malloc(shdr.sh_size);
	if (buf == NULL) {		
		errx (EXIT_FAILURE, "malloc failed");
		return NULL;
	}

	data = NULL; n = 0;
	while (n < shdr.sh_size &&
	       (data = elf_getdata(scn, data)) != NULL) {
		uint8_t* p = (uint8_t*)data->d_buf;
		size_t dsize = data->d_size;
		if(p != NULL)
			memcpy(buf+n, p, dsize);
		else //FIXME: seems some sections are empty in file
			memset(buf+n, 0, dsize);
		n += dsize;
	}

	// get the name string
	char* name;
	if ((name = elf_strptr(g.e, g.shstrndx, shdr.sh_name)) == NULL)
	  errx(EXIT_FAILURE , " elf_strptr()  failed : %s.",
	       elf_errmsg (-1));

	scn_hdr_t* scn_hdr_p = (scn_hdr_t*)malloc(sizeof(scn_hdr_t));
	if (scn_hdr_p == NULL) {
		errx (EXIT_FAILURE , "scn_hdr_t malloc failed");
		return NULL;
	}

	scn_hdr_p->sh_idx = idx;
	scn_hdr_p->name = name;
	scn_hdr_p->sh_addr = shdr.sh_addr;
	scn_hdr_p->sh_size = n;
	scn_hdr_p->data = buf;

	return scn_hdr_p;
}


int sec_in_seg_strict(int sec_hdr_idx, int seg_hdr_idx)
{
	GElf_Shdr shdr = g.shdrs[sec_hdr_idx];
	GElf_Phdr phdr = g.phdrs[seg_hdr_idx];

	return ELF_SECTION_IN_SEGMENT_STRICT(&shdr, &phdr);
}
