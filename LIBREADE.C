/* LIBREADE.C -- File Tokenizer Routines

	Written July 1988 by Craig Finseth
	Copyright 1988,9,90,1,2,3 by Craig A. Finseth

This file implements a basic file tokenizer.  Basic operation is:

	#include "lib.h"

	char fname[];
	FLAG done;
	char buffer[TOKENMAX];

	if (!Read_Init(fname)) {		/ * initialize * /
		xprintf("Cannot open the file '%s'.\n", fname);
		return(FALSE);
		}
	...
	done = FALSE;
	while (!done) {			/ * while there is data * /
		switch (Read_Token(buffer)) {

		case READNONE:		/ * eof or read error * /
		case READEOF:
			...
			done = TRUE;
			break;

		case READSTR:			/ * quoted string * /
			...
			break;

		case READTOK:			/ * token * /
			...
			break;
			}
		}
	...
	Read_Fini();				/ * always safe to call * /

Tokenizing:

	- The high order bit (2^7) is ignored during input.

	- Quoted strings must start and end with the same character
	(defined as QUO in the parse table).

	- Comments start with a comment character and continue until
	the next LF.  Comment characters within quoted strings do not
	start comments.

	- There is a two token deep "untoken" buffer.  Any token --
	including EOF -- may be pushed back.

	- Once an EOF has been returned, Read_Token will keep returning
	EOF until a Read_Fini / Read_Init sequence.

	- Read_Fini may always be safely called.

	- Read_File and Read_Line return the file name and line number
	for error reporting purposes.

	- You can do the sequence:

		Read_Init("");
		Read_Pipe(fptr);	/ * FILE *fptr * /

	to have it read from standard input (using fread) instead of a
	file (using read).

	- The meanings of characters are determined by the parse table.

	- Quoted strings and tokens may be at most TOKENMAX - 1
	characters long.  Warning messages are printed if a token or
	string is too long.

	- Quoted strings may not include newlines.  Backslash is used
	for special characters:

		\ NEWLINE	splices the next line onto this one
		\"		means "
		\'		means '
		\\		means \
		\a, \A	means Bell		^G	7 decimal
		\b, \B	means Back Space	^H	8 decimal
		\f, \F	means Form Feed		^L	12 decimal
		\l, \L	means Line Feed		^J	10 decimal
		\n, \N	means Newline, same as \L
		\r, \R	means Carriage Ret.	^M	13 decimal
		\t, \T	means Tab		^I	9 decimal
		\v, \V	means Vertical Tab	^K	11 decimal
		\x##,	means the char. with the specified hexadecimal value
		 \X##
		\###	means the character with the specified octal value
*/

#include "lib.h"

	/* parse table */

enum parse_type {
	BRK,	/* automatic break:  the character will be returned as a
		separate token. */
	FLU,	/* flush:  the chara. will be discarded (but not in strings) */
	MRG,	/* automatic break, but sucessive ones are merged */
	QUO,	/* character is a string quote character (" or ') -- update
		Read__Slash if you change these. */
	TOK,	/* token:  successive token characters make up a token */
	WHI };	/* whitespace */

