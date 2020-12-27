/* FTERM.C -- Terminal Interface Routines

	Written March 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"

#if defined(UNIX)
#undef TRUE
#undef FALSE
#undef CR
#undef NL
#undef BS
#include <curses.h>

#if defined(UNIXV)
static int savemodes;
#else	/* assume Berkeley sockets */
#endif
#endif

#if defined(UNIX)
#if defined(UNIXV)
#else	/* assume Berkeley sockets */
#if defined(SUNOS3)
#define KERNEL		1	/* Sun repeats the declaration of struct tm */
#endif				/* in this file, so we work around it */
#include <sys/time.h>
#if defined(SUNOS3)
#undef KERNEL
#endif
#include <sys/ioctl.h>
#define RFDSIZE		32	/* in bits */
static struct sgttyb savtty;
#endif
#endif

/* -------------------- screen types -------------------- */

enum METHODS {
	MB,	/* ROM BIOS */
	MM,	/* memory mapped */
	MT,	/* termcap */
	MU,	/* unsupported */
	MV,	/* VT100 */
	M1,	/* custom 1 */
	M2,	/* custom 2 */
	};

static struct screen {
	unsigned baseaddr;	/* screen base memory paragraph */
	char rows;		/* number of rows */
	char cols;		/* number of columns */
	char mapwidth;		/* width of screen map in characters
				(text only) */
	char mode;		/* display mode value */
	char hpmode;		/* mode value to use on the 95/100 */
	char hpzoom;		/* zoom value to use on the 95/100 */
	enum METHODS method;	/* output method */
	char curcnt;		/* for text modes, the last line line of
				cursor end, otherwise, zero */
	char maskcnt;		/* for 100LX graphics modes, the count of
				lines of the cursor mask, otherwise zero */
	int curmask;		/* for 100LX graphics modes, the cursor
				mask */
	} screens[] = {
	/* CGA, text, 1 bit/pixel, 200 x 320 pixels */
{ 0xB800, 25, 40, 40,  0,  0,    0, MM,  7, 0, 0 },

	/* CGA, text, 4 bits/pixel, 200 x 320 pixels */
{ 0xB800, 25, 40, 40,  1,  1,    1, MM,  7, 0, 0 },

	/* CGA, text, 1 bits/pixel, 200 x 640 pixels */
{ 0xB800, 25, 80, 80,  2,  2,    2, MM,  7, 0, 0 },

	/* CGA, text, 4 bits/pixel, 200 x 640 pixels */
{ 0xB800, 25, 80, 80,  3,  3,    3, MM,  7, 0, 0 },

	/* CGA, graphics, 2 bits/pixel, 8 x 8 font, 200 x 320 pixels */
{ 0xB800, 25, 40, 40,  4,  4,    4, MB,  0, 0, 0 },

	/* CGA, graphics, 2 bits/pixel, 8 x 8 font, 200 x 320 pixels,
	monochrome */
{ 0xB800, 25, 40, 40,  5,  5,    5, MB,  0, 0, 0 },

	/* CGA, graphics, 1 bit/pixel, 8 x 8 font, 200 x 640 pixels */
{ 0xB800, 25, 80, 80,  6,  6,    6, MB,  0, 0, 0 },

	/* MDA, text, 1 bit/pixel */
{ 0xB000, 25, 80, 80,  7,  7, 0x21, MM, 13, 0, 0 },

	/* PCJr, graphics, 4 bits/pixel, 8 x 8 font, 200 x 160 pixels */
{      0, 25, 20, 20,  8,  8,    8, MU,  0, 0, 0 },

	/* PCJr, graphics, 4 bits/pixel, 8 x 8 font, 200 x 320 pixels */
{      0, 25, 40, 40,  9,  9,    9, MU,  0, 0, 0 },

	/* PCJr,EGA , graphics, 2,6 bits/pixel, 8 x 8 font, 200 x 640 pixels */
{      0, 25, 80, 80, 10, 10,   10, MU,  0, 0, 0 },

	/* ?, graphics */
{      0,  0,  0,  0, 11, 11,   11, MU,  0, 0, 0 },

	/* ?, graphics */
{      0,  0,  0,  0, 12, 12,   12, MU,  0, 0, 0 },

	/* EGA, graphics, 4 bits/pixel, 8 x 8 font, 200 x 320 pixels */
{ 0xA800, 25, 40, 40, 13, 13,   13, MB,  0, 0, 0 },

	/* EGA, graphics, 4 bits/pixel, 8 x 8 font, 200 x 640 pixels */
{ 0xA800, 25, 80, 80, 14, 14,   14, MB,  0, 0, 0 },

	/* EGA, graphics, 2 bits/pixel, 8 x 8 font, 350 x 640 pixels */
{ 0xA000, 43, 80, 80, 15, 15,   15, MB,  0, 0, 0 },

	/* EGA, ? */
{ 0xA800,  0,  0,  0, 16, 16,   16, MU,  0, 0, 0 },

	/* 95LX, text, 1 bit/pixel */
{ 0xB000, 16, 40, 80, 24,  7,    7, MM, 13, 0, 0 },

	/* 95LX, graphics, 1 bit/pixel, 6 x 4 font, 128 x 240 pixels */
{ 0xB000, 21, 60, 60, 25,0x20,0x20, M1,  5, 0, 0 },

	/* 95LX, graphics, 1 bit/pixel, 5 x 3 font, 128 x 240 pixels */
{ 0xB000, 25, 80, 80, 26,0x20,0x20, M2,  4, 0, 0 },

	/* 100LX, text, 1 bit/pixel, 8 x 8 font */
{ 0xB000, 25, 80, 80, 27,0x21,0x21, MB, 13, 0, 0 },

	/* VT100, text, use standard output */
{      0, 24, 80, 80, 28, 29,   29, MV,  0, 0, 0 },

	/* Termcap, text, use API */
{      0,  0,  0,  0, 29, 29,   29, MT,  0, 0, 0 },

	/* 100LX, text, 1 bit/pixel, 11 x 10 font, 200 x 640 pixels */
{ 0xB800, 18, 64, 80, 80,  2, 0x80, MM, 10, 0, 0 },

	/* 100LX, text, 2 bits/pixel, 11 x 10 font, 200 x 640 pixels */
{ 0xB800, 18, 64, 80, 81,  3, 0x81, MM, 10, 0, 0 },

	/* 100LX, text, 1 bit/pixel, 8 x 8 font, 200 x 320 pixels */
{ 0xB800, 25, 40, 80, 82,  2, 0x82, MM,  7, 0, 0 },

	/* 100LX, text, 2 bits/pixel, 8 x 8 font, 200 x 320 pixels */
{ 0xB800, 25, 40, 80, 83,  3, 0x83, MM,  7, 0, 0 },

	/* 100LX, text, 1 bit/pixel, 12 x 16 font, 200 x 640 pixels */
{ 0xB800, 16, 40, 80, 84,  2, 0x84, MM, 11, 0, 0 },

	/* 100LX, text, 2 bits/pixel, 12 x 16 font, 200 x 640 pixels */
{ 0xB800, 16, 40, 80, 85,  3, 0x85, MM, 11, 0, 0 },
	};


