/* THORRES.C -- Compile Resource Files

	Written December 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "lib.h"

#if defined(MSDOS)
#define STACKSIZE	(10240 / 16);
int _stklen = STACKSIZE;
#endif

/* ------------------------------------------------------------ */

static FLAG opt_dump = FALSE;
static FLAG opt_gc = FALSE;
static FLAG opt_gl = FALSE;
static FLAG opt_new = FALSE;
static FLAG opt_v = FALSE;
static char val_for = NUL;
static char *val_codepage = NULL;
static char *val_header = NULL;
static char *val_language = NULL;
static char *val_o = NULL;

static char *in_file = NULL;

static char hfile[FNAMEMAX];	/* header file */
static char rfile[FNAMEMAX];	/* resource file */

static struct {
	char xx[2];	/* alias */
	FLAG found;	/* has it been listed? */
	} aliases[256] = { 0 };

static char fontmap[256 * 3] =  { 0 };
static char *fontptr = NULL;

/* header file data area */

#define HEADERFIELDS	6
static int hfd;
static struct parse_data hpd;
static char hparsebuffer[PARSELENGTH];

/* Loop state machine */

static enum states {
	EXPTABLE,	/* expecting a table/menu keyword */
	EXPTABNAME,	/* expecting a table name */
	EXPENTNAME,	/* expecting an entry name */
	EXPENTVAL,	/* expecting an entry value */
	EXPENTKEYVAL,	/* expecting an entry key sequence value */
	EXPENTLIST8,	/* expecting an entry list 8 value */
	EXPENTLIST16,	/* expecting an entry list 16 value */
	SKIPENTVAL,	/* skipping an entry value */
	SKIPLIST,	/* skipping an entry list value */
	EXPINCLUDE,	/* expecting an include string */
	EXPGLYPH1,	/* expecting glyph # */
	EXPGLYPH2,	/* expecting glyph abbrev */
	EXPGLYPH3,	/* expecting glyph line 0 */
	EXPGLYPH4,	/* expecting glyph line 1 */
	EXPGLYPH5,	/* expecting glyph line 2 */
	EXPGLYPH6,	/* expecting glyph line 3 */
	EXPGLYPH7,	/* expecting glyph line 4 */
	EXPGLYPH8,	/* expecting glyph line 5 */
	} state = EXPTABLE;

static char *state_names[] = {
	"EXPTABLE",
	"EXPTABNAME",
	"EXPENTNAME",
	"EXPENTVAL",
	"EXPENTKEYVAL",
	"EXPENTLIST8",
	"EXPENTLIST16",
	"SKIPENTVAL",
	"SKIPLIST",
	"EXPINCLUDE",
	"EXPGLYPH1",
	"EXPGLYPH2",
	"EXPGLYPH3",
	"EXPGLYPH4",
	"EXPGLYPH5",
	"EXPGLYPH6",
	"EXPGLYPH7",
	"EXPGLYPH8",
	};

static FLAG got_value = FALSE;
static FLAG first_list;

/* ouput file */

static int ofd;
static char out_buf[RESMAXSIZE];	/* output buffer */
static char *out_ptr = &out_buf[sizeof(struct res_header)];

static int cur_entry = 0;
static int cur_table = -1;
static int max_entry = 0;
static int tot_entries = 0;
static struct resint work_table[RESMAXENTRIES + 1];

/* registry */

#define REGNUM	512		/* number of symbols */
#define REGSIZE	16		/* size of a symbol */

static int reg_num = 0;
static struct {
	char name[REGSIZE + 1];
	int value;
	} registry[REGNUM];

void Dump();		/* void */
int Find_Alias();	/* char *str */
FLAG Fix_String();	/* char *str */
int Font_String();	/* char *str */
FLAG HInit();		/* void */
void HFini();		/* void */
FLAG HLabel();		/* char *label, int value, char *type */
FLAG Loop();		/* void */
int LoopGlyph();	/* int which, int type, char *buf */
int LoopList();		/* int which, char *buf */
int LoopStr();		/* char *buf */
int LoopTok();		/* char *buf */
FLAG OInit();		/* void */
FLAG OFini();		/* void */
FLAG OData();		/* int type, int num */
int ONewEntry();	/* void */
int ONewTable();	/* void */
FLAG ONum();		/* int type, int num */
FLAG OPut();		/* char *buf, int bufsize */
FLAG RegLookup();	/* int *numptr, char *buf */
FLAG RegEntry();	/* char *buf */
FLAG RegTable();	/* char *buf, char *which */
FLAG Select();		/* FLAG *skipptr, char *buf */
int ToKey();		/* char *buf */

/* ------------------------------------------------------------ */

