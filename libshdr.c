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
} g;

static int init_scns()
{
	Elf* e = g.e;
	size_t shstrndx = g.shstrndx;
	size_t shdrnum = g.shdrnum;

	int i = 0;
	Elf_Scn *scn = NULL;
	GElf_Shdr shdr;
	Elf_Scn** buf = (Elf_Scn**)malloc(shdrnum * sizeof(Elf_Scn*));
	if (buf == NULL) {
		errx (EXIT_FAILURE , "malloc failed");
		return -1;
	}
	while ((scn = elf_nextscn(e, scn)) != NULL && i < shdrnum) {
		if (gelf_getshdr (scn , & shdr ) != & shdr) {
			errx ( EXIT_FAILURE , " getshdr ()  failed : %s.",
			       elf_errmsg ( -1));
			free(buf);
			return -1;
		}
		buf[i] = scn;
		i++;
	}

	g.scns = buf;
	
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

	return 0;
}


void fini()
{
	free(g.scns);
	elf_end(g.e);
	close(g.fd);
}


// int shdr_num(char *fname)
// {
// 	int i, fd;
// 	Elf *e;
// 	char *id, bytes[5];
// 	size_t n;
// 	GElf_Ehdr ehdr;

// 	if (elf_version (EV_CURRENT) == EV_NONE)
// 		errx (EXIT_FAILURE, " ELF  library   initialization  "
// 		      " failed : %s", elf_errmsg (-1));

// 	if ((fd = open (fname, O_RDONLY, 0)) < 0)
// 		err (EXIT_FAILURE, " open  \"%s\"  failed ", fname);

// 	if ((e = elf_begin (fd, ELF_C_READ, NULL)) == NULL)
// 		errx (EXIT_FAILURE, " elf_begin ()  failed : %s.",
// 		      elf_errmsg (-1));

// 	if (elf_kind (e) != ELF_K_ELF)
// 		errx (EXIT_FAILURE, " \"%s\" is  not an  ELF  object .",
// 		      fname);

// 	if (gelf_getehdr (e, &ehdr) == NULL)
// 		errx (EXIT_FAILURE, " getehdr ()  failed : %s.",
// 		      elf_errmsg (-1));

// 	if ((i = gelf_getclass (e)) == ELFCLASSNONE)
// 		errx (EXIT_FAILURE, " getclass ()  failed : %s.",
// 		      elf_errmsg (-1));

// 	/* if ((id = elf_getident (e, NULL)) == NULL) */
// 	/* 	errx (EXIT_FAILURE, " getident ()  failed : %s.", */
// 	/* 	      elf_errmsg (-1)); */

// 	if (elf_getshdrnum (e, &n) != 0)
// 		errx (EXIT_FAILURE, " getshdrnum ()  failed : %s.",
// 		      elf_errmsg (-1));

// 	elf_end (e);
// 	close (fd);	
// 	return n;
// }


int get_scn_num()
{
	return g.shdrnum;
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

scn_hdr_t* get_scn_data(int idx)
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
		errx (EXIT_FAILURE , "malloc failed");
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
	// char* name;
	// if ((name = elf_strptr(g.e, g.shstrndx, shdr.sh_name))
	//     == NULL)
	// 	errx(EXIT_FAILURE , " elf_strptr()  failed : %s.",
	// 	       elf_errmsg ( -1));

	scn_hdr_t* scn_hdr_p = (scn_hdr_t*)malloc(sizeof(scn_hdr_t));
	if (scn_hdr_p == NULL) {
		errx (EXIT_FAILURE , "scn_hdr_t malloc failed");
		return NULL;
	}

	scn_hdr_p->sh_idx = idx;
	//scn_hdr_p->name = name;
	scn_hdr_p->sh_size = n;
	scn_hdr_p->data = buf;

	return scn_hdr_p;
}