static enum parse_type parse[] = {
/*	NUL   SOH   STX   ETX   EOT   ENQ   ACK   BEL */
	FLU,  FLU,  FLU,  FLU,  FLU,  FLU,  FLU,  FLU,
/*	BS    HT    LF    VT    FF    CR    SO    SI */
	WHI,  WHI,  WHI,  WHI,  WHI,  WHI,  FLU,  FLU,
/*	DLE   DC1   DC2   DC3   DC4   NAK   SYN   ETB */
	FLU,  FLU,  FLU,  FLU,  FLU,  FLU,  FLU,  FLU,
/*	CAN   EM    SUB   ESC   FS    GS    RS    US */
	FLU,  FLU,  FLU,  FLU,  FLU,  FLU,  FLU,  FLU,
/*	SP    !     "     #     $     %     &     ' */
	WHI,  TOK,  QUO,  TOK,  TOK,  TOK,  TOK,  QUO,
/*	(     )     *     +     ,     -     .     / */
	TOK,  TOK,  TOK,  TOK,  BRK,  TOK,  TOK,  TOK,
/*	0     1     2     3     4     5     6     7 */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	8     9     :     ;     <     =     >     ? */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	@     A     B     C     D     E     F     G */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	H     I     J     K     L     M     N     O */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	P     Q     R     S     T     U     V     W */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	X     Y     Z     [     \     ]     ^     _ */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	`     a     b     c     d     e     f     g */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	h     i     j     k     l     m     n     o */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	p     q     r     s     t     u     v     w */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,
/*	x     y     z     {     |     }     ~     DEL */
	TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  TOK,  FLU };

static int fd;			/* input file descriptor */
static int line;		/* line number */
static int len;			/* amount of data in buffer */
static long off;		/* file offset */
static char *fcptr;		/* buffer pointer */
static char filebuf[BUFFSIZE];
static char fname[FNAMEMAX];

static struct {
	char fname[FNAMEMAX];
	long offset;
	int line;
	int fd;
	FLAG is;
	} saved = { { NUL }, 0L, 0, 0, FALSE };

#define DEFAULT_COMMENT		NUL
static char comment = DEFAULT_COMMENT;	/* comment character */

static char untok1[TOKENMAX];	/* ungotten token */
static char untok2[TOKENMAX];	/* ungotten token */

static FILE *fptr;	/* for Read_Pipe */

int Read__Conv();	/* const int base, int number */
int Read__Get();	/* void */
int Read__Len();	/* char *buffer */
void Read__Pop();	/* void */
int Read__Slash();	/* void */
void Read__Unget();	/* const int c */

/* ------------------------------------------------------------ */

/* Open the input file and initialize local variables. Return TRUE on
success or FALSE on error.  If FILE is "", use standard input as the
input file. */

FLAG
Read_Init(file)
	char *file;
	{
	fd = -1;
	line = 1;
	comment = DEFAULT_COMMENT;
	*untok1 = (char)READNONE;
	*untok2 = (char)READNONE;
	fptr = NULL;
	off = 0;

	if (*file == NUL) {
		fd = 0;
		}
	else	{
		if ((fd = open(file, O_RDONLY)) < 0) return(FALSE);
		}
	xstrcpy(fname, file);
	len = 0;
	fcptr = &filebuf[1];
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Terminate the reading. */

void
Read_Fini()
	{
	if (fd != 0 && fd != -1) close(fd);
	fd = -1;
	}


/* ------------------------------------------------------------ */

/* Set the comment character. */

void
Read_Comment(c)
	char c;
	{
	comment = c;
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the file name. */

char *
Read_File()
	{
	return(fname);
	}


/* ------------------------------------------------------------ */

/* Return the current line number. */

int
Read_Line()
	{
	return(line);
	}


/* ------------------------------------------------------------ */

/* Set the file descriptor. */

void
Read_Pipe(f)
	FILE *f;
	{
	fptr = f;
	}


/* ------------------------------------------------------------ */

/* Do an include. */

FLAG
Read_Push(file)
	char *file;
	{
	xstrcpy(saved.fname, fname);
	saved.line = line;
	saved.fd = fd;
	saved.offset = off;
	saved.is = TRUE;
	return(Read_Init(file));
	}


/* ------------------------------------------------------------ */

/* Read and return the next token.  Return its type (READEOF, READSTR,
READTOK).  The buffer must be at least TOKENMAX characters long. If it
is a value, return it in BUF. */

enum ReadType
Read_Token(buf)
	char *buf;
	{
	int c;
	char *cptr;
	char openstr;
	enum parse_type toktype;

	if (*untok1 != (char)READNONE) {
		if (*untok2 != (char)READNONE) cptr = untok2;
		else cptr = untok1;

		memmove(buf, cptr, Read__Len(cptr));
		*cptr = (char)READNONE;
		return((enum ReadType)*buf);
		}

	if (fd == -1) {
		*buf = (char)READEOF;
		return(READEOF);
		}

		/* skip leading whitespace */
	while ((c = Read__Get()) != -1 &&
		 (parse[c & 0x7F] == WHI || c == comment)) {
		if (c == comment) {			/* skip comment */
			while ((c = Read__Get()) != -1 && c != LF) ;
			if (c == -1) {
				*buf = (char)READEOF;
				return(READEOF);
				}
			}
		}
	if (c == -1) {
		*buf = (char)READEOF;
		return(READEOF);
		}

/* read in token */

	if (parse[c & 0x7F] == QUO) {		/* read in a quoted string */
		openstr = c;
		buf[0] = (char)READSTR;
		cptr = ++buf;
		while ((c = Read__Get()) != -1) {
			if (cptr - buf > TOKENMAX - 1) {
				xprintf(
"Quoted string too long (over %d characters).\n\
String was on line %d of file %s.\n",
					TOKENMAX - 2, line, fname);
				break;
				}
			if (c == '\\') {
				if ((c = Read__Slash()) == -1) {
					*cptr++ = '\\';
					break;
					}
				else if (c != -2) *cptr++ = c;
				}
			else if (c == openstr) break;
			else if (c == CR || c == LF) {
				xprintf(
"Quoted string contains newline.\n\
String was on line %d of file %s.\n",
					line, fname);
				}
			else *cptr++ = c;
			}
		*cptr = NUL;
		return(READSTR);
		}

/* read in a token */

	buf[0] = (char)READTOK;
	cptr = ++buf;
	while (TRUE) {
		toktype = parse[c & 0x7F];
		if (toktype != FLU) break;
		if ((c = Read__Get()) == -1) {
			buf[0] = (char)READEOF;
			return(READEOF);
			}
		}
	if (toktype == BRK) {		/* break character */
		*cptr++ = c;
		*cptr = NUL;
		return(READTOK);
		}

	while (cptr - buf < TOKENMAX - 1 && parse[c & 0x7F] == toktype) {
		*cptr++ = c;
		if (cptr - buf >= TOKENMAX - 1) {
			xprintf(
"Token too long (over %d characters).\n\
Token was on line %d of file %s.\n", TOKENMAX - 2, line, fname);
			*cptr = NUL;
			return(READTOK);
			}
		while (TRUE) {
			if ((c = Read__Get()) == -1) {
				*cptr = NUL;
				return(READTOK);
				}
			if (parse[c & 0x7F] != FLU) break;
			}
		}
	Read__Unget(c);
	*cptr = NUL;
	return(READTOK);
	}


/* ------------------------------------------------------------ */

/* Unget a token -- buffer is two tokens deep. */

void
Read_Untok(buf)
	char *buf;
	{
	memmove(*untok1 == (char)READNONE ? untok1 : untok2,
		buf, Read__Len(buf));
	}


/* ------------------------------------------------------------ */

/* Read at most NUMBER digits in base BASE, convert them to a number
and return the number or -1.  BASE must be 8, 10 or 16. */

int
Read__Conv(base, number)
	int base;
	int number;
	{
	char val;
	int c;

	val = 0;
	while (number-- > 0) {
		if ((c = Read__Get()) == -1) return(val);
		c = xtoupper(c);
		if (base == 8) {
			if (c < '0' || c > '7') {
				Read__Unget(c);
				return(val & 0xFF);
				}
			val *= 8;
			val += c - '0';
			}
		else if (base == 10) {
			if (c < '0' || c > '9') {
				Read__Unget(c);
				return(val & 0xFF);
				}
			val *= 10;
			val += c - '0';
			}
		else	{
			if (!xisxdigit(c)) {
				Read__Unget(c);
				return(val & 0xFF);
				}
			val *= 16;
			if (xisdigit(c))
				val += c - '0';
			else val += xtoupper(c) - 'A' + 10;
			}
		}
	return(val & 0xFF);
	}


/* ------------------------------------------------------------ */

/* Return the next character from input file or -1 on EOF. */

int
Read__Get()
	{
	if (fd == -1) return(-1);
	if (fcptr >= &filebuf[len]) {
		if (fptr != NULL)
			len = fread(filebuf, 1, sizeof(filebuf), fptr);
		else	len = read(fd, filebuf, sizeof(filebuf));
		if (len <= 0) {
			close(fd);
			fd = -1;
			if (saved.is) {
				Read__Pop();
				return(Read__Get());
				}
			else	return(-1);
			}
		fcptr = filebuf;
		}
	if (*fcptr == LF) {
		line++;
		}
	off++;
	return(*fcptr++ & 0xFF);
	}


/* ------------------------------------------------------------ */

/* Return the length of the token. */

int
Read__Len(buffer)
	char *buffer;
	{
	switch (*buffer++) {

	case READNONE:
	case READEOF:
		return(1);
		/* break; */

	case READSTR:
	case READTOK:
		return(1 + strlen(buffer) + 1);
		/* break; */
		}
	return(0);		/* the compiler couldn't figure this one out */
	}


/* ------------------------------------------------------------ */

/* Pop a file off the stack. */

void
Read__Pop()
	{
	xstrcpy(fname, saved.fname);
	line = saved.line;
	fd = saved.fd;

	off = saved.offset;
	lseek(fd, off, 0);
	len = -1;
	fcptr = filebuf;
	saved.is = FALSE;
	}


/* ------------------------------------------------------------ */

/* Read the characters after a backslash and return the converted
character or -1 on EOF or -2 on newline. */

int
Read__Slash()
	{
	int c;

	if ((c = Read__Get()) == -1) return(-1);
	c = xtoupper(c);

	if (c == CR) {
#if defined(MSDOS)
		Read__Get();
#endif
		return(-2);
		}
	else if (c == NL) return(-2);
	else if (c == '"') return('"');
	else if (c == '\'') return('\'');
	else if (c == '\\') return('\\');
	else if (c == 'A') return(BEL);
	else if (c == 'B') return(BS);
	else if (c == 'F') return(FF);
	else if (c == 'L') return(LF);
	else if (c == 'N') return(LF);
	else if (c == 'R') return(CR);
	else if (c == 'T') return(TAB);
	else if (c == 'V') return(ZCK);
	else if (c == 'X') return(Read__Conv(16, 2));
	else if (xisdigit(c)) {
		Read__Unget(c);
		return(Read__Conv(8, 3));
		}
	else return(c);
	}


/* ------------------------------------------------------------ */

/* Unget character C -- one character only. */

void
Read__Unget(c)
	int c;
	{
	if (c == LF) line--;
	--fcptr;
	--off;
	}

/* end of LIBREADE.C -- File Tokenizer Routines */
