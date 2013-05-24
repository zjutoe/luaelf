/*
 * Print the ELF Executable Header from an ELF object .
 *
 * $Id: prog2 .txt 2133 2011 -11 -10 08:28:22 Z jkoshy $
 */
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <bsd/vis.h>

int
prog2(int argc, char * fname)
{
	int i, fd;
	Elf *e;
	char *id, bytes[5];
	size_t n;
	GElf_Ehdr ehdr;

  
	/* if (argc != 2) */
	/* 	errx (EXIT_FAILURE, " usage : %s file - name ", argv [0]); */

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
	(void) printf ("%s: %d-bit ELF  object \n", fname,
		       i == ELFCLASS32 ? 32 : 64);

	if ((id = elf_getident (e, NULL)) == NULL)
		errx (EXIT_FAILURE, " getident ()  failed : %s.",
		      elf_errmsg (-1));

	(void) printf ("%3s e_ident [0..%1d] %7s", " ",  EI_ABIVERSION, " ");

	for (i = 0; i <= EI_ABIVERSION; i++) {
		(void) vis(bytes, id[i], VIS_WHITE, 0);
		(void) printf (" ['%s' %X]", bytes, id[i]);
	}

	(void) printf ("\n");

#define PRINT_FMT "    % -20s 0x%jx\n"

#define PRINT_FIELD(N) do {						\
		(void) printf (PRINT_FMT, #N, (uintmax_t) ehdr.N);	\
	} while (0)

	PRINT_FIELD (e_type);
	PRINT_FIELD (e_machine);
	PRINT_FIELD (e_version);
	PRINT_FIELD (e_entry);
	PRINT_FIELD (e_phoff);
	PRINT_FIELD (e_shoff);
	PRINT_FIELD (e_flags);
	PRINT_FIELD (e_ehsize);
	PRINT_FIELD (e_phentsize);
	PRINT_FIELD (e_shentsize);

	if (elf_getshdrnum (e, &n) != 0)
		errx (EXIT_FAILURE, " getshdrnum ()  failed : %s.",
		      elf_errmsg (-1));

	(void) printf (PRINT_FMT, "(shnum)", (uintmax_t) n);

	if (elf_getshdrstrndx (e, &n) != 0)
		errx (EXIT_FAILURE, " getshdrstrndx ()  failed : %s.",
		      elf_errmsg (-1));

	(void) printf (PRINT_FMT, "(shstrndx)", (uintmax_t) n);

	if (elf_getphdrnum (e, &n) != 0)
		errx (EXIT_FAILURE, " getphdrnum ()  failed : %s.",
		      elf_errmsg (-1));

	(void) printf (PRINT_FMT, "(phnum)", (uintmax_t) n);

	(void) elf_end (e);
	(void) close (fd);
	exit (EXIT_SUCCESS);
}
