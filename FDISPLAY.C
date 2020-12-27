/* FDISPLAY.C -- Redisplay and Display Routines

	Written March 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"

#define WINMINHEIGHT	3

static char screen[ROWMAX][COLMAX + 1];		/* screen map */
	/* last non-blank column on the screen */
static int screen_line_used[ROWMAX + 1] = { 0 };
	/* amount of each line of screen that is actually valid */
static int screen_line_len[ROWMAX + 1] = { 0 };
static int point_row;

static char mode_flags[9];

static FLAG need_screen_range;

static int divide;

static FLAG needclear = TRUE;

static char mode_buf[COLMAX + 1];
static FLAG div_shown = FALSE;

void D_ClearScreen(void);
void D_ClearWind(struct window *wptr);
void D_IncrDisplay(FLAG nested_call);
void D_IncrDisplay2(FLAG nested_call);
void D_IncrDisplay3(void);
int D_InnerDisplay(struct window *wptr, struct mark *point);
void D_InnerDisplay2(struct window *wptr, int row);
void D_ScreenRange(FLAG nested_call);

/* ------------------------------------------------------------ */

/* Initialize the display routines, part 1. */

void
DInit1(void)
	{
	int cnt;

	num_windows = 1;		/* one window at the top */
	divide = TMaxRow() - 1;

	for (cnt = 0; cnt < NUMWINDOWS; cnt++) {
		windows[cnt].visible = FALSE;
		windows[cnt].sstart = NULL;
		windows[cnt].send = NULL;
		windows[cnt].point = NULL;
		}

	windows[0].visible = TRUE;
	windows[0].top = 0;
	windows[0].bot = TMaxRow() - 2;
	windows[0].isend = FALSE;
	windows[0].offset = 0;

	cwin = &windows[0];
	need_screen_range = TRUE;
	}


/* ------------------------------------------------------------ */

/* Initialize the display routines, part 2. */

void
DInit2(void)
	{
	windows[0].sstart = BMarkCreate();
	windows[0].send = BMarkCreate();
	windows[0].point = BMarkCreate();
	D_ClearScreen();
	}


/* ------------------------------------------------------------ */

/* Terminate the display routines. */

void
DFini(void)
	{
	TSetPoint(TMaxRow() - 1, 0);
	THiOff();
	TCLEOL();
#if defined(MSDOS)
#if !defined(SYSMGR)
	if (screen_type == 16) TSetPoint(24, 0);
#endif
#endif
	}


/* ------------------------------------------------------------ */

/* Clear the row. */

void
DClear(int start, int stop)
	{
	while (start <= stop) {
		BMarkSetMod(BMarkScreen(start));
		screen_line_len[start] = 0;
		screen_line_used[start++] = TMaxCol() + 1;
		}
	}


/* ------------------------------------------------------------ */

/* Display a message in the echo line. */

void
DEcho(char *msg)
	{
	TSetPoint(TMaxRow() - 1, 0);
	THiOn();

	strncpy(mode_buf, msg, TMaxCol() + 1);
	mode_buf[TMaxCol() - 1] = NUL;
	TPutStr(mode_buf);

	TCLEOL();
	THiOff();
	TForce();
	}


/* ------------------------------------------------------------ */

/* Display a message in the echo line, but don't move the cursor. */

void
DEchoNM(char *msg)
	{
	int row;
	int col;

	row = TGetRow();
	col = TGetCol();
	DEcho(msg);
	TSetPoint(row, col);
	TForce();
	}


/* ------------------------------------------------------------ */

/* Display an error message */

void
DError(int msgnum)
	{
	DErrorStr(Res_String(NULL, RES_MSGS, msgnum));
	}


/* ------------------------------------------------------------ */

/* Display an error message */

void
DErrorStr(char *msg)
	{
	char buf[2 * COLMAX + 1];
	int row;
	int col;

	row = TGetRow();
	col = TGetCol();

	xsprintf(buf, ">>> %s", msg);
	TBell();

	do	{
		DEcho(buf);
		} while (KGetChar() == KEYREGEN);

	DModeLine();
	TSetPoint(row, col);
	TForce();
	}


/* ------------------------------------------------------------ */

/* Set the font to NEW.  If -1, turn off any graphics.  Return the old
font. Do an IncrDisplay if DOINCR is True. */

