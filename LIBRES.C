/* LIBRES.C -- Read Resource File Routines

	Written December 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "lib.h"

static unsigned char resbuf[RESMAXSIZE];

int Res__Index();	/* int where, int offset */

/* ------------------------------------------------------------ */

/* Load and check the resource file.  Return 0 on success or -1 on
error. */

int
Res_Load(fname)
	char *fname;
	{
	int fd;
	int amt;

	if ((fd = open(fname, O_RDONLY)) < 0) return(-1);
	amt = read(fd, resbuf, RESMAXSIZE);
	close(fd);
	
	if (amt <= sizeof(struct res_header) ||
		 ((struct res_header *)resbuf)->significator != RESSIG ||
		 ((struct res_header *)resbuf)->version != RESVERSION)
		return(-1);
	return(0);
	}


/* ------------------------------------------------------------ */

/* Retern the start of the buffer. */

unsigned char *
Res_Buf()
	{
	return(resbuf);
	}


/* ------------------------------------------------------------ */

/* Return the first character in a string, uppercased. TABLE and
OFFSET specify the place to load from.  */

unsigned char
Res_Char(table, offset)
	int table;
	int offset;
	{
	int start;
	int size;

	start = Res_Get(&size, table, offset);
	return(xtoupper(*(resbuf + start)));
	}


/* ------------------------------------------------------------ */

/* Return the value at (table, offset) and its size (in bytes) in
SIZEPTR. */

int
Res_Get(sizeptr, table, offset)
	int *sizeptr;
	int table;
	int offset;
	{
	struct resint *rptr;
	int tbl;
	int start;
	int next;

	tbl = Res__Index(RESTABSTART, table);
	if (table == -1) {
		*sizeptr = 2;
		return(tbl);
		}

	start = Res__Index(tbl, offset);
	if (offset == -1) {
		*sizeptr = 2;
		return(start);
		}
	next = Res__Index(tbl, offset + 1);
	*sizeptr = next - start;
	return(start);
	}


/* ------------------------------------------------------------ */

/* Return the key sequence.  BUF is a keystroke buffer to return the
sequence into and BUFLEN is the size of the buffer in keystrokes.
TABLE and OFFSET specify the place to load from.  Return the number of
keystroes or -1 if it didn't fit. */

int
Res_KeySeq(buf, buflen, table, offset)
	int *buf;
	int buflen;
	int table;
	int offset;
	{
	struct resint *rptr;
	int start;
	int size;
	int cnt;

	start = Res_Get(&size, table, offset);
	size /= sizeof(struct resint);
	if (size > buflen) return(-1);
	rptr = (struct resint *)(resbuf + start);
	for (cnt = 0; cnt < size; cnt++) {
		*buf++ = rptr->high * 256 + rptr->low;
		rptr++;
		}
	return(size);
	}


/* ------------------------------------------------------------ */

/* Return the numeric value.  TABLE and OFFSET specify the place to
load from.  Using an OFFSET of -1 gets you the number of entries.
Using -1, -1 gets you the number of tables. */

int
Res_Number(table, offset)
	int table;
	int offset;
	{
	struct resint *rptr;
	int start;
	int size;

	start = Res_Get(&size, table, offset);
	if (offset == -1) return(start);
	rptr = (struct resint *)(resbuf + start);
	if (size == 1) {
		return(rptr->low);
		}
	else	{
		return(rptr->high * 256 + rptr->low);
		}
	}


/* ------------------------------------------------------------ */

/* Return a pointer to a string.  Place the length of the string in
LENPTR (if not NULL) TABLE and OFFSET specify the place to load from.
*/

unsigned char *
Res_String(lenptr, table, offset)
	int *lenptr;
	int table;
	int offset;
	{
	int start;
	int size;

	start = Res_Get(&size, table, offset);
	if (lenptr != NULL) *lenptr = size;
	return(resbuf + start);
	}


/* ------------------------------------------------------------ */

/* Return the value at entry OFFSET in the table that starts at WHERE. */

int
Res__Index(where, offset)
	int where;
	int offset;
	{
	struct resint *rptr = (struct resint *)(resbuf + where);
	int num = rptr->high *  256 + rptr->low;

	if (offset < -1) offset = -1;
	if (offset > num - 1) offset = num - 1;

	rptr += offset + 1;
	return(rptr->high * 256 + rptr->low);
	}


/* end of LIBRES.C -- Read Resource File Routines */
