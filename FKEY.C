/* FKEY.C -- Key Input Routines

	Written March 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"

static struct buffer *buf_buf = NULL;
static struct mark *buf_pt = NULL;

static KEYCODE resmac_buf[MACROMAX] = { KEYNONE };
static KEYCODE *resmac_ptr;
static int resmac_count;
static int resmac_uarg = 0;
static FLAG resmac_menu = TRUE;

static KEYCODE mac_buf[MACROMAX] = { KEYNONE };
static KEYCODE *mac_ptr = mac_buf;
static KEYCODE *mac_menu_ptr = NULL;
static FLAG mac_record = FALSE;
static int mac_arg = 0;

static KEYCODE pushed = KEYNONE;

static char *str_ptr = NULL;
static int str_len = 0;

	/* holds last menus for backing up */
#define MENU_STACK_SIZE 5
static int menu_stack[MENU_STACK_SIZE];
static int menu_which[MENU_STACK_SIZE];
static int menu_stack_ptr = -1;

void K_GetLine(char *buf, int len);
void K_GetStrComp(FLAG istab, char *fname, char *keybuf);

/* ------------------------------------------------------------ */

/* Ask a yes/no question in the echo line.  Return KEYQUIT, KEYABORT,
'Y', or 'N'. */

int
KAsk(int msgnum)
	{
	int chr;

	chr = KEYREGEN;
	for (;;) {
		DEcho(Res_String(NULL, RES_MSGS, msgnum));
		chr = ConvUpper(KGetChar());
		if (chr == KEYQUIT) {
			DModeLine();
			return(KEYQUIT);
			}
		else if (chr == KEYREGEN) {
			}
		else if (chr == KEYABORT || chr == ESC || chr == BEL) {
			DModeLine();
			return(KEYABORT);
			}
		else if (*sindex(Res_String(NULL, RES_MSGS, RES_YESSTRING),
				chr) != NUL) {
			DModeLine();
			return('Y');
			}
		else if (*sindex(Res_String(NULL, RES_MSGS, RES_NOSTRING),
				chr) != NUL) {
			DModeLine();
			return('N');
			}
		else	{
			TBell();
			}
		}
	}


/* ------------------------------------------------------------ */

/* begin defining keyboard macro */

void
KBegMac(void)
	{
	uarg = 0;
	if (mac_arg > 0) {
		DError(RES_MACROUSE);
		return;
		}
	mac_ptr = mac_buf;
	mac_menu_ptr = NULL;
	mac_record = TRUE;
	}


/* ------------------------------------------------------------ */

/* Do the ^: command. */

void
KColon(void)
	{
	char buf[COLMAX + 1];
	char resp[COLMAX + 1];
	char *cptr = buf;
	int num;
	KEYCODE chr;

	while ((chr = KGetChar()) != '`' && cptr < &buf[sizeof(buf) - 1]) {
		*cptr++ = chr;
		}
	*cptr = NUL;

	resmac_uarg = 0;	/* turn off buffered menu processing */

	while (KGetStr2(buf, resp, sizeof(resp), FALSE) == 'Y') {
		if (!SToN(resp, &num, 10)) {
			DError(RES_ERRNONNUM);
			}
		else	{
			isuarg = TRUE;
			uarg = num;
			resmac_uarg = 1;	/* menus must always be 1 */
			table = 0;
			key = KGetChar();
			TabDispatch(key, table);
			return;
			}
		}
	}


/* ------------------------------------------------------------ */

/* Echo msg if no char is typed within interval return true if msg
printed, else false */

