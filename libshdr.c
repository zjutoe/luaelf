/*
 * Print the names of ELF sections .
 *
 * $Id: prog4 .txt 2133 2011 -11 -10 08:28:22 Z jkoshy $
 */
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <bsd/vis.h>

#include "libshdr.h"

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
		err (EXIT_FAILURE, " open  \"% s\"  failed ", fname);

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

shdr_t* get_shdr(char *fname, int num)
{
	int fd;
	Elf *e;
	char *name , *p, pc [4* sizeof ( char )];
	Elf_Scn *scn ;
	Elf_Data * data ;
	GElf_Shdr shdr ;
	size_t n, shstrndx , sz;

	if ( elf_version ( EV_CURRENT ) == EV_NONE )
		errx ( EXIT_FAILURE , " ELF  library   initialization  "
		       " failed : %s", elf_errmsg ( -1));

	if (( fd = open (fname , O_RDONLY , 0)) < 0)
		err ( EXIT_FAILURE , " open  \%s\"  failed ", fname);
	if ((e = elf_begin (fd , ELF_C_READ , NULL )) == NULL )
		errx ( EXIT_FAILURE , " elf_begin ()  failed : %s.",
		       elf_errmsg ( -1));
	if ( elf_kind (e) != ELF_K_ELF )
		errx ( EXIT_FAILURE , "%s is  not an  ELF  object .",
		       fname);
	if ( elf_getshdrstrndx (e, & shstrndx ) != 0)
		errx ( EXIT_FAILURE , " elf_getshdrstrndx ()  failed : %s.",
		       elf_errmsg ( -1));
	scn = NULL ;

	shdr_t* buf = (shdr_t*)malloc(num * sizeof(shdr_t));
	int i = 0;

	while (( scn = elf_nextscn (e, scn )) != NULL && i < num ) {
		if ( gelf_getshdr (scn , & shdr ) != & shdr )
			errx ( EXIT_FAILURE , " getshdr ()  failed : %s.",
			       elf_errmsg ( -1));

		buf[i].sh_name   = shdr.sh_name;
		buf[i].sh_type   = shdr.sh_type;
		buf[i].sh_addr   = shdr.sh_addr;
		buf[i].sh_offset = shdr.sh_offset;
		i++;
				
		if (( name = elf_strptr (e, shstrndx , shdr.sh_name ))
		    == NULL )
			errx ( EXIT_FAILURE , " elf_strptr ()  failed : %s.",
			       elf_errmsg ( -1));
		( void ) printf (" Section  %-4.4jd %s %d\n", ( uintmax_t )
				 elf_ndxscn (scn), name, shdr.sh_name);
	}



	/* if (( scn = elf_getscn (e, shstrndx )) == NULL ) */
	/* 	errx ( EXIT_FAILURE , " getscn ()  failed : %s.", */
	/* 	       elf_errmsg ( -1)); */
	/* if ( gelf_getshdr (scn , & shdr ) != & shdr ) */
	/* 	errx ( EXIT_FAILURE , " getshdr ( shstrndx )  failed : %s.", */
	/* 	       elf_errmsg ( -1)); */
	/* ( void ) printf (". shstrab :  size =%jd\n", ( uintmax_t ) */
	/* 		 shdr . sh_size ); */



	/* data = NULL ; n = 0; */
	/* while (n < shdr.sh_size && */
	/*        ( data = elf_getdata (scn , data )) != NULL ) { */
	/* 	p = ( char *) data -> d_buf ; */
	/* 	while (p < ( char *) data -> d_buf + data -> d_size ) { */
	/* 		if ( vis(pc , *p, VIS_WHITE , 0)) */
	/* 			printf ("%s", pc ); */
	/* 		n++; p++; */

	/* 		( void ) putchar ((n % 16) ? ' ' : '\n'); */
	/* 	} */
	/* } */

	/* putchar ('\n'); */

	elf_end(e);
	close(fd);

	return buf;
}