/* -------------------- keyboard input -------------------- */

#if defined(UNIX)
static KEYCODE gotchar = KEYNONE;	/* buffered character */
static KEYCODE gotchar2 = KEYNONE;
static char vt100state = 0;
#endif

/* -------------------- misc -------------------- */

static char printbuf[SMALLBUFFSIZE];  /* for TPrintChar */

static char outbuf[128]; /* output buffer */
static char *outptr;

static FLAG just_cleared;	/* was the screen just cleared? */
static unsigned char *tinyfont;

	/* current values */
static struct screen *cur_screen;
static char cur_method;
static char cur_attrib;

#if defined(MSDOS)
static char huge *scrnbase;
static char huge *scrnptr;
static int old_mode;

static int line_offsets[ROWMAX];
#endif


void T_CurInvert(void);
void T_RawChar(char c);
void T_RawFlush(void);
void T_RawStr(char *str);
void T_VAttr(int new);

/* ------------------------------------------------------------ */

FLAG
TInit(void)
	{
	int i;
	int cnt;
	unsigned char *attrlist;
#if defined(UNIX)
#if !defined(UNIXV)
	struct sgttyb tty;
#endif
#endif

	outptr = outbuf;
	just_cleared = FALSE;

	if (Res_Char(RES_CONF, RES_USECARAT) != 'Y') {
		for (cnt = 0; cnt < 256; cnt++) {
			tprintrep[(cnt & 0xFF) * 4]     = 1;
			tprintrep[(cnt & 0xFF) * 4 + 1] = cnt;
			}
		}

	if (screen_type == 100) {
#if defined(MSDOS)
		old_mode = VidInit(100, 0, 0);
		screen_type = old_mode;
#endif
		}

	for (i = 0; i < sizeof(screens) / sizeof(screens[0]); i++) {
		if (screen_type == (screens[i].mode & 0xff)) break;
		}
	if (i >= sizeof(screens) / sizeof(screens[0])) return(FALSE);
#if defined(MSDOS)
	if (screens[i].method == MU) i = 7;
#endif
	cur_screen = &screens[i];

	cur_method = cur_screen->method;
	i = Res_Char(RES_CONF, RES_SCRNMETHOD);
	if (cur_method == MM && i == MB) cur_method = MB;

/* ----- initialize keyboard ----- */
	switch (key_method) {

	case 'S':
#if defined(UNIX)
	case 'V':
#if defined(UNIXV)
		return(FALSE);		/* not supported yet */
#else	/* assume Berkeley sockets */
		if (ioctl(0, TIOCGETP, (int)&tty) < 0) return(FALSE);
		savtty = tty;
		tty.sg_flags |= RAW;
		tty.sg_flags &= ~ECHO;
		if (ioctl(0, TIOCSETP, (int)&tty) < 0) return(FALSE);
#endif
		if (key_method == 'V') T_RawStr("\033[?1h");
#endif
		break;
		}

/* ----- initialize output ----- */
	tcols = cur_screen->cols;
	trows = cur_screen->rows;
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
	case MM:
		scrnbase = (char huge *)((long)(cur_screen->baseaddr) << 16);
		for (cnt = 0; cnt < ROWMAX; cnt++) {
			line_offsets[cnt] = cur_screen->mapwidth * 2 * cnt;
			}
		VidInit(cur_screen->hpmode,
			(c.g.madefor == 'C' || c.g.madefor == 'D') ?
				cur_screen->hpzoom : 0,
			cur_screen->curcnt);
		VidCurOn();
		break;
#endif

#if defined(UNIX)
	case MT:
		initscr();
		noecho();
		raw();
		leaveok(stdscr, FALSE);
		tcols = COLS;
		trows = LINES;
#if defined(UNIXV)
		if ((savemodes = fcntl(STDIN, F_GETFL, 0)) < 0 ||
			 fcntl(STDIN, F_SETFL, savemodes | O_NDELAY) < 0) {
			return(FALSE);
			}
#endif
		break;
#endif

	case MU:
		break;

	case MV:
		/* set VT100 mode, origin relative, jump scroll */
		T_RawStr("\033<\033[?6h\033[?4l");
		break;

#if defined(MSDOS)
	case M1:
	case M2:
		tinyfont = Res_String(NULL, RES_TINYFONT, 0);
		old_mode = VidInit(cur_screen->hpmode, cur_screen->hpzoom,
			cur_screen->curcnt);
		VidCurOn();
		just_cleared = TRUE;
		break;
#endif
		}

	attrlist = Res_String(NULL, RES_CONF, RES_ATTRLIST) +
		(cur_screen - screens) * 4;
	tattrnorm = attrlist[0];
	tattrrev = attrlist[1];
	tattrunder = attrlist[2];
	tattrru = attrlist[3];
	tcol = 0;
	toffset = 0;
	trow = 0;
	cur_attrib = tattrnorm;