FLAG
KDelayPrompt(int msgnum)
	{
	int cnt;

	for (cnt = 0; cnt < DELAYCOUNT; cnt++) {
		if (KIsKey() == 'Y') return(FALSE);
		}
	DEchoNM(Res_String(NULL, RES_MSGS, msgnum));
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* finish defining keyboard macro */

void
KEndMac(void)
	{
	uarg = 0;
	mac_record = FALSE;
	if (isuarg && mac_menu_ptr != NULL)
		mac_ptr = mac_menu_ptr - 1;
	else	mac_ptr -= 2;
	if (mac_ptr < mac_buf) mac_ptr = mac_buf;
	*mac_ptr = KEYNONE;
	}


/* ------------------------------------------------------------ */

/* Switch input to be from the specified buffer.  The entire buffer is
read, starting from the beginning.  The point is preserved. */

void
KFromBuf(struct buffer *bptr)
	{
	struct buffer *savebuf = cbuf;

	BBufGoto(bptr);
	if ((buf_pt = BMarkCreate()) != NULL) {
		buf_buf = bptr;
		BMoveToStart();
		}
	BBufGoto(savebuf);
	}


/* ------------------------------------------------------------ */

/* do keyboard macro */

void
KFromMac(void)
	{
	if (mac_record) {
		DError(RES_MACROCREATE);
		uarg = 0;
		return;
		}
	mac_arg = uarg;
	mac_ptr = mac_buf;
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Switch input to be from the supplied string. */

void
KFromStr(char *str, int len)
	{
	if (str == NULL || len <= 0) {
		str_len = 0;
		}
	else	{
		str_ptr = str;
		str_len = len;
		}
	}


/* ------------------------------------------------------------ */

/* Get a character and handle keyboard macros */

KEYCODE
KGetChar(void)
	{
	KEYCODE chr;

	if (pushed != KEYNONE) {
		chr = pushed;
		pushed = KEYNONE;
		return(chr);
		}
	if (str_len > 0) {
		str_len--;
		return(*str_ptr++);
		}
	while (resmac_uarg > 0) {
		chr = *resmac_ptr++;
		if (chr != KEYNONE) return(chr);
		resmac_uarg--;
		resmac_ptr = resmac_buf;
		if (!resmac_menu) {
			DIncrDisplay();
			TForce();
			}
		resmac_menu = FALSE;
		}
	if (buf_buf != NULL) {
		struct buffer *savebuf = cbuf;

		BBufGoto(buf_buf);
		if (!BIsEnd()) {	/* get next char */
			chr = BGetCharAdv();
			BBufGoto(savebuf);
			return(chr);
			}
		BPointToMark(buf_pt);
		BMarkDelete(buf_pt);
		buf_buf = NULL;
		BBufGoto(savebuf);
		}
	while (mac_arg > 0) {
		chr = *mac_ptr++;
		if (chr != KEYNONE) return(chr);
		mac_arg--;
		mac_ptr = mac_buf;
		DIncrDisplay();
		TForce();
		}
	TForce();
	chr = TGetKey();

	if (c.g.ESC_swap != ESC) {
		if (chr == ESC) chr = c.g.ESC_swap;
		else if (chr == c.g.ESC_swap) chr = ESC;
		}
	if (c.g.CTX_swap != ZCX) {
		if (chr == ZCX) chr = c.g.CTX_swap;
		else if (chr == c.g.CTX_swap) chr = ZCX;
		}
	KMacRec(chr);
	return(chr);
	}


/* ------------------------------------------------------------ */

/* Input a file name into FNAME.  FNAME must be FNAMEMAX characters
long. Return KEYQUIT, KEYABORT, or 'Y' (if ok).  If useDIRED is True,
work specially if you're in a DIRED buffer. */

int
KGetFile(int msgnum, char *fname, FLAG useDIRED)
	{
	char buf[BUFFSIZE];
	char dir[FNAMEMAX];

	if (useDIRED && strequ(cbuf->fname,
		 Res_String(NULL, RES_MSGS, RES_SYSDIRED))) {
		K_GetLine(fname, FNAMEMAX);
		return('Y');
		}

#if defined(SYSMGR)
	if (c.g.madefor == 'S' && Res_Char(RES_CONF, RES_USEGETTER) == 'G') {
		return(JGetFile(msgnum, fname) ? 'Y' : KEYABORT);
		}
#endif
	FGetDir(dir);
	xsprintf(buf, "%s [%s] ", Res_String(NULL, RES_MSGS, msgnum), dir);

	return(KGetStr2(buf, fname, FNAMEMAX, TRUE));
	}


/* ------------------------------------------------------------ */

/* Input a string argument. Return KEYQUIT, KEYABORT, or 'Y' (if ok). */

int
KGetStr(int msgnum, char *str, int len)
	{
	return(KGetStr2(Res_String(NULL, RES_MSGS, msgnum), str, len, FALSE));
	}


/* ------------------------------------------------------------ */

/* Input a string argument. Return KEYQUIT, KEYABORT, or 'Y' (if ok). */

int
KGetStr2(char *msg, char *str, int len, FLAG isfile)
	{
	char sbuf[BIGBUFFSIZE];
	char keybuf[BIGBUFFSIZE];
	char buf[BIGBUFFSIZE];
	KEYCODE c;
	int amt;
	int retval;
	FLAG wastext = xprintf_get_text();
	FLAG washelp = FALSE;

	xprintf_set_text(FALSE);
	*sbuf = NUL;
	c = KEYREGEN;
	for (retval = KEYNONE; retval == KEYNONE; ) {
		xsprintf(buf, "%s: %s", msg, sbuf);
		amt = strlen(sbuf);
		if (c == KEYREGEN || KIsKey() != 'Y') DEcho(buf);
		c = KGetChar();
		switch (c) {

		case KEYQUIT:
			retval = KEYQUIT;
			break;

		case KEYREGEN:
			break;

		case KEYABORT:
		case ESC:
		case BEL:
			retval = KEYABORT;
			break;

		case CR:
			retval = 'Y';
			break;

		case BS:
		case DEL:
			if (*sbuf != NUL) sbuf[amt - 1] = NUL;
			break;

		case ZCU:
			*sbuf = NUL;
			break;

#if defined(MSDOS)
		case 0x100 + 97:	/* Ctrl-F4 */
		case 0x100 + 214:	/* PASTE */
#endif
		case ZCY:
			KFromBuf(kill_buf);
			break;

		case TAB:
		case '?':
		case KEYHELP:
			if (isfile) {
				K_GetStrComp(c == TAB, sbuf, keybuf);
				washelp = TRUE;
				break;
				}

		default:
			if (amt >= len - 1) {
				sbuf[amt - 1] = NUL;
				TBell();
				}

			if (c == ZCQ) c = KGetChar();
			sbuf[amt] = c;
			sbuf[amt + 1] = NUL;
			break;
			}
		}
	if (retval != KEYQUIT && retval != KEYABORT && *sbuf != NUL)
		xstrcpy(str, sbuf);
	xprintf_set_text(wastext);
	if (washelp) DWindOne();
	DModeLine();
	return(retval);
	}


/* ------------------------------------------------------------ */

/* Is key available from macro? */

char
KIsKey(void)
	{
	if (KMacIs() == 'Y') return('Y');
	return(TIsKey());
	}


/* ------------------------------------------------------------ */

/* Load and save the keyboard macro buffer. */

void
KLoadMac(void)
	{
	int cnt;

	if (!isuarg) {	/* save into buffer */
		for (cnt = 0; cnt < MACROMAX && mac_buf[cnt] != KEYNONE;
			 cnt++) {
			BInsChar(mac_buf[cnt]);
			}
		}
	else if (uarg == 4) {	/* load from buffer */
		BMarkToPoint(cwin->point);
		BMoveToStart();
		for (cnt = 0; cnt < MACROMAX - 1 && !BIsEnd(); cnt++) {
			mac_buf[cnt] = BGetCharAdv();
			}
		mac_buf[cnt] = KEYNONE;
		BPointToMark(cwin->point);
		}
	else	{	/* load from resource file */
		cnt = Res_KeySeq(mac_buf,
			sizeof(mac_buf) / sizeof(mac_buf[0]) - 1,
			RES_KEYM, uarg);
		if (cnt < 0) {
			mac_buf[0] = KEYNONE;
			return;
			}
		mac_buf[cnt] = KEYNONE;
		mac_ptr = mac_buf;
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Load and execute a macro from the resource file.  Overwrites the
menu macro. */

void
KMacDo(int table, int offset)
	{
	resmac_uarg = uarg;
	resmac_count = Res_KeySeq(resmac_buf,
		sizeof(resmac_buf) / sizeof(resmac_buf[0]) - 1, table, offset);
	if (resmac_count < 0) {
		resmac_buf[0] = KEYNONE;
		return;
		}
	resmac_buf[resmac_count] = KEYNONE;
	resmac_ptr = resmac_buf;
	}


/* ------------------------------------------------------------ */

/* As KIsKey, but check for everything but terminal input. */

char
KMacIs(void)
	{
	if (pushed != KEYNONE) return('Y');
	if (str_len > 0) return('Y');
	if (resmac_uarg > 0 && *resmac_ptr != KEYNONE) return('Y');
	if (buf_buf != NULL) {
		struct buffer *savebuf = cbuf;

		BBufGoto(buf_buf);
		if (!BIsEnd()) {
			BBufGoto(savebuf);
			return('Y');
			}
		BPointToMark(buf_pt);
		BMarkDelete(buf_pt);
		buf_buf = NULL;
		BBufGoto(savebuf);
		}
	if (!mac_record && mac_arg > 0) return('Y');
	return('N');
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the start of the keyboard macro. */

KEYCODE *
KMacPtr(void)
	{
	return(mac_buf);
	}


/* ------------------------------------------------------------ */

/* Record a macro keystroke. */

void
KMacRec(KEYCODE key)
	{
	if (mac_record) {
		*mac_ptr++ = key;
		if (mac_ptr > &mac_buf[MACROMAX - 1]) {
			DError(RES_MACROFULL);
			mac_ptr--;
			}
		*mac_ptr = KEYNONE;
		}
	}


/* ------------------------------------------------------------ */

/* Handle the specified menu. */

void
KMenu(int menu)
	{
	int cols[RESMAXENTRIES];
	int rows[RESMAXENTRIES];
	int kbuf[MACROMAX];
	char hot_chars[RESMAXENTRIES];
	char buf[(COLMAX + 1) * 2];
	KEYCODE chr;
	int which;
	int cntmenu;
	int number;
	int cnt;
	int w;
	int type;
	FLAG washelp = FALSE;
	FLAG needdisp;
	FLAG needshow;
	int last_row = 0;
	int snap_row = 0;

	menu_stack_ptr = -1;
again:
	which = 0;
	needdisp = TRUE;
	needshow = FALSE;
	if (menu == 0) {
		KMenuMac();
		menu = Res_Number(Res_Number(-1, -1) - 1, 0);
		float_row = 0;
		float_col = 0;
		menu_stack_ptr = 0;
		}
	else if (menu == 1) {
		float_row = 0;
		float_col = 0;
		needshow = TRUE;
		washelp = TRUE;
		menu = Res_Number(Res_Number(-1, -1) - 1, 1);
		if (menu_stack_ptr < MENU_STACK_SIZE) menu_stack_ptr++;
		}
	else	{
		if (menu_stack_ptr < MENU_STACK_SIZE) menu_stack_ptr++;
		}
	menu_stack[menu_stack_ptr] = menu;
	menu_which[menu_stack_ptr] = 0;
	uarg = 0;
	isuarg = FALSE;

	chr = NUL;
	for (;;) {
		if (needdisp) {
			DClear(0, last_row);

			for (cnt = 0; cnt <= menu_stack_ptr; cnt++) {
				cntmenu = menu_stack[cnt];
				number = (Res_Number(cntmenu, -1) - 2) / 2;
				type = Res_Number(cntmenu, 1) & 0x0f;
				which = menu_which[cnt];
				DMenuSetup(rows, cols, hot_chars, cntmenu, 0,
					number);
				snap_row = TGetRow();
				DMenu(which, rows, cols);

				if (type == 0) {
					float_row = snap_row;
					float_col = 0;
					}
				else if (type == 1) {
					float_row = rows[which];
					float_col = cols[which];
					}
				else if (type == 2) {
					float_row = rows[which] - which - 1;
					float_col = cols[which] + 3;
					}
				}

			TSetPoint(snap_row, 0);
			while (TGetRow() <= last_row) {
				DLine(buf, TGetRow());
				TAttrBlock(buf, TMaxCol(), FALSE, 0);
				TSetPoint(TGetRow() + 1, 0);
				}
			if (last_row >= TMaxRow() - 1) DModeLine();
			last_row = snap_row;
			needdisp = FALSE;

			if (needshow) {
				cnt = float_row;
				float_row = snap_row;
				DShow('M', Res_String(NULL, RES_MSGS,
					TMaxCol() <= 40 ? RES_FKEYHELP40 :
					RES_FKEYHELP), TRUE);
				float_row = cnt;
				}
			}
		DMenu(which, rows, cols);

		chr = ConvUpper(KGetChar());
		if (c.g.ctype[chr] & CSETMASK_TOK) {
			w = which;
			for (cnt = 0; cnt < number; cnt++) {
				if (chr == hot_chars[w]) {
					which = w;
					chr = CR;
					break;
					}
				if (++w >= number) w = 0;
				}
			}
		menu_which[menu_stack_ptr] = which;
		switch (chr) {

		case KEYQUIT:
			MExit();
			return;
			/*break;*/

		case KEYABORT:
		case BEL:
		case ESC:
			if (menu_stack_ptr == 0) {
				DClear(0, last_row);
				for (cnt = 0; cnt <= last_row; cnt++) {
					DLine(buf, cnt);
					TSetPoint(cnt, 0);
					TAttrBlock(buf, TMaxCol(), FALSE, 0);
					}
				if (washelp) DNewDisplay();
				return;
				}
			menu = menu_stack[--menu_stack_ptr];
			which = menu_which[menu_stack_ptr];
			needdisp = TRUE;
			break;

		case CR:
			if (type == 0) {
				float_row = last_row;
				float_col = 0;
				}
			else if (type == 1) {
				float_row = rows[which];
				float_col = cols[which];
				}
			else if (type == 2) {
				float_row = rows[which] - which - 1;
				float_col = cols[which] + 3;
				}

			cnt = Res_KeySeq(kbuf,
				sizeof(kbuf) / sizeof(kbuf[0]) - 1,
				menu,
				which * 2 + 3);
			if (cnt == 2 && kbuf[0] == 256) {
				menu = kbuf[1];
				goto again;
				}
			else	{
				DClear(0, last_row);
				for (cnt = 0; cnt < last_row; cnt++) {
					DLine(buf, cnt);
					TSetPoint(cnt, 0);
					TAttrBlock(buf, TMaxCol(), FALSE, 0);
					}
				if (last_row >= TMaxRow() - 1) DModeLine();
				KMenuDo(menu, which * 2 + 3);
				if (washelp) DNewDisplay();
				return;
				}
			/*break;*/

		case KEYHELP:
		case '?':
			DMenuHelp(menu, which * 2 + 3);
			needdisp = TRUE;
			break;

		case KEYLEFT:
		case ZCB:
			if (type == 2) {
				KFromStr("\x1b\b\r", 3);
				}
			else	{
				if (--which < 0) which = number - 1;
				}
			break;

		case KEYUP:
		case ZCP:
		case DEL:
		case BS:
			if (--which < 0) which = number - 1;
			break;

		case KEYRIGHT:
		case ZCF:
			if (type == 2) {
				KFromStr("\x1b \r", 3);
				}
			else	{
				if (++which >= number) which = 0;
				}
			break;

		case KEYDOWN:
		case ZCN:
		case SP:
			if (++which >= number) which = 0;
			break;

		case FF:
		case KEYREGEN:
			DNewDisplay();
			DIncrDisplay();
			needdisp = TRUE;
			break;

		default:
			TBell();
			break;
			}
		}
	}


/* ------------------------------------------------------------ */

/* Execute the specified menu entry. */

void
KMenuDo(int menu, int entry)
	{
	uarg = 1;
	resmac_menu = TRUE;
	KMacDo(menu, entry);
	}


/* ------------------------------------------------------------ */

/* Record the keyboard macro position. */

void
KMenuMac(void)
	{
	if (mac_record) mac_menu_ptr = mac_ptr;
	}


/* ------------------------------------------------------------ */

/* Push the supplied key into the input. */

void
KPush(KEYCODE key)
	{
	pushed = key;
	}


/* ------------------------------------------------------------ */

/* Internal routine to display current argument */

FLAG
KUArg(int targ)
	{
	int cnt;
	char buf[LINEBUFFSIZE];

	if (targ == 4) {
		for (cnt = 0; cnt < DELAYCOUNT; cnt++) {
			if (KIsKey() == 'Y') return(FALSE);
			}
		}
	xsprintf(buf, Res_String(NULL, RES_MSGS, RES_PROMPTARG), targ);
	DEchoNM(buf);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Copy at most LEN-1 chars from the end of the current line back to a
whitespace character into BUF. */

void
K_GetLine(char *buf, int len)
	{
	struct mark *mptr;
	char wrkbuf[BUFFSIZE];
	char *wptr;
	int amt;

	*buf = NUL;
	if ((mptr = BMarkCreate()) == NULL) return;
	CLineA();
	for (wptr = wrkbuf; !BIsEnd() && !IsNL() &&
		 wptr < &wrkbuf[sizeof(wrkbuf)]; ) {
		*wptr++ = BGetCharAdv();
		}
	*wptr = NUL;

	while (wptr > wrkbuf && !xiswhite(*wptr)) wptr--;
	if (xiswhite(*wptr)) wptr++;
	amt = min(len - 1, strlen(wptr));
	memmove(buf, wptr, amt);
	buf[amt] = NUL;	

	BPointToMark(mptr);
	BMarkDelete(mptr);
	}


/* ------------------------------------------------------------ */

/* Handle the completion commands.  ISTAB is True for Tab or False for
help. Put any keystrokes to simulate in keybuf. */

void
K_GetStrComp(FLAG istab, char *fname, char *keybuf)
	{
	char sname[FNAMEMAX];
	char filepart[FNAMEMAX];
	char *cptr;
	struct buffer *save_buf = cbuf;
	struct buffer *bptr;
	int chr;
	int cnt;

	xstrcpy(sname, fname);
	FFileGetFile(filepart, sname);
	if (*sfindin(filepart, "*?") == NUL) {
		strcat(sname, "*");
#if defined(MSDOS)
		if (*sindex(filepart, '.') == NUL) strcat(sname, ".*");
#endif
		}
	if (!FDIREDDo(sname, FALSE, TRUE)) return;
	DIncrDisplay();

	if (!istab) return;

	if ((bptr = BBufFind(Res_String(NULL, RES_MSGS, RES_SYSDIRED))) ==
		NULL) return;
	BBufGoto(bptr);
	BMoveToStart();

/* fetch first name */

	for (cptr = sname; !BIsEnd() && cptr < &sname[sizeof(sname) - 2];
		 cptr++) {
		*cptr = BGetCharAdv();
		if (*cptr == NL) break;
		}
	*cptr = NUL;

	if (BGetLength(cbuf) == 0 || *sname == NUL) {
		BBufGoto(save_buf);
		return;
		}

/* now find greatest common substring */

	while (*sname != NUL && !BIsEnd()) {
		for (cptr = sname; *cptr != NUL; cptr++) {
			chr = BGetCharAdv();
			if (*cptr != chr || chr == NL) {
				*cptr = NUL;
				break;
				}
			}
		if (chr != NL) CLineE();
		BMoveBy(1);
		}

	for (cnt = 0; cnt < strlen(filepart); cnt++) {
		keybuf[cnt] = BS;
		}
	
	xstrcpy(keybuf + cnt, sname);
	KFromStr(keybuf, strlen(keybuf));
	BMoveToStart();
	BBufGoto(save_buf);
	}


/* FKEY.C -- Key Input Routines */
