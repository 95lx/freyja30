/* MAKEEXM.C -- Make a .EXE File Into a .EXM File

	Written March 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include <stdio.h>
#include "lib.h"

#define STACKSIZE	(4096 / 16);
int _stklen = STACKSIZE;

static struct {
	unsigned char one[28672];
	unsigned char two[17408];
	} patch_buf;

unsigned short Find();	/* unsigned short *nearfarptr, FLAG ispatch,
			char *map_file */
void Fixups();		/* char *file */
unsigned short Patch();	/* char *exm_file, unsigned short codeend,
			unsigned short nearfar **/
void Patch_One();	/* unsigned amt, unsigned short addr,
			unsigned short nearfar */
unsigned short Update(); /* char *exm_file, unsigned short codeend */

/* ------------------------------------------------------------ */

int
main(argc, argv)
	int argc;
	char *argv[];
	{
	char *map_file = NULL;
	char *exm_file = NULL;
	FLAG opt_f = FALSE;
	FLAG opt_p = FALSE;
	unsigned short codeend;
	unsigned short nearfar;
	unsigned short fixups;
	int start;
	int cnt;

	if (argc < 2) {
usage:
		xprintf("Make .EXM file program.\n\
usage is:  makeexm [options] <.exm file> [<.map file>]\n\
options are:\n\
	[-f]	print the number of fixups in each of the <file(s)>\n\
	[-p]	patch the far calls\n%s", COPY);
		exit(1);
		}

	for (cnt = 1; cnt < argc; cnt++) {
		if (strequ(argv[cnt], "-f")) {
			opt_f = TRUE;
			}
		else if (strequ(argv[cnt], "-p")) {
			opt_p = TRUE;
			}
		else if (*argv[cnt] == '-') {
			goto usage;
			}
		else if (exm_file == NULL) {
			exm_file = argv[cnt];
			}
		else	{
			map_file = argv[cnt];
			}
		}

/* check arguments */

	if (exm_file == NULL) {
		xeprintf("No .exm file specified -- program cannot run.\n");
		goto usage;
		}

	if (opt_f && opt_p) {
		xeprintf("Cannot specify both -f and -p.\n");
		goto usage;
		}

	if (opt_f) {
		for (cnt = 1; cnt < argc; cnt++) {
			if (*argv[cnt] != '-') Fixups(argv[cnt]);
			}
		exit(0);
		}

	if (map_file == NULL) map_file = exm_file;

		/* find value */

	codeend = Find(&nearfar, opt_p, map_file);

		/* update .exm file */

	if (opt_p)
		fixups = Patch(exm_file, codeend, nearfar);
	else	fixups = Update(exm_file, codeend);

	xprintf("Converted %s, data segment %x, %u fixups\n", exm_file,
		codeend, fixups);
	exit(0);
	}


/* ------------------------------------------------------------ */

/* Find the start of the data segment.  This is the first of either
_INIT_ or _DATA. Print a message and abort if error.

If ISPATCH is True, also find the address for the nearfar routine and
return it in NEARFARPTR. */

unsigned short
Find(nearfarptr, ispatch, map_file)
	unsigned short *nearfarptr;
	FLAG ispatch;
	char *map_file;
	{
	char fname[FNAMEMAX];
	unsigned char buf[BUFFSIZE];
	unsigned short codeend;
	FILE *fp;

	xstrcpy(fname, map_file);
	PSetExt(fname, "MAP", TRUE);

	if ((fp = fopen(fname, "r")) == NULL) {
		xeprintf("Unable to open map file '%s'.\n", fname);
		exit(1);
		}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strlen(buf) < 27) continue;
		if (!strnequ(&buf[22], "_DATA", 5) && 
			 !strnequ(&buf[22], "_INIT_", 6))
			continue;

		buf[5] = NUL;
		if (!SToN(&buf[1], &codeend, 16)) {
			fclose(fp);
			xeprintf("Unable to find start of data segment in %s.\n",
				fname);
			exit(1);
			}
		if (!ispatch) {
			fclose(fp);
			return(codeend);
			}
		else	break;
		}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strlen(buf) < 25 ||
			 !strnequ(&buf[0], " 0000:", 6) ||
			 !strnequ(&buf[10], "       _nearfar", 15))
			continue;
		buf[10] = NUL;
		if (!SToN(&buf[6], nearfarptr, 16)) break;
		fclose(fp);
		return(codeend);
		}

	fclose(fp);
	xeprintf("Unable to find _nearfar in %s.\n", fname);
	exit(1);
	}


/* ------------------------------------------------------------ */

/* Print the number of fixups in each file. */

void
Fixups(file)
	char *file;
	{
	unsigned char buf[BUFFSIZE];
	int fd;

	if ((fd = open(file, O_RDONLY)) < 0) {
		xeprintf("Unable to open file '%s'.\n", file);
		return;
		}
	if (read(fd, buf, sizeof(buf)) <= 0x1b) {
		xeprintf("Unable to read file %s.\n", file);
		close(fd);
		return;
		}
	close(fd);
	xprintf("%s: %u fixups.\n", file,  (buf[7] << 8) + buf[6]);
	}