int
DFont(int new, FLAG doincr)
	{
	int old = screen_type;

	TFini();
	screen_type = new;
	TInit();
	if (new != old) {
		if (num_windows == 1)
			windows[0].bot = TMaxRow() - 2;
		else	DWindOne();
		}
	needclear = FALSE;
	DNewDisplay();
	if (doincr) DIncrDisplay();
	return(old);
	}


/* ------------------------------------------------------------ */

/* Update the display from the buffer */

void
DIncrDisplay(void)
	{
	if (need_screen_range) {
		D_ScreenRange(FALSE);
		DModeLine();
		}
	if (num_windows > 1) {
		if (!div_shown) {
			div_shown = TRUE;
			TSetPoint(divide, 0);
			DRowChar(RES_WINDSPLITCHAR);
			}
		}
	else	div_shown = FALSE;
	D_IncrDisplay(FALSE);
	}


/* ------------------------------------------------------------ */

/* Scroll the window left. */

void
DLeft(void)
	{
	if (!isuarg) {
		cwin->offset -= TMaxCol() - Res_Number(RES_CONF, RES_HOVERLAP);
		uarg = cwin->offset + TMaxCol() / 2;
		if (cwin->offset < 0) {
			cwin->offset = 0;
			uarg = 0;
			}
		}
	if (uarg < 0) uarg = 0;
	BMakeColF(uarg);
	uarg = 0;
	D_ClearWind(cwin);
	}


/* ------------------------------------------------------------ */

/* Force a redisplay of the specified line, but put the data into the
supplied buffer instead of sending it to the display.  The data should
be put as (char,attr) pairs.  The line is TMaxCol() chars wide. */

void
DLine(char *buf, int row)
	{
	int cnt;
	int offset;
	int col;
	int chr;
	char *srptr;
	char *charptr;

	if (num_windows > 1) {
		if (row <= windows[0].bot) {
			offset = windows[0].offset;
			}
		else if (row == divide) {
			chr = *Res_String(NULL, RES_CONF, RES_WINDSPLITCHAR);
			for (cnt = 0; cnt < TMaxCol(); cnt++) {
				*buf++ = chr;
				*buf++ = TAttrRev();
				}
			return;
			}
		else	{
			offset = windows[1].offset;
			}
		}
	else	{
		offset = windows[0].offset;
		}

	BMarkToPoint(cwin->point);
	if (BMarkBuf(BMarkScreen(row)) != cbuf) {
		for (cnt = 0; cnt < TMaxCol(); cnt++) {
			*buf++ = SP;
			*buf++ = TAttrNorm();
			}
		return;
		}
	BPointToMark(BMarkScreen(row));
	col = 0;
	cnt = 0;
	while (!BIsEnd() && (chr = BGetChar()) != NL && chr != SNL) {
		for (charptr = TPutCharBuf(chr, col); *charptr != NUL;
			 charptr++) {
			if (col >= offset) {
				*buf++ = *charptr;
				*buf++ = TAttrNorm();
				if (cnt++ >= TMaxCol()) {
					BPointToMark(cwin->point);
					return;
					}
				}
			col++;
			}
		BMoveBy(1);
		}
	if (c.g.vis_gray && !BIsEnd() && BGetChar() == NL) {
		if (col >= offset) {
			*buf++ = chr;
			*buf++ = TAttrNorm();
			if (cnt++ >= TMaxCol()) {
				BPointToMark(cwin->point);
				return;
				}
			}
		}
	while (cnt++ < TMaxCol()) {
		*buf++ = SP;
		*buf++ = TAttrNorm();
		}
	BPointToMark(cwin->point);
	}


/* ------------------------------------------------------------ */

/* Put the cursor at the WHICHth entry. */

void
DMenu(int which, int *rows, int *cols)
	{
	TSetPoint(rows[which], cols[which]);
	}


/* ------------------------------------------------------------ */

/* Show the menu help. */