int
main(argc, argv)
	int argc;
	char *argv[];
	{
	int cnt;
	FLAG ok;

	if (argc < 3) {
usage:
		xprintf("Resource file compilation program\n\
usage is: thorres [options] <source>\n\
options are:\n\
	[-c|codepage] <what> specify a code page\n\
	[-dump]		dump an existing file\n\
	[-f|for] <what>	configuration to convert for\n\
	[-gc|glyphcheck] check glyph abbrevs to see if they were defined\n\
	[-gl|glyphlist] list glyph abbrevs as encountered\n\
	[-h|header] <file> specifies an alternate header file\n\
	[-l|language] <what> specify a language\n\
	[-new]		build a new header file\n\
	[-o] <file>	specifies an alternate output file\n\
	[-v|verbose]	be verbose about what you are doing\n%s", COPY);
		exit(1);
		}

	for (cnt = 1; cnt < argc; ++cnt) {
		if (strequ(argv[cnt], "-c") ||
			  strequ(argv[cnt], "-codepage")) {
			if (++cnt >= argc) {
				xeprintf("Missing value for -c option.\n");
				exit(1);
				}
			val_codepage = argv[cnt];
			}
		else if (strequ(argv[cnt], "-dump")) {
			opt_dump = TRUE;
			}
		else if (strequ(argv[cnt], "-f") ||
			  strequ(argv[cnt], "-for")) {
			if (++cnt >= argc) {
				xeprintf("Missing value for -for option.\n");
				exit(1);
				}
			val_for = xtolower(*argv[cnt]);
			}
		else if (strequ(argv[cnt], "-gc") ||
			  strequ(argv[cnt], "-glyphcheck")) {
			opt_gc = TRUE;
			}
		else if (strequ(argv[cnt], "-gl") ||
			  strequ(argv[cnt], "-glyphlist")) {
			opt_gl = TRUE;
			}
		else if (strequ(argv[cnt], "-h") ||
			  strequ(argv[cnt], "-header")) {
			if (++cnt >= argc) {
				xeprintf("Missing value for -h option.\n");
				exit(1);
				}
			val_header = argv[cnt];
			}
		else if (strequ(argv[cnt], "-l") ||
			  strequ(argv[cnt], "-language")) {
			if (++cnt >= argc) {
				xeprintf("Missing value for -l option.\n");
				exit(1);
				}
			val_language = argv[cnt];
			}
		else if (strequ(argv[cnt], "-new")) {
			opt_new = TRUE;
			}
		else if (strequ(argv[cnt], "-o")) {
			if (++cnt >= argc) {
				xeprintf("Missing value for -o option.\n");
				exit(1);
				}
			val_o = argv[cnt];
			}
		else if (strequ(argv[cnt], "-v") ||
			 strequ(argv[cnt], "-verbose")) {
			opt_v = TRUE;
			}
		else if (*argv[cnt] == '-' && *(argv[cnt] + 1) != NUL) {
			xprintf("Unknown option '%s'.\n", argv[cnt]);
			goto usage;
			}
		else	in_file = argv[cnt];
		}

/* check out the invalid combinations */

	if (opt_dump && opt_new) {
		xeprintf("You may not supply both -dump and -new.\n");
		exit(1);
		}

	if (in_file == NULL) {
		xeprintf("You must specify an input file.\n");
		exit(1);
		}

	if (!opt_dump && val_for == NUL) {
		xeprintf("You must use -f to specify a system to compile for.\n");
		exit(1);
		}

/* fill in defaults */

	if (val_header == NULL) {
		xstrncpy(hfile, in_file);
		PSetExt(hfile, "h", TRUE);
		}
	else	xstrncpy(hfile, val_header);

	if (val_o == NULL) {
		xstrncpy(rfile, in_file);
		PSetExt(rfile, "ri", TRUE);
		}
	else	xstrncpy(rfile, val_o);

	if (opt_dump) {
		Dump();
		exit(1);
		}
	else	{
		exit(Loop() ? 0 : 1);
		}
	}


/* ------------------------------------------------------------ */

/* Dump an existing resource file. */

void
Dump()
	{
	int num_tables;
	int table;
	int num_entries;
	int entry;
	int start;
	int size;
	int cnt;
	unsigned char *resbuf = Res_Buf();

	if (Res_Load(rfile) < 0) {
		xeprintf("Unable to load resource file %s\n", rfile);
		return;
		}

	if (opt_v) {
		xprintf("%d: %02x significator\n", 0, resbuf[0] & 0xFF);
		xprintf("%d: %02x version\n", 1, resbuf[1] & 0xFF);
		xprintf("%d: %02x %02x\t%d num tables\n", 2, resbuf[2] & 0xFF,
				resbuf[3] & 0xFF,
				(resbuf[3] << 8) + resbuf[2]);

		for (cnt = 4; cnt < sizeof(struct res_header); cnt += 2) {
			xprintf("%d / table %d: %02x %02x\t%d\n", cnt,
				(cnt / 2) - 2,
				resbuf[cnt] & 0xFF,
				resbuf[cnt + 1] & 0xFF,
				(resbuf[cnt + 1] << 8) + resbuf[cnt]);
			}
		}

	num_tables = Res_Number(-1, -1);
	for (table = 0; table < num_tables; table++) {
		xprintf("\n========================================\n\
table %d of %d:\n\n", table, num_tables);

		num_entries = Res_Number(table, -1);
		for (entry = 0; entry < num_entries - 1; entry++) {
			start = Res_Get(&size, table, entry);
			if (size == 2) {
				xprintf(
"entry %d of %d  start %4d  length   2  %02x %02x  #%d\n", entry, num_entries,
					start,
					(resbuf + start)[0] & 0xff,
					(resbuf + start)[1] & 0xff,
					(resbuf + start)[1] * 256 +
						(resbuf + start)[0]);
				}
			else	{
				xprintf("entry %d of %d  start %4d  length %3d ",
					entry, num_entries, start, size);
				for (cnt = 0; cnt < size; cnt++) {
					xprintf(" %02x",
						(resbuf + start)[cnt]);
					}
				xprintf("\n");
				}
			}
		}
	}


/* ------------------------------------------------------------ */

/* Find the specified alias.  Return its index or -1 on not found. */

