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

	return 0;
}


void fini()
{
	free(g.scns);
	elf_end(g.e);
	close(g.fd);
}



int shdr_num(char *fname)
{
	int i, fd;
	Elf *e;
	char *id, bytes[5];
	size_t n;
	GElf_Ehdr ehdr;

	if (elf_version (EV_CURRENT) == EV_NONE)
		errx (EXIT_FAILURE, " ELF  library   initialization  "
		      " failed : %s", elf_errmsg (-1));

	if ((fd = open (fname, O_RDONLY, 0)) < 0)
		err (EXIT_FAILURE, " open  \"%s\"  failed ", fname);

	if ((e = elf_begin (fd, ELF_C_READ, NULL)) == NULL)
		errx (EXIT_FAILURE, " elf_begin ()  failed : %s.",
		      elf_errmsg (-1));

	if (elf_kind (e) != ELF_K_ELF)
		errx (EXIT_FAILURE, " \"%s\" is  not an  ELF  object .",
		      fname);

	if (gelf_getehdr (e, &ehdr) == NULL)
		errx (EXIT_FAILURE, " getehdr ()  failed : %s.",
		      elf_errmsg (-1));

	if ((i = gelf_getclass (e)) == ELFCLASSNONE)
		errx (EXIT_FAILURE, " getclass ()  failed : %s.",
		      elf_errmsg (-1));

	/* if ((id = elf_getident (e, NULL)) == NULL) */
	/* 	errx (EXIT_FAILURE, " getident ()  failed : %s.", */
	/* 	      elf_errmsg (-1)); */

	if (elf_getshdrnum (e, &n) != 0)
		errx (EXIT_FAILURE, " getshdrnum ()  failed : %s.",
		      elf_errmsg (-1));

	elf_end (e);
	close (fd);	
	return n;
}

int get_scns()
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


int get_scn_num(int scn_idx)
{
	return g.shdrnum;
}

size_t get_scn_size(int scn_idx)
{
	Elf_Scn* scn = g.scns[scn_idx];
	GElf_Shdr shdr;
	if ( gelf_getshdr (scn , & shdr ) != & shdr ) {
		errx ( EXIT_FAILURE , " getshdr ( shstrndx )  failed : %s.",
		       elf_errmsg ( -1));
		return -1;
	}

	return shdr.sh_size;
}

uint8_t* get_scn_data(int scn_idx)
{
	Elf_Scn* scn = g.scns[scn_idx];

	Elf_Data* data;
	size_t n;

	GElf_Shdr shdr;
	if ( gelf_getshdr (scn , & shdr ) != & shdr ) {
		errx ( EXIT_FAILURE , " getshdr ( shstrndx )  failed : %s.",
		       elf_errmsg ( -1));
		return NULL;
	}


	size_t buf_sz = 0;
	/* cycle through the Elf_Data descriptors to sum up the size */
	data = NULL;
	while ( (data=elf_getdata(scn, data)) != NULL ) {
		buf_sz += data->d_size;
	}
	uint8_t* buf = (uint8_t*) malloc(buf_sz);
	if (buf == NULL) {		
		errx (EXIT_FAILURE , "malloc failed");
		return NULL;
	}

	data = NULL; n = 0;
	while (n < shdr.sh_size &&
	       (data = elf_getdata(scn, data)) != NULL) {
		uint8_t* p = (uint8_t*)data->d_buf;
		size_t dsize = data->d_size;
		memcpy(buf+n, p, dsize);
		/* for (size_t i=0; i<dsize; i++) { */
		/* } */
		n += dsize;
	}

	return buf;
}