void
DMenuHelp(int menu, int entry)
	{
	KEYCODE kbuf[MACROMAX];
	char buf[2 * COLMAX];
	char *cptr = buf;
	KEYCODE *kptr;
	int num;
	int table = 0;
	KEYCODE key = KEYNONE;
	char state = 'a';

	num = Res_KeySeq(kbuf, sizeof(kbuf) / sizeof(kbuf[0]) - 1, menu, entry);
	if (num < 0) return;
	if (num == 2 && kbuf[0] == 256) {
		key = 0;
		table = 3;
		xsprintf(buf, "^U %d ^0", kbuf[1]);
		}
	else	{
		for (kptr = kbuf; kptr < &kbuf[num]; kptr++) {
			xstrcpy(cptr, TabDescr(*kptr, 0));
			cptr += strlen(cptr);
			*cptr++ = SP;
			if (cptr >= &buf[COLMAX]) break;

			switch (state) {

			case 'a':
				if (*kptr == ZCU)		/* skip arg */
					state = 'b';
				else if (*kptr == 0x101)	/* skip ^:...` */
					state = 'c';
				else	state = 'z';		/* start in */
				break;

			case 'b':
				if (!xisdigit(*kptr)) state = 'z';
				break;

			case 'c':
				if (*kptr == '`') state = 'y';
				break;

			case 'y':
				state = 'z';
				break;
				}
			if (state == 'z') {
				if (table > 0) {
					key = *kptr;
					state = NUL;
					}
				else	{
					table = TabTable(*kptr, table);
					if (table == 0 || table == 3) {
						key = *kptr;
						state = NUL;
						}
					}
				}
			}
		*cptr = NUL;
		}

	do	{
		TSetPoint(0, 0);
		TPutStr(key == KEYNONE ? "" : TabHelp(key, table));
		TCLEOL();

		TSetPoint(1, 0);
		TPutStr(buf);
		TCLEOL();

		TSetPoint(2, 0);
		DRowChar(RES_OTHERSPLITCHAR);

		TSetPoint(3, 0);
		} while (KGetChar() == KEYREGEN);
	}


/* ------------------------------------------------------------ */

/* Set up the menu.  Display the menu numbered MENU, starting at entry
FIRST for NUMBER entries.  Put the row, column, and hot key
information in ROWS, COLS, and HOT_CHARS. */