int
Find_Alias(str)
	char *str;
	{
	int cnt;

	for (cnt = 255; cnt >= 0; cnt--) {
		if (*str == aliases[cnt].xx[0] &&
			*(str + 1) == aliases[cnt].xx[1]) break;
		}
	if (cnt >= 0) {
		if (opt_gl) {
			xprintf("alias %c%c encountered\n", *str, *(str + 1));
			aliases[cnt].found = TRUE;
			}
		}
	else	{
		if (opt_gc) {
			xprintf("alias %c%c encountered, but no glyph\n",
				*str, *(str + 1));
			}
		}
	return(cnt);		
	}


/* ------------------------------------------------------------ */

/* Check for and handle the four ^ special forms. */

FLAG
Fix_String(str)
	char *str;
	{
	int amt;
	int len;
	char *cptr;

	for (cptr = str; *cptr != NUL; cptr++) {
		if (*cptr != '^') continue;

		switch (*++cptr) {

		case '$':
			if (val_codepage == NULL) {
				xeprintf("%s line %d: ^$ but no -c option.\n",
					Read_File(&hpd), Read_Line(&hpd));
				return(FALSE);
				}
			amt = (TOKENMAX - 2) - (strlen(str) - 2);
			len = strlen(val_codepage);
			if (len > amt) len = amt;	/* not too much data */
			amt = strlen(cptr) + 1 - 1;
			memmove(cptr - 1 + len, cptr + 1, amt);
			memmove(cptr - 1, val_codepage, len);
			cptr += len;
			break;

		case '%':
			if (val_language == NULL) {
				xeprintf("%s line %d: ^$ but no -l option.\n",
					Read_File(&hpd), Read_Line(&hpd));
				return(FALSE);
				}
			amt = (TOKENMAX - 2) - (strlen(str) - 2);
			len = strlen(val_language);
			if (len > amt) len = amt;	/* not too much data */
			amt = strlen(cptr) + 1 - 1;
			memmove(cptr - 1 + len, cptr + 1, amt);
			memmove(cptr - 1, val_language, len);
			cptr += len;
			break;

		case '&':
			memmove(cptr - 1, cptr, strlen(cptr) + 1);
			--cptr;
			*cptr = val_for;
			break;

		case '/':
			if (strlen(cptr + 1) < 2) {
				xeprintf("%s line %d: missing alias for ^/.\n",
					Read_File(&hpd), Read_Line(&hpd));
				return(FALSE);
				}
			amt = Find_Alias(cptr + 1);
			if (amt < 0) {	/* not found */
				xstrcpy(cptr - 1, cptr + 1);
				}
			else	{
				*(cptr - 1) = amt;
				xstrcpy(cptr, cptr + 3);
				}				
			break;

		case '?':
			if (strlen(cptr + 1) < 2) {
				xeprintf("%s line %d: missing aliase for ^?.\n",
					Read_File(&hpd), Read_Line(&hpd));
				return(FALSE);
				}
			amt = Find_Alias(cptr + 1);
			if (amt < 0) {	/* not found */
				xstrcpy(cptr - 1, cptr + 3);
				cptr -= 2;
				}
			else	{
				*(cptr - 1) = amt;
				xstrcpy(cptr, cptr + 3);
				}				
			break;
			}
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Validate and convert a font line. Return font data as 0-15 or -1 on
error. */

int
Font_String(str)
	char *str;
	{
	int val = 0;

	if (strlen(str) != 4) {
		xeprintf("%s line %d: glyph line '%s' is not 4 characters long.\n",
			Read_File(), Read_Line(), str);
		return(-1);
		}
	while (*str != NUL) {
		val = (val << 1) | *str++ != SP;
		}
	return(val);
	}


/* ------------------------------------------------------------ */

/* Initialize the header file, as appropriate. Return True on success
or print a message and return False on failure. */

FLAG
HInit()
	{
	if (opt_new) {
#if defined(MSDOS)
		if ((hfd = creat(hfile, 0666)) < 0) {
#endif
#if defined(UNIX)
		if ((hfd = open(hfile, O_WRONLY | O_TRUNC | O_CREAT,
			 0666)) < 0) {
#endif
			xeprintf("Cannot create header file '%s'.\n", hfile);
			return(FALSE);
			}
		}
	else	{
		hpd.p_buffer = hparsebuffer;
		hpd.p_length = sizeof(hparsebuffer);
		hpd.p_separator = SP;
		hpd.p_comment = NUL;
		hpd.p_trim = TRUE;
		if (Parse_Open(&hpd, hfile) < 0) {
			xeprintf("Cannot open header file '%s'.\n", hfile);
			return(FALSE);
			}
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Close up the header file, as appropriate. */

void
HFini()
	{
	if (opt_new) {
		close(hfd);
		}
	else	{
		Parse_Close(&hpd);
		}
	}


/* ------------------------------------------------------------ */

/* Check or put a label in the header file, as appropriate. Return
True on success or print a message and return False on failure. */

FLAG
HLabel(label, value, type)
	char *label;
	int value;
	char *type;
	{
	char buf[TOKENMAX];
	char *tokens[HEADERFIELDS + 1];

	if (opt_new) {
		xsprintf(buf, "#define\tRES_%s\t%d\t/* %s */\n",
			label, value, type);
		if (write(hfd, buf, strlen(buf)) != strlen(buf)) {
			xeprintf("%s line %d: write error on header file.\n",
				Read_File(), Read_Line());
			return(FALSE);
			}
		return(TRUE);
		}
	else	{
		if (Parse_A_Line(&hpd, tokens, HEADERFIELDS + 1) !=
			 HEADERFIELDS) {
#define	RES_CONF	0	/* table */

			xeprintf(
"%s line %d: header file is misformatted, use -new to rebuild.\n",
				Parse_File(&hpd), Parse_Line(&hpd));
			return(FALSE);
			}
		if (strcmp(tokens[0], "#define") != 0 ||
			 strncmp(tokens[1], "RES_", 4) != 0 ||
			 strcmp(tokens[3], "/*") != 0 ||
			 strcmp(tokens[5], "*/") != 0) {
			xeprintf(
"%s line %d: header file is misformatted, use -new to rebuild.\n",
				Parse_File(&hpd), Parse_Line(&hpd));
			return(FALSE);
			}
		xsprintf(buf, "%d", value);
		if (strcmp(tokens[1] + 4, label) != 0 ||
			 strcmp(tokens[2], buf) != 0 ||
			 strcmp(tokens[4], type) != 0) {
			xeprintf(
"%s line %d: header file has %s %s %s and label is %s %s %s\n",
				Parse_File(&hpd), Parse_Line(&hpd),
				tokens[1] + 4, tokens[2], tokens[4],
				label, buf, type);
			return(FALSE);
			}
		return(TRUE);
		}
	}


/* ------------------------------------------------------------ */

/* Do the main loop. Return True on success or print a message and
return False on failure. */

FLAG
Loop()
	{
	int done;
	char buffer[3 * TOKENMAX];

	if (!Read_Init(in_file)) {
		xeprintf("Cannot open the source file '%s'.\n", in_file);
		return(FALSE);
		}
	if (!HInit()) return(FALSE);
	if (!OInit()) return(FALSE);

	Read_Comment('#');
	done = 0;
	while (done == 0) {
		switch (Read_Token(buffer)) {

		case READNONE:
		case READEOF:
			done = 1;
			break;

		case READSTR:
			done = LoopStr(&buffer[1]);
			break;

		case READTOK:
			done = LoopTok(&buffer[1]);
			break;
			}
		}
	Read_Fini();
	HFini();
	if (!OFini()) return(FALSE);
	if (done == 1) {
		xprintf("%d tables, largest has %d entries, %d entries, %d labels, %d bytes\n",
			cur_table,
			max_entry,
			tot_entries,
			reg_num,
			out_ptr - out_buf);
		return(TRUE);
		}
	else	return(FALSE);
	}


/* ------------------------------------------------------------ */

/* Process the token in a glyph.  Return 0 on more, 1 on done (ok), 2
on error.  Which is the current glyph item. (1=#, 2=abbrev, 3-8=lines  */

int
LoopGlyph(which, type, buf)
	int which;
	int type;
	char *buf;
	{
	char buffer[3 * TOKENMAX];
	int size;
	int val;
	FLAG skip;
	int num;

	buffer[0] = type;
	xstrcpy(&buffer[1], buf);
	if (buf[0] == READTOK) {
		if (buf[1] == '?') {
			if (!Select(&skip, buf)) return(2);
			if (skip) {
				num = Read_Token(buffer);
				if (num == READNONE || num == READEOF) {
					xeprintf("%s line %d: missing data.\n",
						Read_File(), Read_Line());
					return(2);
					}
				}
			return(0);
			}
		if (buf[1] == '!') {
			if (!Select(&skip, buf)) return(2);
			if (!skip) {
				num = Read_Token(buffer);
				if (num == READNONE || num == READEOF) {
					xeprintf("%s line %d: missing data.\n",
						Read_File(), Read_Line());
					return(2);
					}
				}
			return(0);
			}
		}
	
	if (buffer[0] == READSTR) {
		switch (which) {

		case 1:
			xeprintf("%s line %d: got string when expecting number.\n",
				Read_File(), Read_Line());
			return(2);
			/* break; */

		case 2:
			if (strlen(&buffer[1]) > 0) {
				if (strlen(&buffer[1]) != 2) {
					xeprintf("%s line %d: glyph abbrev '%s' is not 2 characters long.\n",
						Read_File(), Read_Line(),
						&buffer[1]);
					return(2);
					}
				val = (fontptr - fontmap) / 3;
				if (val < 0 || val > 255) {
					xeprintf(
"%s line %d: INTERNAL ERROR: wild pointer %d\n",
						Read_File(), Read_Line(), val);
					return(2);
					}
				aliases[val].xx[0] = buffer[1];
				aliases[val].xx[1] = buffer[2];
				}
			state = EXPGLYPH3;
			break;

		case 3:
			if ((val = Font_String(&buffer[1])) < 0) return(2);
			*fontptr = val << 4;
			state = EXPGLYPH4;
			break;

		case 4:
			if ((val = Font_String(&buffer[1])) < 0) return(2);
			*fontptr++ |= val;
			state = EXPGLYPH5;
			break;

		case 5:
			if ((val = Font_String(&buffer[1])) < 0) return(2);
			*fontptr = val << 4;
			state = EXPGLYPH6;
			break;

		case 6:
			if ((val = Font_String(&buffer[1])) < 0) return(2);
			*fontptr++ |= val;
			state = EXPGLYPH7;
			break;

		case 7:
			if ((val = Font_String(&buffer[1])) < 0) return(2);
			*fontptr = val << 4;
			state = EXPGLYPH8;
			break;

		case 8:
			if ((val = Font_String(&buffer[1])) < 0) return(2);
			*fontptr++ |= val;
			state = EXPTABLE;
			break;
			}
		}
	else	{
		if (which > 1) {
			xeprintf("%s line %d: got '%s' when expecting glyph abbrev or line.\n",
			Read_File(), Read_Line(), &buffer[1]);
			return(2);
			}
		if (!SToAny(&buffer[1], &num) &&
			 !RegLookup(&num, &buffer[1])) {
			xeprintf("%s line %d: invalid value '%s':\n\
must be number or symbol.\n",
				Read_File(), Read_Line(), &buffer[1]);
			return(2);
			}
		if (opt_v) xprintf("value num %d\n", num);
		if (num >= 256) {
			xeprintf("%s line %d: WARNING: value '%d' is >= 256, %d used.\n",
				Read_File(), Read_Line(), num, num & 0xFF);
			}
		fontptr = &fontmap[(num & 0xff) * 3];
		state = EXPGLYPH2;
		}
	return(0);
	}


/* ------------------------------------------------------------ */

/* Process the token in a list.  Return 0 on more, 1 on done (ok), 2
on error.  Which is 1 on list8 or 2 on list16.  */

int
LoopList(which, buf)
	int which;
	char *buf;
	{
	char buffer[3 * TOKENMAX];
	int size;
	FLAG skip;
	int num;

	if (strcmp(buf, ".") == 0) {
		state = EXPENTNAME;
		return(0);
		}
	if (buf[0] == '?') {
		if (!Select(&skip, buf)) return(2);
		if (skip) {
			num = Read_Token(buffer);
			if (num == READNONE || num == READEOF) {
				xeprintf("%s line %d: missing data.\n",
					Read_File(), Read_Line());
				return(2);
				}
			}
		return(0);
		}
	if (buf[0] == '!') {
		if (!Select(&skip, buf)) return(2);
		if (!skip) {
			num = Read_Token(buffer);
			if (num == READNONE || num == READEOF) {
				xeprintf("%s line %d: missing data.\n",
					Read_File(), Read_Line());
				return(2);
				}
			}
		return(0);
		}
	if (strequ(buf, "fontmap")) {
		if (opt_v) xprintf("value fontmap\n");
		if (first_list) {
			if (!OData(1, fontmap[0])) return(2);
			}
		else	{
			first_list = TRUE;
			if (!ONum(1, fontmap[0])) return(2);
			}
		for (num = 1; num < 256 * 3; num++) {
			if (!OData(1, fontmap[num])) return(2);
			}
		return(0);
		}
	else if (SToAny(buf, &num) || RegLookup(&num, buf)) {
		if (opt_v) xprintf("value num %d\n", num);
		if (first_list) {
			if (!OData(which, num)) return(2);
			}
		else	{
			first_list = TRUE;
			if (!ONum(which, num)) return(2);
			}
		return(0);
		}
	else	{
		xeprintf("%s line %d: invalid value '%s':\n\
Must be number or label.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		}
	}


/* ------------------------------------------------------------ */

/* Process the quoted string.  Return 0 on more, 1 on done (ok), 2 on
error.  */

int
LoopStr(buf)
	char *buf;
	{
	int size;
	int val;

	if (!Fix_String(buf)) return(2);
	if (opt_v) xprintf("%s string '%s' %c\n", state_names[(int)state],
		buf, got_value ? 't' : 'f');
	switch (state) {

	case EXPTABLE:
		xeprintf("%s line %d: Unexpected quoted string\n\
'%s', expecting 'table' or 'menu'.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPTABNAME:
		xeprintf("%s line %d: Unexpected quoted string\n\
'%s', expecting table name.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPENTNAME:
		xeprintf("%s line %d: Unexpected quoted string\n\
'%s', expecting entry name.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPENTVAL:
		if (!got_value) {
			if (opt_v) xprintf("value string '%s'\n", buf);
			if (!OPut(buf, strlen(buf) + 1)) return(2);
			got_value = TRUE;
			}
		state = EXPENTNAME;
		break;

	case EXPENTKEYVAL:
		if (!got_value) {
			if (opt_v) xprintf("value key '%s'\n", buf);
			size = ToKey(buf);
			if (!OPut(buf, size)) return(2);
			got_value = TRUE;
			}
		state = EXPENTNAME;
		break;

	case EXPENTLIST8:
		xeprintf("%s line %d: Unexpected quoted string\n\
'%s', expecting list8 value.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPENTLIST16:
		xeprintf("%s line %d: Unexpected quoted string\n\
'%s', expecting list16 value.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case SKIPENTVAL:
		state = EXPENTNAME;
		break;

	case SKIPLIST:
		xeprintf("%s line %d: Unexpected quoted string\n\
'%s', expecting list value.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPINCLUDE:
		if (opt_v) xprintf("including file %s\n", buf);
		Read_Push(buf);
		Read_Comment('#');
		state = EXPTABLE;
		break;

	case EXPGLYPH1:
		return(LoopGlyph(1, READSTR, buf));
		/* break; */

	case EXPGLYPH2:
		return(LoopGlyph(2, READSTR, buf));
		/* break; */

	case EXPGLYPH3:
		return(LoopGlyph(3, READSTR, buf));
		/* break; */

	case EXPGLYPH4:
		return(LoopGlyph(4, READSTR, buf));
		/* break; */

	case EXPGLYPH5:
		return(LoopGlyph(5, READSTR, buf));
		/* break; */

	case EXPGLYPH6:
		return(LoopGlyph(6, READSTR, buf));
		/* break; */

	case EXPGLYPH7:
		return(LoopGlyph(7, READSTR, buf));
		/* break; */

	case EXPGLYPH8:
		return(LoopGlyph(8, READSTR, buf));
		/* break; */
		}
	return(0);
	}


/* ------------------------------------------------------------ */

/* Process the token.  Return 0 on more, 1 on done (ok), 2 on error.
*/

int
LoopTok(buf)
	char *buf;
	{
	int size;
	FLAG skip;
	int num;

	if (opt_v) xprintf("%s token '%s' %c\n", state_names[(int)state],
		buf, got_value ? 't' : 'f');
	switch (state) {

	case EXPTABLE:
		if (strcmp(buf, "table") == 0 || strcmp(buf, "menu") == 0) {
			state = EXPTABNAME;
			return(0);
			}
		else if (strcmp(buf, "glyph") == 0) {
			state = EXPGLYPH1;
			return(0);
			}
		else if (strcmp(buf, "include") == 0) {
			state = EXPINCLUDE;
			return(0);
			}
		xprintf("%s line %d: got '%s' when expecting keyword.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPTABNAME:
		if (!RegTable(buf, "table")) return(2);
		state = EXPENTNAME;
		return(0);
		/* break; */

	case EXPENTNAME:
		if (buf[0] == '_') {
			if (!got_value) {
				if (!Select(&skip, buf)) return(2);
				if (skip)
					state = SKIPENTVAL;
				else	state = EXPENTVAL;
				}
			else	{
				state = SKIPENTVAL;
				}
			return(0);
			}
		if (strcmp(buf, "table") == 0 || strcmp(buf, "menu") == 0) {
			state = EXPTABNAME;
			return(0);
			}
		else if (strcmp(buf, "glyph") == 0) {
			state = EXPGLYPH1;
			return(0);
			}
		else if (strcmp(buf, "include") == 0) {
			state = EXPINCLUDE;
			return(0);
			}
		if (!RegEntry(buf)) return(2);
		got_value = FALSE;
		state = EXPENTVAL;
		return(0);
		/* break; */

	case EXPENTVAL:
		if (buf[0] == '_') {
			if (!Select(&skip, buf)) return(2);
			if (skip) state = SKIPENTVAL;
			got_value = FALSE;
			return(0);
			}
		if (strcmp(buf, "key") == 0) {
			state = EXPENTKEYVAL;
			return(0);
			}
		if (strcmp(buf, "list8") == 0) {
			first_list = FALSE;
			state = EXPENTLIST8;
			return(0);
			}
		if (strcmp(buf, "list16") == 0) {
			first_list = FALSE;
			state = EXPENTLIST16;
			return(0);
			}
		else if (SToAny(buf, &num) || RegLookup(&num, buf)) {
			if (!got_value) {
				if (opt_v) xprintf("value num %d\n", num);
				if (!ONum(0, num)) return(2);
				got_value = TRUE;
				}
			state = EXPENTNAME;
			return(0);
			}
		else	{
			xeprintf("%s line %d: invalid value '%s':\n\
must be number, symbol, or 'key' or string.\n",
				Read_File(), Read_Line(), buf);
			return(2);
			}
		/* break; */

	case EXPENTKEYVAL:
		xeprintf("%s line %d: got '%s' when expecting quoted key sequence.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPENTLIST8:
		return(LoopList(1, buf));
		/* break; */

	case EXPENTLIST16:
		return(LoopList(2, buf));
		/* break; */

	case SKIPENTVAL:
		if (strcmp(buf, "list8") == 0 ||
			 strcmp(buf, "list16") == 0) {
			state = SKIPLIST;
			}
		else if (strcmp(buf, "key") != 0) {
			state = EXPENTNAME;
			}
		return(0);
		/* break; */

	case SKIPLIST:
		if (strcmp(buf, ".") == 0) state = EXPENTNAME;
		return(0);
		/* break; */

	case EXPINCLUDE:
		xeprintf("%s line %d: got '%s' when expecting include string.\n",
			Read_File(), Read_Line(), buf);
		return(2);
		/* break; */

	case EXPGLYPH1:
		return(LoopGlyph(1, READTOK, buf));
		/* break; */

	case EXPGLYPH2:
		return(LoopGlyph(2, READTOK, buf));
		/* break; */

	case EXPGLYPH3:
		return(LoopGlyph(3, READTOK, buf));
		/* break; */

	case EXPGLYPH4:
		return(LoopGlyph(4, READTOK, buf));
		/* break; */

	case EXPGLYPH5:
		return(LoopGlyph(5, READTOK, buf));
		/* break; */

	case EXPGLYPH6:
		return(LoopGlyph(6, READTOK, buf));
		/* break; */

	case EXPGLYPH7:
		return(LoopGlyph(7, READTOK, buf));
		/* break; */

	case EXPGLYPH8:
		return(LoopGlyph(8, READTOK, buf));
		/* break; */
		}
	}


/* ------------------------------------------------------------ */

/* Initialize the output file. Return True on success or print a
message and return False on failure. */

FLAG
OInit()
	{
#if defined(MSDOS)
	if ((ofd = creat(rfile, 0666)) < 0) {
#endif
#if defined(UNIX)
	if ((ofd = open(rfile, O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0) {
#endif
		xeprintf("Cannot create ouput file '%s'.\n", rfile);
		return(FALSE);
		}
	memset(out_buf, NUL, sizeof(out_buf));
	((struct res_header *)out_buf)->significator = RESSIG;
	((struct res_header *)out_buf)->version = RESVERSION;
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Finish with the output file. Return True on success or print a
message and return False on failure. */

FLAG
OFini()
	{
	int amt;
	int where;
	int cnt;
	int len;

	if ((cnt = ONewTable()) < 0) return(-1);	/* extra one */
	((struct res_header *)out_buf)->num_tables.low  =  cnt       & 0xFF;
	((struct res_header *)out_buf)->num_tables.high = (cnt >> 8) & 0xFF;

	amt = write(ofd, out_buf, out_ptr - out_buf);
	close(ofd);

	if (amt != out_ptr - out_buf) {
		xeprintf("Write error on output file.\n");
		unlink(rfile);
		return(FALSE);
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Put a value. Return True on success or print a message and return
False on failure.  TYPE is 1 on one byte, 2 on 2 byte, or 0 on
auto-select. */

FLAG
OData(type, num)
	int type;
	int num;
	{
	if (out_ptr - out_buf + 2 > RESMAXSIZE) {
		xeprintf("%s line %d: Output file too big.\n",
			Read_File(), Read_Line());
		return(FALSE);
		}
	if (type == 1 || (type == 0 && num >= 0 && num <= 127)) {
		*out_ptr++ = num & 0xFF;
		}
	else	{
		*out_ptr++ = num & 0xFF;
		*out_ptr++ = (num >> 8) & 0xFF;
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Allocate a new table entry. Return the next entry number on success
or print a message and return -1 on failure. */

int
ONewEntry()
	{
	int where;

	if (cur_entry >= RESMAXENTRIES) {
		xeprintf("%s line %d: More than %d entries in a table.\n",
			Read_File(), Read_Line(), RESMAXENTRIES);
		return(-1);
		}
	where = out_ptr - out_buf;
	work_table[cur_entry + 1].low  =  where       & 0xFF;
	work_table[cur_entry + 1].high = (where >> 8) & 0xFF;
	return(++cur_entry);
	}


/* ------------------------------------------------------------ */

/* Allocate a new table. Return the table number on success or print a
message and return -1 on failure. */

int
ONewTable()
	{
	int where;
	int cnt;
	int len;

	if (cur_table == -1) {
		return(++cur_table);
		}
	if (cur_entry == 0) {
		work_table[0].low = 0;
		work_table[0].high = 0;
		cnt = 1;
		}
	else	{
		where = out_ptr - out_buf;
		work_table[cur_entry + 1].low  =  where       & 0xFF;
		work_table[cur_entry + 1].high = (where >> 8) & 0xFF;
		work_table[0].low  =   cur_entry + 1       & 0xFF; /* store count */
		work_table[0].high = ((cur_entry + 1)>> 8) & 0xFF;
		cnt = cur_entry + 2;
		}
	if (cnt > max_entry) max_entry = cnt;
	tot_entries += cnt;
	cur_entry = 0;

	if (cur_table >= RESTABLES) {
		xeprintf("%s line %d: More than %d tables.\n",
			Read_File(), Read_Line(), RESTABLES);
		return(-1);
		}
	where = out_ptr - out_buf;
	len = cnt * sizeof(work_table[0]);
	if (out_ptr - out_buf + len > RESMAXSIZE) {
		xeprintf("%s line %d: Output file too big.\n",
			Read_File(), Read_Line());
		return(-1);
		}
	memmove(out_ptr, (char *)work_table, len);
	out_ptr += len;

	((struct res_header *)out_buf)->tables[cur_table].low =
		where       & 0xFF;
	((struct res_header *)out_buf)->tables[cur_table].high =
		(where >> 8) & 0xFF;
	return(++cur_table);
	}


/* ------------------------------------------------------------ */

/* Put a value. Return True on success or print a message and return
False on failure.  TYPE is 1 on one byte, 2 on 2 byte, or 0 on
auto-select. */

FLAG
ONum(type, num)
	int type;
	int num;
	{
	if (ONewEntry() < 0) return(FALSE);
	if (out_ptr - out_buf + 2 > RESMAXSIZE) {
		xeprintf("%s line %d: Output file too big.\n",
			Read_File(), Read_Line());
		return(FALSE);
		}
	if (type == 1 || (type == 0 && num >= 0 && num <= 127)) {
		*out_ptr++ = num & 0xFF;
		}
	else	{
		*out_ptr++ = num & 0xFF;
		*out_ptr++ = (num >> 8) & 0xFF;
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Put a value. Return True on success or print a message and return
False on failure. */

FLAG
OPut(buf, buflen)
	char *buf;
	int buflen;
	{
	if (ONewEntry() < 0) return(FALSE);
	if (out_ptr - out_buf + buflen > RESMAXSIZE) {
		xeprintf("%s line %d: Output file too big.\n",
			Read_File(), Read_Line());
		return(FALSE);
		}
	memmove(out_ptr, buf, buflen);
	out_ptr += buflen;
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Look up the specified symbol.  If found, put its value in NUMPTR
and return True.  Otherwise, print a message and return False.  */

FLAG
RegLookup(numptr, buf)
	int *numptr;
	char *buf;
	{
	int cnt;

	for (cnt = 0; cnt < reg_num; cnt++) {
		if (strcmp(buf, registry[cnt].name) == 0) {
			*numptr = registry[cnt].value;
			return(TRUE);
			}
		}
	xeprintf("%s line %d: label '%s' not found.\n",
		Read_File(), Read_Line(), buf);
	return(FALSE);
	}


/* ------------------------------------------------------------ */

/* Register the specified symbol as an entry. Return True on success
or print a message and return False on failure.  */

FLAG
RegEntry(buf)
	char *buf;
	{
	int cnt;

	if (strcmp(buf, "-") == 0) return(TRUE);
	buf[sizeof(registry[0].name) - 1] = NUL;
	if (!xisalpha(*buf) ||
		 *sfindnotin(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")
		  != NUL) {
		xeprintf("%s line %d: label '%s' has invalid characters.\n",
				Read_File(), Read_Line(), buf);
		return(FALSE);
		}
	for (cnt = 0; cnt < reg_num; cnt++) {
		if (strcmp(buf, registry[cnt].name) == 0) {
			xeprintf("%s line %d: duplicate label '%s'.\n",
				Read_File(), Read_Line(), buf);
			return(FALSE);
			}
		}
	if (cnt >= REGNUM) {
		xeprintf("%s line %d: too many labels.\n",
			Read_File(), Read_Line());
		return(FALSE);
		}
	xstrncpy(registry[reg_num].name, buf);
	registry[reg_num++].value = cur_entry;
	if (!HLabel(buf, cur_entry, "entry")) return(FALSE);
	if (opt_v) xprintf("registered entry '%s' value %d\n", buf, cur_entry);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Register the specified symbol as a table.  WHICH is "menu" or
"table". Return True on success or print a message and return False on
failure.  */

FLAG
RegTable(buf, which)
	char *buf;
	char *which;
	{
	int cnt;

	if (strcmp(buf, "-") == 0) {
		if (ONewTable() < 0) return(FALSE);
		return(TRUE);
		}
	buf[sizeof(registry[0].name) - 1] = NUL;
	if (!xisalpha(*buf) ||
		 *sfindnotin(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")
		  != NUL) {
		xeprintf("%s line %d: label '%s' has invalid characters.\n",
				Read_File(), Read_Line(), buf);
		return(FALSE);
		}
	for (cnt = 0; cnt < reg_num; cnt++) {
		if (strcmp(buf, registry[cnt].name) == 0) {
			xeprintf("%s line %d: duplicate label '%s'.\n",
				Read_File(), Read_Line(), buf);
			return(FALSE);
			}
		}
	if (cnt >= REGNUM) {
		xeprintf("%s line %d: too many labels.\n",
			Read_File(), Read_Line());
		return(FALSE);
		}
	xstrncpy(registry[reg_num].name, buf);
	if ((cnt = ONewTable()) < 0) return(FALSE);
	registry[reg_num++].value = cnt;
	if (!HLabel(buf, cnt, which)) return(FALSE);
	if (opt_v) xprintf("registered table '%s' value %d\n", buf, cnt);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Check the supplied buffer for a skip directive (you get to assume
buf[0] is '_', '?', or '!').  Return True on success or print a
message and return False on failure.  Put True in SKIPPTR if we are
supposed to skip the next entry or False if not. */

FLAG
Select(skipptr, buf)
	FLAG *skipptr;
	char *buf;
	{
	while (*++buf != NUL) {
		if (*buf == '*') {
			*skipptr = FALSE;
			return(TRUE);
			}
		if (!xislower(*buf)) {
			xeprintf("%s line %d: invalid skip selector character '%c'.\n",
			Read_File(), Read_Line(), *buf);
			return(FALSE);
			}
		if (*buf == val_for) {
			*skipptr = FALSE;
			return(TRUE);
			}
		}
	*skipptr = TRUE;
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Convert a string value to a key sequence.  Return the size of the
sequence in bytes. */

int
ToKey(buf)
	char *buf;
	{
	char key[TOKENMAX];
	char digbuf[TOKENMAX];
	char *cptr;
	char *kptr = key;
	char *savebuf = buf;
	int num;

	for ( ; *buf != NUL; buf++) {

		if (*buf != '^') {
			*kptr++ = *buf;
			*kptr++ = NUL;
			}
		else	{
			buf++;
			if (xisdigit(*buf)) {
				cptr = sfindnotin(buf, "0123456789");
				memmove(digbuf, buf, cptr - buf);
				digbuf[cptr - buf] = NUL;
				num = 0;
				SToN(digbuf, &num, 10);
				*kptr++ = num;
				*kptr++ = '\x1';
				buf = cptr - 1;
				}
			else if (xisupper(*buf)) {
				*kptr++ = *buf ^ '@';
				*kptr++ = NUL;
				}
			else	{
				switch (*buf) {

				case '@':
				case '[':
				case '\\':
				case ']':
				case '^':
				case '_':
				case DEL:
					*kptr++ = *buf ^ '@';
					*kptr++ = NUL;
					break;

				case '=':
					*kptr++ = '^';
					*kptr++ = NUL;
					break;

				case ':':
					*kptr++ = 1;
					*kptr++ = 1;
					break;

				case '!':
					cptr = sindex(++buf, '`');
					memmove(digbuf, buf, cptr - buf);
					digbuf[cptr - buf] = NUL;
					buf = cptr;
					if (!RegLookup(&num, digbuf)) {
						return(0);
						}
					if (num >= 100) {
						*kptr++ = num / 100 + '0';
						*kptr++ = NUL;
						num %= 100;

						*kptr++ = num / 10 + '0';
						*kptr++ = NUL;
						num %= 10;

						*kptr++ = num + '0';
						*kptr++ = NUL;
						}
					else if (num >= 10) {
						*kptr++ = num / 10 + '0';
						*kptr++ = NUL;
						num %= 10;

						*kptr++ = num + '0';
						*kptr++ = NUL;
						}
					else	{
						*kptr++ = num + '0';
						*kptr++ = NUL;
						}
					break;
					}
				}
			}
		}
	memmove(savebuf, key, kptr - key);
	return(kptr - key);
	}


/* end of THORRES.C -- Compile Resource Files */