#if defined(MSDOS)
	scrnptr = scrnbase;
#endif
	TClrScreen();
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Terminate special terminal handling. */

void
TFini(void)
	{
	TForce();

	switch (key_method) {

	case 'S':
#if defined(UNIX)
	case 'V':
		if (key_method == 'V') T_RawStr("\033[?1l");
#if defined(UNIXV)
#else	/* assume Berkeley sockets */
		ioctl(0, TIOCSETP, (int)&savtty);
#endif
#endif
		break;
		}

	switch (cur_method) {

#if defined(MSDOS)
	case MB:
	case MM:
	case M1:
	case M2:
		VidFini(old_mode, 0);
		break;
#endif

#if defined(UNIX)
	case MT:
#if defined(UNIXV)
		fcntl(STDIN, F_SETFL, savemodes);
#endif
		mvcur(0, COLS - 1, LINES - 1, 0);
		clrtoeol();
		refresh();
		endwin();
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Display COUNT items from the data in the data block.  The block is
in (char, attribute) pairs.  (COUNT counts pairs, not bytes.)  If
DOCLEAR is set, clear to the end of the line with attribute CLEARATTR.
*/

void
TAttrBlock(char *data, int count, FLAG doclear, int clearattr)
	{
	int amt;
	int chr;
	int devattr;
	int saveattr = cur_attrib;
#if defined(MSDOS)
	int cnt;
#endif

	if (cur_attrib != tattrnorm) THiOff();

	TForce();
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
		amt = tcol - toffset;
		if (amt < 0) amt = 0;
		if (amt > tcols) amt = tcols;
		tcol += count;
		while (count-- > 0) {
			chr = *data++;
			VidCursor(trow, amt++);
			VidChar(chr, *data++);
			tcol++;
			}
		if (doclear) {
			cur_attrib = clearattr;
			VidClear(tcols - tcol, cur_attrib);
			}
		break;

	case MM:
		tcol += count;
		while (count-- > 0) {
			*scrnptr++ = *data++;
			*scrnptr++ = *data++;
			}
		if (doclear) {
			for (cnt = 0; cnt < tcols - tcol; cnt++) {
				*scrnptr++ = SP;
				*scrnptr++ = clearattr;
				}
			scrnptr -= 2 * (tcols - tcol);
			}
		break;

	case M1:
	case M2:
		while (count-- > 0) {
			chr = *data++;
			cur_attrib = *data++;
			T_RawChar(chr);
			tcol++;
			}
		if (doclear) {
			cur_attrib = clearattr;
			for (cnt = 0; cnt < tcols - tcol; cnt++) {
				T_RawChar(SP);
				}
			}
		break;
#endif

	case MV:
		devattr = tattrnorm;
		while (count-- > 0) {
			chr = *data++;
			cur_attrib = *data++;
			if (cur_attrib != devattr) {
				T_VAttr(cur_attrib);
				devattr = cur_attrib;
				}
			T_RawChar(chr);
			tcol++;
			}
		if (doclear) {
			if (clearattr != devattr) {
				T_VAttr(clearattr);
				devattr = cur_attrib;
				}
			cur_attrib = clearattr;
			T_RawStr("\033[K");
			}
		if (saveattr != devattr) {
			T_VAttr(saveattr);
			devattr = cur_attrib;
			}
		break;

#if defined(UNIX)
	case MT:
		devattr = tattrnorm;
		while (count-- > 0) {
			chr = *data++;
			cur_attrib = *data++;
			if (cur_attrib != devattr) {
				(cur_attrib == tattrnorm) ? standend() :
					standout();
				devattr = cur_attrib;
				}
			addch(chr);
			tcol++;
			}
		if (doclear) {
			if (clearattr != devattr) {
				(clearattr == tattrnorm) ? standend() :
					standout();
				devattr = cur_attrib;
				}
			cur_attrib = clearattr;
			clrtoeol();
			}
		if (saveattr != devattr) {
			(saveattr == tattrnorm) ? standend() : standout();
			devattr = cur_attrib;
			}
		break;
#endif
		}
	if (saveattr != tattrnorm) THiOn();
	cur_attrib = saveattr;
	}


/* ------------------------------------------------------------ */

/* Ring the terminal bell. */

void
TBell(void)
	{
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
	case MM:
	case M1:
	case M2:
		VidBell();
		break;
#endif

	case MV:
		T_RawChar(BEL);
		break;

#if defined(UNIX)
	case MT:
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Clear to the end of the line. */

void
TCLEOL(void)
	{
#if defined(MSDOS)
	int cnt;
	int savecol = tcol;
#endif

	TForce();
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
		VidClear(tcols - tcol, cur_attrib);
		break;

	case MM:
		for (cnt = 0; cnt < tcols - tcol; cnt++) {
			*scrnptr++ = SP;
			*scrnptr++ = cur_attrib;
			}
		scrnptr -= 2 * (tcols - tcol);
		break;

	case M1:
	case M2:
		for (cnt = tcols - tcol; cnt > 0; cnt--, tcol++) {
			T_RawChar(SP);
			}
		tcol = savecol;
		break;
#endif

	case MV:
		T_RawStr("\033[K");
		break;

#if defined(UNIX)
	case MT:
		clrtoeol();
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Clear the entire screen and put the cursor at row,column 0,0. */

void
TClrScreen(void)
	{
	if (just_cleared) {
		just_cleared = FALSE;
		TSetPoint(0, 0);
		return;
		}
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
	case MM:
	case M1:
	case M2:
		tcol = 0;
		for (trow = 0; trow < trows; ++trow) TCLEOL();
		break;
#endif

	case MV:
		TSetPoint(0,0);
		T_RawStr("\033[2J");
		break;

#if defined(UNIX)
	case MT:
		clear();
		break;
#endif
		}
	TSetPoint(0, 0);
	}


/* ------------------------------------------------------------ */

/* Ensure that the cursor is not displayed.  Only useful in tinyfonts. */

void
TCurOff(void)
	{
#if defined(MSDOS)
	if (cur_method == M1 || cur_method == M2) T_CurInvert();
#endif
	}


/* ------------------------------------------------------------ */

/* Ensure that the cursor is displayed.  Only useful in tinyfonts. */

void
TCurOn(void)
	{
#if defined(MSDOS)
	if (cur_method == M1 || cur_method == M2) T_CurInvert();
#endif
	}


/* ------------------------------------------------------------ */

/* Force the cursor to be displayed at the current row,column. */

void
TForce(void)
	{
#if defined(MSDOS)
	int amt = tcol - toffset;

	if (amt < 0) amt = 0;
	if (amt > tcols) amt = tcols;
#endif
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
		VidCursor(trow, amt);
		break;

	case MM:
		scrnptr = scrnbase + line_offsets[trow] + 2 * amt;
		VidCursor(trow, amt);
		break;

	case M1:
	case M2:
		VidCursor(trow * 16 / trows, amt * 40 / tcols);
		break;
#endif

	case MV:
		T_RawFlush();
		break;

#if defined(UNIX)
	case MT:
		refresh();
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Wait for and return the next key. */

KEYCODE
TGetKey(void)
	{
	KEYCODE chr;
	char cchr;

#if defined(UNIX)
	if (gotchar != KEYNONE) {
		chr = gotchar;
		gotchar = gotchar2;
		gotchar2 = KEYNONE;
		return(chr);
		}
#endif

	switch (key_method) {

#if defined(MSDOS)
	case 'B':
		chr = BGetKey();
		if (chr & 0xFF)
			chr &= 0xFF;
		else	chr = ((chr >>= 8) & 0xFF) + 0x100;
		break;

	case 'D':
		chr = PSystem(0x7) & 0xFF;
		if (chr == 0) chr = 0x100 + (PSystem(0x7) & 0xFF);
		break;

	case 'E':
		chr = BGetKeyE();
		if (chr & 0xFF)
			chr &= 0xFF;
		else	chr = ((chr >>= 8) & 0xFF) + 0x100;
		break;

	case 'J':
		chr = JGetKey();
		break;

	case 'P':
		while (read(3, &cchr, 1) < 1) ;
		chr = cchr;
		break;
#endif

	case 'S':
#if defined(UNIXV)
		return(0);
#else	/* assume Berkeley sockets */
		read(0, &cchr, sizeof(cchr));
		chr = cchr & 0xFF;
#endif
		break;

#if defined(UNIX)
	case 'T':
#if defined(UNIXV)
		read(0, &cchr, sizeof(cchr));
		chr = cchr & 0xFF;
#else	/* assume Berkeley sockets */
		chr = getch() & 0xFF;
#endif
		break;
#endif

#if defined(UNIX)
	case 'V':
#if defined(UNIXV)
		return(0);
#else	/* assume Berkeley sockets */
		read(0, &cchr, sizeof(cchr));
		chr = cchr & 0xFF;
#endif

		switch (vt100state) {

		case 0:
			if (chr == ESC) {
				vt100state = 1;
				return(TGetKey());
				}
			vt100state = 0;
			return(chr);
			/*break;*/

		case 1:
			if (chr == 'O') {
				vt100state = 2;
				return(TGetKey());
				}
			vt100state = 0;
			gotchar = chr;
			return(ESC);
			/*break;*/

		case 2:
			vt100state = 0;
			if (chr >= 'p' && chr <= 'y')
				return(chr - 'p' + '0');
			switch (chr) {

			case 'm':	return('-');	/*break;*/
			case 'l':	return(',');	/*break;*/
			case 'n':	return('.');	/*break;*/
			case 'M':	return('\15');	/*break;*/
			case 'P':	return(KEYF1);	/*break;*/
			case 'Q':	return(KEYF2);	/*break;*/
			case 'R':	return(KEYF3);	/*break;*/
			case 'S':	return(KEYF4);	/*break;*/
			case 'A':	return(KEYUP);	/*break;*/
			case 'B':	return(KEYDOWN);	/*break;*/
			case 'C':	return(KEYLEFT);	/*break;*/
			case 'D':	return(KEYRIGHT);	/*break;*/
			default:
				gotchar2 = chr;
				gotchar = 'O';
				return(ESC);
				}
			}
		break;
#endif
		}			
	return(chr);
	}


/* ------------------------------------------------------------ */

/* Turn off highlighting. */

void
THiOff(void)
	{
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
	case MM:
	case M1:
	case M2:
		cur_attrib = tattrnorm;
		break;
#endif

	case MV:
		T_RawStr("\033[0m");
		break;

#if defined(UNIX)
	case MT:
		standend();
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Turn on highlighting. */

void
THiOn(void)
	{
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
	case MM:
	case M1:
	case M2:
		cur_attrib = tattrrev;
		break;
#endif

	case MV:
		T_RawStr("\033[7m");
		break;

#if defined(UNIX)
	case MT:
		standout();
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Return 'Y', if a key is available, 'N' if not, or '?' if you can't
tell. */

char
TIsKey(void)
	{
#if defined(UNIX)
#if !defined(UNIXV)
	struct timeval timeout;
	fd_set rfd;
	char chr;

	if (gotchar != KEYNONE) return('Y');
#endif
#endif
	switch (key_method) {

#if defined(MSDOS)
	case 'B':
		return(BIsKey() ? 'Y' : 'N');
		/*break;*/

	case 'D':
		return(PSystem(0xB) & 0xFF ? 'Y' : 'N');
		/*break;*/

	case 'E':
		return(BIsKeyE() ? 'Y' : 'N');
		/*break;*/

	case 'J':
		return(JIsKey());
		/*break;*/

	case 'P':
		return('?');
		/*break;*/
#endif

#if defined(UNIX)
	case 'S':
	case 'V':
#if defined(UNIXV)
		return('?');
#else	/* assume Berkeley sockets */

		FD_ZERO(&rfd);
		FD_SET(0, &rfd);
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if (select(RFDSIZE, &rfd, 0, 0, &timeout) == 1) {
			read(0, &chr, sizeof(chr));
			gotchar = chr & 0xFF;
			return('Y');
			}
		else	{
			return('N');
			}
#endif
		break;
#endif

#if defined(UNIX)
	case 'T':
#if defined(UNIXV)
		return('?');
#else	/* assume Berkeley sockets */
		FD_ZERO(&rfd);
		FD_SET(0, &rfd);
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if (select(RFDSIZE, &rfd, 0, 0, &timeout) == 1) {
			read(0, &chr, sizeof(chr));
			gotchar = chr & 0xFF;
			return('Y');
			}
		else	{
			return('N');
			}
		break;
#endif
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Put a printed representation of the specified character in a static
buffer.  Return a pointer to the start of the buffer. */

char *
TPrintChar(char c)
	{
	char *optr = printbuf;

	c &= 0xff;
	if (c & 0x80) {
		*optr++ = '~';
		c &= 0x7f;
		}

	if (c < SP || c == DEL) {
		*optr++ = '^';
		c ^= '@';
		}

	*optr++ = c;
	*optr = NUL;
	return(printbuf);
	}


/* ------------------------------------------------------------ */

/* Put the character to the screen, expanding tabs and control
characters. */

void
TPutChar(char chr)
	{
	char *cptr = &tprintrep[(chr & 0xFF) * 4];
	int cnt = *cptr++;

	if (c.g.vis_gray) {
		if (chr == SP || chr == c.g.vis_sp_char) {
			T_RawChar(c.g.vis_sp_char);
			++tcol;
			}
		else if (chr == TAB) {
			cnt = TGetTabWidth(tcol) - 1;
			T_RawChar(c.g.vis_tab_char);
			++tcol;
			while (cnt-- > 0) {
				T_RawChar(SP);
				++tcol;
				}
			}
		else	{
			while (cnt-- > 0) {
				T_RawChar(*cptr++);
				++tcol;
				}
			}
		}
	else	{
		if (chr == TAB) {
			cnt = TGetTabWidth(tcol);
			while (cnt-- > 0) {
				T_RawChar(SP);
				++tcol;
				}
			}
		else	{
			while (cnt-- > 0) {
				T_RawChar(*cptr++);
				++tcol;
				}
			}
		}
	}


/* ------------------------------------------------------------ */

/* Put a printed representation of the specified character in a static
buffer, expanding tabs and control characters.  Return a pointer to
the start of the buffer. */

char *
TPutCharBuf(char chr, int col)
	{
	char *optr = printbuf;
	char *cptr = &tprintrep[(chr & 0xFF) * 4];
	int cnt = *cptr++;

	if (c.g.vis_gray) {
		if (chr == SP || chr == c.g.vis_sp_char) {
			*optr++ = c.g.vis_sp_char;
			}
		else if (chr == TAB) {
			cnt = TGetTabWidth(tcol) - 1;
			*optr++ = c.g.vis_tab_char;
			while (cnt-- > 0) {
				*optr++ = SP;
				}
			}
		else	{
			while (cnt-- > 0) {
				*optr++ = *cptr++;
				}
			}
		}
	else	{
		if (chr == TAB) {
			cnt = TGetTabWidth(tcol);
			while (cnt-- > 0) {
				*optr++ = SP;
				}
			}
		else	{
			while (cnt-- > 0) {
				*optr++ = *cptr++;
				}
			}
		}
	*optr = NUL;
	return(printbuf);
	}


/* ------------------------------------------------------------ */

/* Put the string to the screen, expanding each character as TPutChar.
*/

void
TPutStr(char *str)
	{
	while (*str != NUL) TPutChar(*str++);
	}


/* ------------------------------------------------------------ */

/* Set the horizontal offset.  Set to 0 means no offset used.  When
using, do: TSetPoint, TSetOffset, TPutChar (repeatable),
TSetOffset(0), TSetPoint. */

void
TSetOffset(int col, int o)
	{
	toffset = o;
	tcol = col;
	}


/* ------------------------------------------------------------ */

/* Set the cursor row and column. */

void
TSetPoint(int xrow, int xcol)
	{
	char buf[SMALLBUFFSIZE];

	if (trow == xrow && tcol == xcol) return;

	trow = xrow;
	tcol = xcol;

	switch (cur_method) {

#if defined(MSDOS)
	case MB:
	case MM:
	case M1:
	case M2:
		TForce();
		break;
#endif

	case MV:
		xsprintf(buf, "\033[%d;%dH", trow + 1, tcol + 1);
		T_RawStr(buf);
		break;

#if defined(UNIX)
	case MT:
		move(trow, tcol);
		break;
#endif
		}

	}


/* ------------------------------------------------------------ */

/* Invert the cursor. */

void
T_CurInvert(void)
	{
#if defined(MSDOS)
	int amt;
	int line;
	unsigned int mask;
	unsigned int bits;
	unsigned int word;

	if (toffset != 0 && tcol < toffset) return;

	amt = tcol - toffset;
	if (amt < 0) amt = 0;
	if (amt > tcols) amt = tcols;

	switch (cur_method) {

	case M1:
		scrnptr = scrnbase + trow * 30 * 6 + amt / 2;
		mask = (amt & 1) ? 0x0f : 0xf0;
		*scrnptr ^= mask;
		scrnptr += 30;
		*scrnptr ^= mask;
		scrnptr += 30;
		*scrnptr ^= mask;
		scrnptr += 30;
		*scrnptr ^= mask;
		scrnptr += 30;
		*scrnptr ^= mask;
		scrnptr += 30;
		*scrnptr ^= mask;
		break;

	case M2:
		scrnptr = scrnbase + trow * 30 * 5 + (amt * 3) / 8;
		bits = (amt * 3) & 7;
		mask = 0xe000 >> bits;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word ^= mask;
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word ^= mask;
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word ^= mask;
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word ^= mask;
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word ^= mask;
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		break;
		}
#endif
	}


/* ------------------------------------------------------------ */

/* Send the character to the screen, uninterpreted. */

void
T_RawChar(char chr)
	{
	int amt;
#if defined(MSDOS)
	unsigned char *fnt;
	unsigned int mask;
	unsigned int bits;
	unsigned int word;
	unsigned int inv;
#endif

	if (toffset > 0) {
		amt = tcol - toffset;
		if (amt < 0) amt = 0;
		if (amt > tcols) amt = tcols;
		}
	else	{
		amt = tcol;
		}

	if (tcol < toffset) return;
	switch (cur_method) {

#if defined(MSDOS)
	case MB:
		VidCursor(trow, amt);
		VidChar(chr, cur_attrib);
		break;

	case MM:
		scrnptr = scrnbase + line_offsets[trow] + 2 * amt;
		*scrnptr++ = chr;
		*scrnptr++ = cur_attrib;
		break;

	case M1:
		fnt = tinyfont + 3 * (unsigned char)chr;
		scrnptr = scrnbase + trow * 30 * 6 + amt / 2;

		if (amt & 1) {
			inv = (cur_attrib == tattrrev ||
				cur_attrib == tattrru) ? 0x0F : 0;
			*scrnptr &= 0xf0;
			*scrnptr |= (((*fnt) >> 4) & 0x0f) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0xf0;
			*scrnptr |= ((*fnt++) & 0xf) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0xf0;
			*scrnptr |= (((*fnt) >> 4) & 0x0f) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0xf0;
			*scrnptr |= ((*fnt++) & 0xf) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0xf0;
			*scrnptr |= (((*fnt) >> 4) & 0x0f) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0xf0;
			if (cur_attrib == tattrunder) {
				*scrnptr |= 0x0f;
				}
			else if (cur_attrib == tattrru) {
				}
			else	{
				*scrnptr |= (*fnt & 0xf) ^ inv;
				}
			}
		else	{
			inv = (cur_attrib == tattrrev ||
				cur_attrib == tattrru) ? 0xF0 : 0;
			*scrnptr &= 0x0f;
			*scrnptr |= ((*fnt) & 0xf0) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0x0f;
			*scrnptr |= ((*fnt++) << 4) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0x0f;
			*scrnptr |= ((*fnt) & 0xf0) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0x0f;
			*scrnptr |= ((*fnt++) << 4) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0x0f;
			*scrnptr |= ((*fnt) & 0xf0) ^ inv;
			scrnptr += 30;

			*scrnptr &= 0x0f;
			if (cur_attrib == tattrunder) {
				*scrnptr |= 0xf0;
				}
			else if (cur_attrib == tattrru) {
				}
			else	{
				*scrnptr |= (*fnt << 4) ^ inv;
				}
			}
		break;

	case M2:
		fnt = tinyfont + 3 * (unsigned char)chr;
		inv = (cur_attrib == tattrrev ||
			cur_attrib == tattrru) ? 0x0F : 0;
		scrnptr = scrnbase + trow * 30 * 5 + (amt * 3) / 8;
		bits = (amt * 3) & 7;
		mask = ~(0xe000 >> bits);

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word &= mask;
		word |= ((((*fnt) >> 4) & 0x0f) ^ inv) << (12 - bits);
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word &= mask;
		word |= (((*fnt++) & 0x0f) ^ inv) << (12 - bits);
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word &= mask;
		word |= ((((*fnt) >> 4) & 0x0f) ^ inv) << (12 - bits);
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;


		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word &= mask;
		word |= (((*fnt++) & 0x0f) ^ inv) << (12 - bits);
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		scrnptr += 30;

		word = *scrnptr++ << 8;
		word |= *scrnptr--;
		word &= mask;
		if (cur_attrib == tattrunder) {
			word |= 0x0f << (12 - bits);
			}
		else if (cur_attrib == tattrru) {
			}
		else	{
			word |= ((((*fnt) >> 4) & 0x0f) ^ inv) << (12 - bits);
			}
		*scrnptr++ = word >> 8;
		*scrnptr-- = word & 0xff;
		break;
#endif

	case MV:
		*outptr++ = chr;
		if (outptr >= &outbuf[sizeof(outbuf)]) T_RawFlush();
		break;

#if defined(UNIX)
	case MT:
		addch(chr);
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Flush the output buffer. */

void
T_RawFlush(void)
	{
	if (outptr > outbuf) write(1, outbuf, outptr - outbuf);
	outptr = outbuf;
	}


/* ------------------------------------------------------------ */

/* Send the string to the screen, uninterpreted. */

void
T_RawStr(char *str)
	{
	switch (cur_method) {

	case MV:
#if defined(MSDOS)
	case MB:
	case MM:
	case M1:
	case M2:
#endif
		while (*str != NUL) T_RawChar(*str++);
		break;

#if defined(UNIX)
	case MT:
		addstr(str);
		break;
#endif
		}
	}


/* ------------------------------------------------------------ */

/* Send a VT100 attribute string. */

void
T_VAttr(int new)
	{
	char buf[SMALLBUFFSIZE];

	xsprintf(buf, "\33[%sm",
		(new == tattrnorm) ? "0" :
		  ((new == tattrrev) ? "7" :
		    ((new == tattrunder) ? "4" : "4;7")));
	T_RawStr(buf);
	}


/* end of FTERM.C -- Terminal Interface Routines */