void
DMenuSetup(int *rows, int *cols, char *hot_chars, int menu, int first,
	int number)
	{
	int cnt;
	int chr;
	int row;
	int wid;
	int maxwid;
	int llen;
	int cmdlen;
	char *cmdptr;
	FLAG isand;
	FLAG nexthot;
	char *lptr;
	char *boxchars = Res_String(NULL, RES_CONF, RES_BOXCHARS);
	char dispbuf[(COLMAX + 1) * 2];
	char label[SMALLBUFFSIZE];
	char *cptr;
	unsigned char *rptr;
	int type = Res_Number(menu, 1);

	if (!(type & 0x10)) {	/* not a floating menu */
		float_row = 0;
		float_col = 0;
		}
	else	{
		TSetPoint(float_row, float_col);
		}
	type &= 0x0f;

	switch (type) {

	case 0:
	case 1:
		row = float_row;
		if (type == 1) {
			cptr = dispbuf;
			*cptr++ = boxchars[0];
			*cptr++ = TAttrNorm();
			for (cnt = 1; cnt < TMaxCol() - 1; cnt++) {
				*cptr++ = boxchars[1];
				*cptr++ = TAttrNorm();
				}
			*cptr++ = boxchars[2];
			*cptr   = TAttrNorm();
			TSetPoint(row++, 0);
			TAttrBlock(dispbuf, TMaxCol(), FALSE, 0);
			}		
		for (cnt = first; cnt < number; row++) {
			cptr = dispbuf;
			if (type == 1) {
				*cptr++ = boxchars[3];
				*cptr++ = TAttrNorm();
				}
			for (; cnt < number; cnt++) {
				*cptr++ = SP;
				*cptr++ = TAttrNorm();

				xstrncpy(label, Res_String(NULL, menu,
					cnt * 2 + 2));
				isand = FALSE;
				for (lptr = label, llen = 0; *lptr != NUL;
					 lptr++) {
					if (*lptr == '&')
						isand = TRUE;
					else	llen++;
					}
				if ((cptr - dispbuf) / 2 + llen >
					 TMaxCol() - ((type == 1) ? 3 : 2))
					break;

				*rows++ = row;
				*cols++ = (cptr - dispbuf) / 2;
				nexthot = !isand;
				for (lptr = label, llen = 0; *lptr != NUL;
					 lptr++) {
					if (*lptr == '&') {
						nexthot = TRUE;
						}
					else if (nexthot) {
						*cptr++ = *lptr;
						*hot_chars++ =
							ConvUpper(*lptr);
						*cptr++ = TAttrUnder();
						nexthot = FALSE;
						}
					else	{
						*cptr++ = *lptr;
						*cptr++ = TAttrNorm();
						}
					}

				*cptr++ = SP;
				*cptr++ = TAttrNorm();
				}
			while ((cptr - dispbuf) / 2 < TMaxCol() - 1) {
				*cptr++ = SP;
				*cptr++ = TAttrNorm();
				}
			*cptr++ = (type == 1) ? boxchars[4] : SP;
			*cptr++ = TAttrNorm();
			TSetPoint(row, 0);
			TAttrBlock(dispbuf, TMaxCol(), FALSE, 0);
			}
		TSetPoint(row, 0);

		if (type == 0) {
			chr = *Res_String(NULL, RES_CONF, RES_MENU0SPLITCHAR);
			for (cptr = dispbuf; cptr < &dispbuf[5]; cptr++)
				*cptr = chr;
			xstrcpy(cptr, Res_String(NULL, menu, 0));
			cptr += strlen(cptr);
			while (cptr - dispbuf < TMaxCol()) *cptr++ = chr;
			*cptr = NUL;
			TPutStr(dispbuf);
			}
		else	{
			cptr = dispbuf;
			*cptr++ = boxchars[5];
			*cptr++ = TAttrNorm();
			for (cnt = 1; cnt < TMaxCol() - 1; cnt++) {
				*cptr++ = boxchars[6];
				*cptr++ = TAttrNorm();
				}
			*cptr++ = boxchars[7];
			*cptr   = TAttrNorm();
			TSetPoint(row++, 0);
			TAttrBlock(dispbuf, TMaxCol(), FALSE, 0);
			}		
		break;

	case 2:
		TSetPoint(float_row, float_col);
		THiOn();
		xstrncpy(label, Res_String(NULL, menu, 0));
		for (lptr = label; *lptr == SP; lptr++) ;
		for (cptr = lptr; *cptr != SP; cptr++) ;
		*cptr = NUL;
		TPutStr(lptr);
		THiOff();

		/* find width */

		maxwid = 0;
		for (cnt = first; cnt < number; cnt++) {
			wid = 4;
			for (cptr = Res_String(NULL, menu, cnt * 2 + 2);
				 *cptr != NUL; cptr++) {
				if (*cptr == '|')
					wid += 3;
				else if (*cptr != '&')
					wid++;
				}
			if (wid > maxwid) maxwid = wid;
			}

		if (float_col > TMaxCol() - maxwid) {
			float_col = TMaxCol() - maxwid;
			}

		row = float_row + 1;

		cptr = dispbuf;
		*cptr++ = boxchars[0];
		*cptr++ = TAttrNorm();
		for (cnt = 2; cnt < maxwid; cnt++) {
			*cptr++ = boxchars[1];
			*cptr++ = TAttrNorm();
			}
		*cptr++ = boxchars[2];
		*cptr   = TAttrNorm();
		TSetPoint(row++, float_col);
		TAttrBlock(dispbuf, maxwid, FALSE, 0);

		for (cnt = first; cnt < number; row++, cnt++) {
			cptr = dispbuf;
			*cptr++ = boxchars[3];
			*cptr++ = TAttrNorm();
			*cptr++ = SP;
			*cptr++ = TAttrNorm();

			xstrncpy(label, Res_String(NULL, menu, cnt * 2 + 2));
			isand = FALSE;
			cmdptr = NULL;
			for (lptr = label; *lptr != NUL; lptr++) {
				if (*lptr == '&')
					isand = TRUE;
				else if (*lptr == '|')
					cmdptr = lptr;
				}
			cmdlen = (cmdptr != NULL) ? lptr - cmdptr : 0;

			*rows++ = row;
			*cols++ = float_col + 2;

			nexthot = !isand;
			for (lptr = label, llen = 0; *lptr != NUL; lptr++) {
				if (*lptr == '&') {
					nexthot = TRUE;
					}
				else if (nexthot) {
					*cptr++ = *lptr;
					*hot_chars++ = ConvUpper(*lptr);
					*cptr++ = TAttrUnder();
					nexthot = FALSE;
					llen++;
					}
				else if (*lptr == '|') {
					while (llen < maxwid - 3 - cmdlen) {
						*cptr++ = SP;
						*cptr++ = TAttrNorm();
						llen++;
						}
					}
				else	{
					*cptr++ = *lptr;
					*cptr++ = TAttrNorm();
					llen++;
					}
				}

			while (llen++ < maxwid - 3) {
				*cptr++ = SP;
				*cptr++ = TAttrNorm();
				}
			*cptr++ = boxchars[4];
			*cptr++ = TAttrNorm();
			TSetPoint(row, float_col);
			TAttrBlock(dispbuf, maxwid, FALSE, 0);
			}

		cptr = dispbuf;
		*cptr++ = boxchars[5];
		*cptr++ = TAttrNorm();
		for (cnt = 2; cnt < maxwid; cnt++) {
			*cptr++ = boxchars[6];
			*cptr++ = TAttrNorm();
			}
		*cptr++ = boxchars[7];
		*cptr   = TAttrNorm();
		TSetPoint(row++, float_col);
		TAttrBlock(dispbuf, maxwid, FALSE, 0);
		break;
		}
	TSetPoint(TGetRow() + 1, 0);
	}


