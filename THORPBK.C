/* THORPBK.C -- Convert HP95LX Phone Book Files To/From ASCII

	Written September 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "lib.h"

/* -------------------- Phone Book Records -------------------- */

#define PBKNAMEMAX	30
#define PBKNUMBERMAX	30
#define PBKADDRESSMAX	39
#define PBKADDRESSLINES	8

struct pbk_id {
	unsigned char	ProductCodeL;	/* must be -2 (0xFFFe) */
	unsigned char	ProductCodeH;
	unsigned char	ReleaseNumL;	/* must be 1 */
	unsigned char	ReleaseNumH;
	unsigned char	FileType;	/* must be 3 */
	};

struct pbk_data {
	unsigned char	RecordType;	/* must be 1 */
	unsigned char	RecordLengthL;	/* number of bytes after, may be padded */
	unsigned char	RecordLengthH;
	unsigned char	NameLength;	/* length of name field */
	unsigned char	NumberLength;	/* length of number field */
	unsigned char	AddressLengthL;	/* length of address text */
	unsigned char	AddressLengthH;
	unsigned char	data[30 + 30 + 8 * 40];	/* order is name text, number
				text, address text (NUL is line delimiter) */
	};

struct pbk_eof {
	unsigned char	RecordType;	/* must be 2 */
	unsigned char	RecordLengthL;	/* must be 0 */
	unsigned char	RecordLengthH;
	};

/* ------------------------------------------------------------ */

static FLAG opt_from = FALSE;
static FLAG opt_to = FALSE;
static FLAG opt_cc = FALSE;
static FLAG opt_fs = FALSE;
static FLAG opt_d = FALSE;
static FLAG opt_v = FALSE;
static int val_cc = '#';
static int val_fs = ';';
static int val_lines = 0;

static FLAG is_line = TRUE;

static char *from_file = NULL;
static char *to_file = NULL;

FLAG From();		/* void */
char *GetExt();		/* char *name */
FLAG To();		/* void */
int To_Record();	/* struct pbk_data *pdataptr, 
			char *tokens[PBKADDRESSLINES + 2],
			int amt,
			struct parse_data *pdptr */

/* ------------------------------------------------------------ */

int
main(argc,argv)
	int argc;
	char *argv[];
	{
	int cnt;
	FLAG ok;

	if (argc < 3) {
usage:
		xprintf("Phone book conversion program\n\
usage is: thorpbk [options] <from> <to>\n\
options are:\n\
	[-f|from|frompbk] convert from .PBK to ASCII\n\
	[-t|to|topbk]	convert from ASCII to .PBK\n\
\n\
	[-cc|commentchar] <c>	specify the comment character\n\
	[-fs|fieldsep] <c>	specify the field separator\n\
	[-l|lines] <#>		specify the number of lines to pad to\n\
\n\
	[-d|debug]	show debugging information\n\
	[-v|verbose]	show records as processed\n%s", COPY);
		exit(1);
		}

	for (cnt = 1; cnt < argc; ++cnt) {
		if (strequ(argv[cnt], "-f") || strequ(argv[cnt], "-from") ||
			 strequ(argv[cnt], "-frompbk")) {
			opt_from = TRUE;
			}
		else if (strequ(argv[cnt], "-t") || strequ(argv[cnt], "-to") ||
			 strequ(argv[cnt], "-topbk")) {
			opt_to = TRUE;
			}
		else if (strequ(argv[cnt], "-cc") ||
			 strequ(argv[cnt], "-commentchar")) {
			opt_cc = TRUE;
			if (++cnt >= argc) {
				xeprintf("Missing char for -cc option.\n");
				exit(1);
				}
			if (!SToN(argv[cnt], &val_cc, 10)) {
				val_cc = *argv[cnt];
				}
			is_line = FALSE;
			}
		else if (strequ(argv[cnt], "-fs") ||
			 strequ(argv[cnt], "-fieldsep")) {
			opt_fs = TRUE;
			if (++cnt >= argc) {
				xeprintf("Missing char for -fs option.\n");
				exit(1);
				}
			if (!SToN(argv[cnt], &val_fs, 10)) {
				val_fs = *argv[cnt];
				}
			is_line = FALSE;
			}
		else if (strequ(argv[cnt], "-l") ||
			 strequ(argv[cnt], "-lines")) {
			if (++cnt >= argc) {
				xeprintf("Missing number for -lines option.\n");
				exit(1);
				}
			if (!SToN(argv[cnt], &val_lines, 10)) {
				xeprintf("Value '%s' for -lines is not a number.\n",
					argv[cnt]);
				exit(1);
				}
			}
		else if (strequ(argv[cnt], "-d") ||
			 strequ(argv[cnt], "-debug")) {
			opt_d = TRUE;
			}
		else if (strequ(argv[cnt], "-v") ||
			 strequ(argv[cnt], "-verbose")) {
			opt_v = TRUE;
			}
		else if (*argv[cnt] == '-' && *(argv[cnt] + 1) != NUL) {
			xprintf("Unknown option '%s'.\n", argv[cnt]);
			goto usage;
			}
		else if (from_file == NULL) {
			from_file = argv[cnt];
			}
		else	to_file = argv[cnt];
		}

/* check out the invalid combinations */

	if (opt_to && opt_from) {
		xeprintf("You may not supply both -topbk and -frompbk.\n");
		exit(1);
		}

	if (!opt_from && !opt_to) {
		opt_from = strequ(GetExt(from_file), "pbk");
		opt_to =   strequ(GetExt(  to_file), "pbk");
		if (opt_to && opt_from) {
			xeprintf(
"If both names end in .pbk, I can't figure out what direction to go.\n");
			exit(1);
			}
		}

	if (opt_d) {
		xeprintf("from %s, to %s, isline %s, cc '%c', fs '%c'\n",
			opt_from ? "yes" : "no",
			opt_to ? "yes" : "no",
			is_line ? "yes" : "no",
			val_cc,
			val_fs);
		}
	if (opt_to) {
		ok = To();
		}
	else if (opt_from) {
		ok = From();
		}
	else	{
		xeprintf("You have to say either -to or -from.\n");
		exit(1);
		}

	exit(ok ? 0 : 1);
	}