/* ------------------------------------------------------------ */

/* Update the magic number of code end segment for the .exm file. 
Also patch out all fixups. Print a message and abort if error.  Return
the number of fixups. */

unsigned short
Patch(exm_file, codeend, nearfar)
	char *exm_file;
	unsigned short codeend;
	unsigned short nearfar;
	{
	char fname[FNAMEMAX];
	int amt1 = 0;
	int amt2 = 0;
	int fd;
	int cnt;
	int numfixups;
	unsigned short base;
	unsigned short *uptr;

	xstrcpy(fname, exm_file);
	PSetExt(fname, "EXM", FALSE);

	if ((fd = open(fname, O_RDWR)) < 0) {
		xeprintf("Unable to open exm file '%s'.\n", fname);
		exit(1);
		}

	if ((amt1 = read(fd, patch_buf.one, sizeof(patch_buf.one))) <= 0) {
		xeprintf("Unable to read exm file %s.\n", fname);
		close(fd);
		exit(1);
		}
	if (amt1 == sizeof(patch_buf.one)) {
		if ((amt2 = read(fd, patch_buf.two,
			 sizeof(patch_buf.two))) <= 0) {
			xeprintf("Unable to read exm file %s.\n", fname);
			close(fd);
			exit(1);
			}
		}


/* set the new magic number */
	patch_buf.one[0] = 0x44;
	patch_buf.one[1] = 0x4c;

/* set the data segment start */
	patch_buf.one[0x1a] = codeend;
	patch_buf.one[0x1b] = codeend >> 8;

	numfixups = (patch_buf.one[7] << 8) + patch_buf.one[6];
	patch_buf.one[6] = 0;
	patch_buf.one[7] = 0;

	base = (patch_buf.one[25] << 8) + patch_buf.one[24];
	xprintf("patching %d...", numfixups);

	for (cnt = 0; cnt < numfixups; cnt++) {
		uptr = (unsigned short *)&patch_buf.one[base + cnt * 4];
		Patch_One(amt1 + amt2, *uptr, nearfar);
		*uptr = 0;
		}

	xprintf("\n");

	lseek(fd, 0L, 0);
	if (write(fd, patch_buf.one, amt1) != amt1 ||
		 (amt2 > 0 && write(fd, patch_buf.two, amt2) != amt2)) {
		xeprintf("Unable to update exm file %s.\n", fname);
		close(fd);
		exit(1);
		}

	close(fd);
	return(0);
	}


/* ------------------------------------------------------------ */

/* Patch one call.  Call format is:

	0x9A  low high 0x00 0x00
			^

and ADDR points to the ^.  Change this to:

	0xE8  nearfar   low high

and complain if it is out of the buffer. */

void
Patch_One(amt, addr, nearfar)
	unsigned amt;
	unsigned short addr;
	unsigned short nearfar;
	{
	unsigned char *cptr;

	xprintf("	%x", addr);

	if (addr + 2 > amt) {
		xeprintf("All of .exm doesn't fit into %u bytes.\n",
			sizeof(patch_buf));
		exit(1);
		}

	cptr = &patch_buf.one[addr] - 3 + 512;

	if (*cptr != 0x9A || *(cptr + 3) != 0 || *(cptr + 4) != 0) {
		xeprintf("Don't know how to fixup a call at %x.\n", addr);
		exit(1);
		}

	*(cptr + 4) = *(cptr + 2);
	*(cptr + 3) = *(cptr + 1);
	nearfar -= addr;
	*(cptr + 2) = nearfar >> 8;
	*(cptr + 1) = nearfar;
	*cptr = 0xE8;
	}


/* ------------------------------------------------------------ */

/* Update the magic number of code end segment for the .exm file.
Print a message and abort if error.  Return the number of fixups. */

unsigned short
Update(exm_file, codeend)
	char *exm_file;
	unsigned short codeend;
	{
	char fname[FNAMEMAX];
	unsigned char buf[BUFFSIZE];
	int amt;
	int fd;

	xstrcpy(fname, exm_file);
	PSetExt(fname, "EXM", FALSE);

	if ((fd = open(fname, O_RDWR)) < 0) {
		xeprintf("Unable to open exm file '%s'.\n", fname);
		exit(1);
		}

	if ((amt = read(fd, buf, sizeof(buf))) <= 0x1b) {
		xeprintf("Unable to read exm file %s.\n", fname);
		close(fd);
		exit(1);
		}

/* set the new magic number */
	buf[0] = 0x44;
	buf[1] = 0x4c;

/* set the data segment start */
	buf[0x1a] = codeend;
	buf[0x1b] = codeend >> 8;

	lseek(fd, 0L, 0);
	if (write(fd, buf, amt) != amt) {
		xeprintf("Unable to update exm file %s.\n", fname);
		close(fd);
		exit(1);
		}

	close(fd);
	return((buf[7] << 8) + buf[6]);
	}


/* end of MAKEEXM.C -- Make a .EXE File Into a .EXM File */