/* ------------------------------------------------------------ */

/* Display the mode flags */

void
DModeFlags(void)
	{
	long loc;
	long len;
	int pct;
	char buf[LINEBUFFSIZE];

 	if (KIsKey() == 'Y') return;

	loc = BGetLocation();
	len = BGetLength(cbuf);
	if (len == 0) len = 1;
	pct = (loc * 100) / len;

	xsprintf(buf, "%3d%% %c%c%c",
			pct,
			BIsMod(cbuf) ? '*' : ' ',
			TabIsDelete(lastkey, lasttable) ? '+' : ' ',
			togpgln ? '^' : ' ');

	if (!strequ(mode_flags, buf)) {
		TSetPoint(TMaxRow() - 1, 7);
		THiOn();
		TPutStr(buf);
		THiOff();
		xstrcpy(mode_flags, buf);
		}
	}


/* ------------------------------------------------------------ */

/* Display the mode line */

void
DModeLine(void)
	{
	char buf[2 * COLMAX + 1];

	TSetPoint(TMaxRow() - 1, 0);
	xsprintf(buf, Res_String(NULL, RES_MSGS, RES_FREYJAVER),
		cbuf->c.tab_spacing,
		cbuf->c.right_margin,
		Res_String(NULL, RES_MSGS,
			(cbuf->c.fill == 'N') ? RES_DISPNONE :
			(cbuf->c.fill == 'F') ? RES_DISPFILL : RES_DISPWRAP),
		cbuf->fname);
	TCLEOL();
	*mode_flags = NUL;
	DEcho(buf);
	DModeFlags();
	}


/* ------------------------------------------------------------  */

/* Put a new display on the screen. */