/* ------------------------------------------------------------ */

/* Convert a phone book to an ASCII file.  Return True on success or
False on failure. */

FLAG
From()
	{
	struct pbk_id pi;
	struct pbk_data pd;
	unsigned char buf[sizeof(struct pbk_data)];
	unsigned char *cptr;
	unsigned char *ncptr;
	int ifd;
	int ofd;
	int size;
	int amt;
	int addsize;
	int datasize;
	int where;
	int line;

/* open input */

	if (strequ(from_file, "-")) {
		ifd = 0;
		}
	else if ((ifd = open(from_file, O_RDONLY)) < 0) {
		xeprintf("Unable to open input file '%s'.\n", from_file);
		return(FALSE);
		}
	if ((amt = read(ifd, (char *)&pi, 5)) != 5 ||
		 pi.ProductCodeL != 0xFE ||
		 pi.ProductCodeH != 0xFF ||
		 pi.ReleaseNumL != 1 ||
		 pi.ReleaseNumH != 0 ||
		 pi.FileType != 3) {
		if (opt_v) {
			xprintf("amt %d: %d %d %d %d %d\n", amt,
				pi.ProductCodeL, pi.ProductCodeH,
				pi.ReleaseNumL, pi.ReleaseNumH,
				pi.FileType);
			}
		xeprintf("Input fine %s is not a valid phone book.\n",
			from_file);
		close(ifd);
		return(FALSE);
		}

/* open output */

	if (strequ(to_file, "-")) {
		ofd = 1;
		}
	else if ((ofd = creat(to_file, 0666)) < 0) {
		xeprintf("Unable to create output file '%s'.\n", to_file);
		close(ifd);
		return(FALSE);
		}


/* read the input */

	while ((amt = read(ifd, (char *)&pd, 3)) == 3) {
		if (pd.RecordType == 2) {
			amt = 0;
			break;
			}
		if (pd.RecordType != 1) {
			xeprintf("Unknown phone book record type %d.\n",
				pd.RecordType);
			break;
			}

		size = (pd.RecordLengthH << 8) | pd.RecordLengthL;
		if (size < 4 || size > sizeof(struct pbk_data) - 3) {
			xeprintf("Invalid record size %d.\n", size);
			break;
			}
		if ((amt = read(ifd, 3 + (char *)&pd, size)) != size) {
			break;
			}

		addsize = (pd.AddressLengthH << 8) | pd.AddressLengthL;
		if (opt_v) {
			xprintf("record size %d, na %d, nu %d, ad %d\n",
				size, pd.NameLength, pd.NumberLength, addsize);
			}
		datasize = pd.NameLength + pd.NumberLength + addsize;
		if (datasize < 0 || datasize > size - 4) {
			xeprintf("Internal inconsistency.\n");
			break;
			}

		where = 0;
		line = 1;
		if (is_line) {

			memmove(buf, &pd.data[where], pd.NameLength);
			where += pd.NameLength;
			buf[pd.NameLength] = NUL;
			xdprintf(ofd, "%s\n", buf);
			line++;

			memmove(buf, &pd.data[where], pd.NumberLength);
			where += pd.NumberLength;
			buf[pd.NumberLength] = NUL;
			xdprintf(ofd, "%s\n", buf);
			line++;

			memmove(buf, &pd.data[where], addsize);
			buf[addsize] = NUL;
			for (cptr = buf; cptr < &buf[addsize];
				 cptr += strlen(cptr) + 1) {
				xdprintf(ofd, "%s\n", cptr);
				line++;
				}

			if (val_lines > 0) {
				while (line++ < val_lines) {
					xdprintf(ofd, "\n");
					}
				}
			}
		else	{
			memmove(buf, &pd.data[where], pd.NameLength);
			where += pd.NameLength;
			buf[pd.NameLength] = NUL;
			if (pd.NumberLength == 0 && addsize == 0)
				xdprintf(ofd, "%s", buf);
			else	xdprintf(ofd, "%s%c", buf, val_fs);

			memmove(buf, &pd.data[where], pd.NumberLength);
			where += pd.NumberLength;
			buf[pd.NumberLength] = NUL;
			if (addsize == 0)
				xdprintf(ofd, "%s", buf);
			else	xdprintf(ofd, "%s%c", buf, val_fs);

			memmove(buf, &pd.data[where], addsize);
			buf[addsize] = NUL;
			for (cptr = buf; cptr < &buf[addsize]; cptr = ncptr) {
				ncptr = cptr + strlen(cptr) + 1;
				if (ncptr >= &buf[addsize])
					xdprintf(ofd, "%s", cptr);
				else	xdprintf(ofd, "%s%c", cptr, val_fs);
				}
			}
		xdprintf(ofd, "\n");
		}

/* handle read error */

	if (amt != 0) {
		xeprintf("Read error on input file %s.\n", from_file);
		close(ifd);
		close(ofd);
		return(FALSE);
		}

/* ok */

	close(ifd);
	close(ofd);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the file extension. */

char *
GetExt(name)
	char *name;
	{
	char *cptr;

	for (cptr = name + strlen(name); cptr > name; --cptr) {
		if (*cptr == '.') return(cptr + 1);
#if defined(MSDOS)
		if (*cptr == ':' || *cptr == '/' || *cptr == '\\') break;
#endif
#if defined(UNIX)
		if (*cptr == '/') break;
#endif
		}
	return("");
	}


/* ------------------------------------------------------------ */

/* Convert an ASCII file to a phone book.  Return True on success or
False on failure. */

FLAG
To()
	{
	struct parse_data pd;
	char *tokens[PBKADDRESSLINES + 10];
	char buffer[PARSELENGTH];
	char buf2[BUFFSIZE];
	char buf3[BUFFSIZE];
	struct pbk_id pid;
	struct pbk_data pdata;
	struct pbk_eof peof;
	int amt;
	int amt2;
	int len;
	int tlen;
	int fd;
	FLAG isdone;

/* open input */

	pd.p_buffer = buffer;
	pd.p_length = PARSELENGTH;
	pd.p_separator = val_fs;
	pd.p_comment = val_cc;
	pd.p_trim = FALSE;

	if (Parse_Open(&pd, strequ(from_file, "-") ? "" : from_file) < 0) {
		xeprintf("Unable to open input file '%s'.\n", from_file);
		return(FALSE);
		}

/* open output */

	if (strequ(to_file, "-")) {
		fd = 1;
		}
	else if ((fd = creat(to_file, 0666)) < 0) {
		xeprintf("Unable to create output file '%s'.\n", to_file);
		Parse_Close(&pd);
		return(FALSE);
		}
	pid.ProductCodeL = -2;
	pid.ProductCodeH = -1;
	pid.ReleaseNumL = 1;
	pid.ReleaseNumH = 0;
	pid.FileType = 3;
	if (write(fd, (char *)&pid, 5) != 5) {
		xeprintf("Write error on %s.\n", to_file);
		Parse_Close(&pd);
		close(fd);
		return(FALSE);
		}

/* read it */

	if (!is_line) {	/* record fits on one line */
		while ((amt = Parse_A_Line(&pd, tokens, PBKADDRESSLINES + 10)) > 0) {
			amt2 = To_Record(&pdata, tokens, amt, &pd);
			if (write(fd, (char *)&pdata, amt2) != amt2) {
				xeprintf("Write error on %s.\n", to_file);
				Parse_Close(&pd);
				close(fd);
				return(FALSE);
				}
			}
		}
	else	{		/* one field per line */
		for (isdone = FALSE; !isdone; ) {
			len = 0;
			for (amt = 0; amt < PBKADDRESSLINES + 10 && !isdone;
				 amt++) {
				tokens[amt] = &buf3[len];
				*tokens[amt] = NUL;

				if (Parse_Get_Line(&pd, buf2, sizeof(buf2)) < 0) {
					isdone = TRUE;
					*buf2 = NUL;
					}
				if (*buf2 == NUL) break;

				tlen = strlen(buf2);
				if (len + tlen + 1 >= sizeof(buf3)) {
					xeprintf(
"%s line %d: record too big to handle.\n", Parse_File(&pd), Parse_Line(&pd));
					break;
					}
				memmove(tokens[amt], buf2, tlen);
				len += tlen;
				buf3[len++] = NUL;
				}

			amt2 = To_Record(&pdata, tokens, amt, &pd);
			if (write(fd, (char *)&pdata, amt2) != amt2) {
				xeprintf("Write error on %s.\n", to_file);
				Parse_Close(&pd);
				close(fd);
				return(FALSE);
				}
			}
		}
	Parse_Close(&pd);

/* handle read error */

	if (amt < 0) {
		xeprintf("Read error on file %s on or after line %d.\n",
			Parse_File(&pd), Parse_Line(&pd));
		close(fd);
		return(FALSE);
		}

/* ok */

	peof.RecordType = 2;
	peof.RecordLengthL = 0;
	peof.RecordLengthH = 0;
	if (write(fd, (char *)&peof, 3) != 3) {
		xeprintf("Write error on %s.\n", to_file);
		close(fd);
		return(FALSE);
		}
	close(fd);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Put the tokens into the data record and return the number of bytes
packed. */

int
To_Record(pdataptr, tokens, amt, pdptr)
	struct pbk_data *pdataptr;
	char *tokens[PBKADDRESSLINES + 2];
	int amt;
	struct parse_data *pdptr;
	{
	int cnt;
	int len;
	int tlen;
	int alen;

	pdataptr->RecordType = 1;
	len = 0;

/* name */

	if (strlen(tokens[0]) > PBKNAMEMAX) {
		xeprintf("%s line %d: warning: name '%s' is > %d chars.\n",
			Parse_File(pdptr), Parse_Line(pdptr),
			tokens[0], PBKNAMEMAX);
		tokens[0][PBKNAMEMAX] = NUL;
		}
	tlen = strlen(tokens[0]);
	if (opt_v) xprintf("%d  %d '%s'\n", 0, tlen, tokens[0]);
	pdataptr->NameLength = tlen;
	memmove(&pdataptr->data[len], tokens[0], tlen);
	len += tlen;

/* number */

	if (amt <= 1) tokens[0][0] = NUL;
	if (strlen(tokens[1]) > PBKNUMBERMAX) {
		xeprintf("%s line %d: warning: number '%s' is > %d chars.\n",
			Parse_File(pdptr), Parse_Line(pdptr),
			tokens[1], PBKNUMBERMAX);
		tokens[1][PBKNUMBERMAX] = NUL;
		}
	tlen = strlen(tokens[1]);
	if (opt_v) xprintf("%d  %d '%s'\n", 1, tlen, tokens[1]);
	pdataptr->NumberLength = tlen;
	memmove(&pdataptr->data[len], tokens[1], tlen);
	len += tlen;

/* address lines */

	alen = 0;
	if (amt > PBKADDRESSLINES + 2) {
		xeprintf("%s line %d: warning: more than %d address lines.\n",
			Parse_File(pdptr), Parse_Line(pdptr), PBKADDRESSLINES);
		amt = PBKADDRESSLINES + 2;
		}

	for (cnt = 2; cnt < amt; cnt++) {
		if (strlen(tokens[cnt]) > PBKADDRESSMAX) {
			xeprintf(
"%s line %d: warning: address line '%s' is > %d chars.\n",
				Parse_File(pdptr), Parse_Line(pdptr),
				tokens[cnt], PBKADDRESSMAX);
			tokens[cnt][PBKADDRESSMAX] = NUL;
			}
		tlen = strlen(tokens[cnt]);
		if (opt_v) xprintf("%d  %d '%s'\n", cnt, tlen, tokens[cnt]);
		memmove(&pdataptr->data[len], tokens[cnt], tlen);
		len += tlen;
		pdataptr->data[len++] = NUL;
		alen += tlen + 1;
		}
	if (alen > 0) {
		alen--;
		len--;
		}

	pdataptr->AddressLengthL = alen;
	pdataptr->AddressLengthH = alen >> 8;
	if (opt_v) xprintf("\n");

	len += 4;
	pdataptr->RecordLengthL = len;
	pdataptr->RecordLengthH = len >> 8;

	return(len + 3);
	}


/* end of THORPBK.C -- Convert HP95LX Phone Book Files To/From ASCII */