void
DNewDisplay(void)
	{
	if (needclear) TClrScreen();
	needclear = TRUE;
	D_ClearScreen();
	D_ScreenRange(FALSE);
	div_shown = FALSE;
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Return the current preferred line. */

int
DPrefLine(void)
	{
	return((Res_Number(RES_CONF, RES_PREFPCT) * DWindHeight()) / 100);
	}


/* ------------------------------------------------------------ */

/* Scroll the window right. */

void
DRight(void)
	{
	if (!isuarg) {
		cwin->offset += TMaxCol() - Res_Number(RES_CONF, RES_HOVERLAP);
		uarg = cwin->offset + TMaxCol() / 2;
		}
	if (uarg < 0) uarg = 0;
	BMakeColF(uarg);
	uarg = 0;
	D_ClearWind(cwin);
	}


/* ------------------------------------------------------------ */

/* Display a row of the specified character. */

void
DRowChar(int which)
	{
	int chr = *Res_String(NULL, RES_CONF, which);
	int cnt;

	THiOn();
	for (cnt = 0; cnt < TMaxCol(); ++cnt) TPutChar(chr);
	THiOff();
	}


/* ------------------------------------------------------------ */

/* Center redisplay */

void
DScreenRange(void)
	{
	need_screen_range = TRUE;
	}


/* ------------------------------------------------------------ */

/* Show the multi-line message starting at WHERE (T=top of screen,
M=after menu bar).  Turn on highlighting if HILIT is True. */

void
DShow(char where, char *str, FLAG hilit)
	{
	unsigned char buf[BUFFSIZE];
	unsigned char *cptr;
	unsigned char *mptr;
	int start_row = where == 'M' ? float_row : 0;
	int row;

	row = start_row;

	if (hilit) THiOn();
	for (mptr = str; *mptr != NUL;
		 mptr = (*cptr == NL) ? (cptr + 1) : cptr) {
		cptr = (unsigned char *)sindex(mptr, NL);
		memmove(buf, mptr, cptr - mptr);
		buf[cptr - mptr] = NUL;

		TSetPoint(row++, 0);
		TPutStr(buf);
		TCLEOL();
		if (row >= TMaxRow() - 1) break;
		}
	if (!hilit && row < TMaxRow() - 1) {
		TSetPoint(row++, 0);
		DRowChar(RES_OTHERSPLITCHAR);
		}
	TSetPoint(row, 0);
	TForce();
	if (hilit) THiOff();
	DClear(start_row, TGetRow());
	}


/* ------------------------------------------------------------ */

/* Toggle the visible gray flag. */

void
DTogVisGray(void)
	{
	c.g.vis_gray = !c.g.vis_gray;
	DNewDisplay();
	}


/* ------------------------------------------------------------ */

/* Display an informative message in the message area and wait for a
key press. */

void
DView(char *msg)
	{
	int row;
	int col;

	row = TGetRow();
	col = TGetCol();
	do	{
		DEcho(msg);
		} while (KGetChar() == KEYREGEN);
	DModeLine();
	TSetPoint(row, col);
	TForce();
	}


/* ------------------------------------------------------------ */

/* Grow the current window. */

void
DWindGrow(void)
	{
	int height;

	if (num_windows <= 1) return;

	DClear(divide, divide);
	if (cwin > &windows[0]) {	/* move divider up */
		height = (cwin - 1)->bot - (cwin - 1)->top + 1;
		if (uarg > height - WINMINHEIGHT) uarg = height - WINMINHEIGHT;
		if (uarg >= 1) {
			DClear((cwin - 1)->bot - uarg + 1, (cwin - 1)->bot);
			divide -= uarg;
			(cwin - 1)->bot -= uarg;
			cwin->top -= uarg;
			}
		}
	else	{			/* move divider down */
		height = (cwin + 1)->bot - (cwin + 1)->top + 1;
		if (uarg > height - WINMINHEIGHT) uarg = height - WINMINHEIGHT;
		if (uarg >= 1) {
			DClear((cwin + 1)->top, (cwin + 1)->top + uarg - 1);
			divide += uarg;
			cwin->bot += uarg;
			(cwin + 1)->top += uarg;
			}
		}
	div_shown = FALSE;
	DModeLine();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Return the height of the current window */

int
DWindHeight(void)
	{
	return(cwin->bot - cwin->top + 1);
	}


/* ------------------------------------------------------------ */

/* Switch to one window mode. */

void
DWindOne(void)
	{
	struct window *wptr;

	if (num_windows <= 1) return;

	wptr = (cwin == &windows[0]) ? &windows[1] : &windows[0];
	wptr->visible = FALSE;

	BMarkDelete(wptr->sstart);
	BMarkDelete(wptr->send);
	BMarkDelete(wptr->point);

	D_ClearWind(wptr);
	DClear(divide, divide);

	num_windows = 1;
	cwin->top = 0;
	cwin->bot = TMaxRow() - 2;
	DScreenRange();
	}


/* ------------------------------------------------------------ */

/* Switch which window we are in. */

void
DWindSwap(void)
	{
	if (num_windows <= 1) return;

	BMarkToPoint(cwin->point);
	cwin = (cwin == &windows[0]) ? &windows[1] : &windows[0];
	BPointToMark(cwin->sstart);
	BMarkToPoint(cwin->send);
	BPointToMark(cwin->point);
	cwin->isend = FALSE;
	}


/* ------------------------------------------------------------ */

/* Toggle between one and two window mode. */

void
DWindTog(void)
	{
	if (num_windows == 1)
		DWindTwo();
	else	DWindOne();
	}


/* ------------------------------------------------------------ */

/* Switch to two window mode. */

void
DWindTwo(void)
	{
	struct window *wptr;

	if (num_windows > 1) return;

	wptr = (cwin == &windows[0]) ? &windows[1] : &windows[0];
	wptr->visible = TRUE;

	divide = (TMaxRow() - 2) / 2;
	cwin->top = 0;
	cwin->bot = divide - 1;
	wptr->top = divide + 1;
	wptr->bot = TMaxRow() - 2;
	wptr->offset = cwin->offset;

	D_ClearWind(wptr);

	wptr->point = BMarkCreate();
	BMarkSwap(cwin->sstart);
	wptr->sstart = BMarkCreate();
	wptr->send = BMarkCreate();
	wptr->isend = FALSE;
	BMarkSwap(cwin->sstart);

	num_windows = 2;
	DScreenRange();
	}


/* ------------------------------------------------------------ */

/* Switch to two window mode, but go to the other window. */

void
DWindTwoO(void)
	{
	DWindTwo();
	DWindSwap();
	}


/* ------------------------------------------------------------ */

/* Clear the screen. */

void
D_ClearScreen(void)
	{
	DClear(0, TMaxRow() - 1);
	}


/* ------------------------------------------------------------ */

/* Clear the window. */

void
D_ClearWind(struct window *wptr)
	{
	DClear(wptr->top, wptr->bot);
	}


/* ------------------------------------------------------------ */

/* Actually update the display from the buffer */

void
D_IncrDisplay(FLAG nested_call)
	{
	struct window *wptr;
	int cnt;
	FLAG ischanged = FALSE;

	cnt = BGetCol();
	while (cnt < cwin->offset) {
		cwin->offset -= TMaxCol() - Res_Number(RES_CONF, RES_HOVERLAP);
		if (cwin->offset < 0) cwin->offset = 0;
		ischanged = TRUE;
		}
	while (cnt >= cwin->offset + TMaxCol()) {
		cwin->offset += TMaxCol() - Res_Number(RES_CONF, RES_HOVERLAP);
		ischanged = TRUE;
		}
	if (ischanged) D_ClearWind(cwin);

	BPushState();
	if (BMarkBuf(cwin->sstart) != cbuf || BIsBeforeMark(cwin->sstart) ||
		 (cwin->isend && BMarkBuf(cwin->send) == cbuf &&
		 !BIsBeforeMark(cwin->send))) {
		D_ScreenRange(nested_call);
		}

	BMarkToPoint(cwin->point);
	BPointToMark(cwin->sstart);
	cwin->isend = FALSE;
	point_row = D_InnerDisplay(cwin, cwin->point);

	if (BIsBeforeMark(cwin->point) && KIsKey() != 'Y') {
		D_IncrDisplay2(nested_call);
		}
	else	{
		if (num_windows > 1) {
			wptr = (cwin == &windows[0]) ? &windows[1] :
				&windows[0];
			BPointToMark(wptr->sstart);
			D_InnerDisplay(wptr, NULL);
			}
		D_IncrDisplay3();
		}
	}


/* ------------------------------------------------------------ */

/* Inner part of IncrDisplay. */

void
D_IncrDisplay2(FLAG nested_call)
	{
	BPointToMark(cwin->point);
	D_ScreenRange(nested_call);
	BPopState();
	D_IncrDisplay(TRUE);
	}


/* ------------------------------------------------------------ */

/* Inner part of IncrDisplay. */

void
D_IncrDisplay3(void)
	{
	int col;

	BPointToMark(cwin->point);
	BPopState();
	DModeFlags();
	col = BGetCol() - cwin->offset;
	if (col < 0) col = 0;
	if (col >= TMaxCol()) col = TMaxCol() - 1;
	TSetPoint(point_row, col);
	}


/* ------------------------------------------------------------ */

/* Display one window. */

int
D_InnerDisplay(struct window *wptr, struct mark *point)
	{
	struct mark *mptr;
	int row;
	FLAG need_pnt = TRUE;

#if defined(MSDOS)
	if (c.g.madefor == 'C' || c.g.madefor == 'D' || c.g.madefor == 'J' ||
		c.g.madefor == 'S') JDisStart();
#endif
	for (row = wptr->top; row <= wptr->bot && KIsKey() != 'Y'; ++row) {
		mptr = BMarkScreen(row);
		if (BMarkBuf(mptr) != cbuf || BMarkIsMod(mptr) ||
			 !BIsAtMark(mptr)) {
			BMarkToPoint(mptr);
			D_InnerDisplay2(wptr, row);
			}
		else	BPointToMark(BMarkScreen(row + 1));

		if (point != NULL && need_pnt && BMarkBuf(point) == cbuf &&
			 BIsAfterMark(point)) {
			point_row = row;
			need_pnt = FALSE;
			}
		}

	mptr = BMarkScreen(row);
	if (BMarkBuf(mptr) != cbuf || !BIsAtMark(mptr)) BMarkSetMod(mptr);
	BMarkToPoint(mptr);
	if (point != NULL) {
		if (KIsKey() != 'Y') {
			wptr->isend = TRUE;
			BMarkToPoint(wptr->send);
			}
		if (need_pnt) point_row = wptr->bot + 1;
		}
#if defined(MSDOS)
	if (c.g.madefor == 'C' || c.g.madefor == 'D' || c.g.madefor == 'J' ||
		c.g.madefor == 'S') JDisEnd();
#endif
	return(point_row);
	}


/* ------------------------------------------------------------ */

/* Inner loop of InnerDisplay. */

void
D_InnerDisplay2(struct window *wptr, int row)
	{
	int maxoff = TMaxCol() + wptr->offset;
	int col;
	int chr;
	char *srptr;
	char *cptr;
	int *sllptr;

	srptr = screen[row];
	sllptr = &screen_line_len[row];

	cptr = srptr;
	col = 0;

	while (cptr - srptr < *sllptr && !BIsEnd() &&
		 (chr = BGetChar()) != NL && chr != SNL) {
		if (col >= wptr->offset) {
			if (chr != *cptr) break;
			cptr++;
			}
		col += (chr == TAB) ? TGetTabWidth(col) : TGetWidth(chr);
		BMoveBy(1);
		}

	if (wptr->offset > 0)
		TSetPoint(row, col > wptr->offset ? col % wptr->offset : 0);
	else	TSetPoint(row, col);
	TSetOffset(col, wptr->offset);

	while (!BIsEnd() && (chr = BGetChar()) != NL && chr != SNL &&
		 col < maxoff) {
		if (col >= wptr->offset) *cptr++ = chr;
		TPutChar(chr);
		col += (chr == TAB) ? TGetTabWidth(col) : TGetWidth(chr);
		BMoveBy(1);
		}
	if (c.g.vis_gray && !BIsEnd() && BGetChar() == NL && col < maxoff) {
		if (col >= wptr->offset) *cptr++ = c.g.vis_nl_char;
		TPutChar(c.g.vis_nl_char);
		col += TGetWidth(c.g.vis_nl_char);
		}

	TSetOffset(0, 0);

	if (wptr->offset > 0) {
		if (col < wptr->offset) {
			col = 0;
			}
		else	{
			col -= wptr->offset;
			if (col < 0) col = 0;
			if (col > TMaxCol()) col = TMaxCol();
			}
		}

	TSetPoint(row, col);

	if (col < screen_line_used[row] || *sllptr > cptr - srptr) TCLEOL();
	screen_line_used[row] = col;
	*sllptr = cptr - srptr;

	if (TGetCol() < TMaxCol()) {
		if (BIsEnd())
			BInvalid();
		else if (!IsNL())
			SearchNLF();
		else	BMoveBy(1);
		}
	else	{
		if (!IsNL())
			SearchNLF();
		else	BMoveBy(1);
		}
	}


/* ------------------------------------------------------------ */

/* Do the work for centering the display. */

void
D_ScreenRange(FLAG nested_call)
	{
	int cnt;
	int pref = DPrefLine();

	need_screen_range = FALSE;
	cwin->isend = FALSE;

	BMarkToPoint(cwin->point);
	if (nested_call) {
		for (cnt = 0; cnt < pref + 1 && SearchNLB(); ++cnt) ;
		}
	else	{
		for (cnt = 0; cnt < pref; ++cnt) SearchNLB();
		}
	CLineA();
	BMarkToPoint(cwin->sstart);
	BPointToMark(cwin->point);
	}


/* end of FDISPLAY.C -- Redisplay and Display Routines */
