/* FMISC.C -- Miscellaneous Commands

	Written July 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"
#if defined(MSDOS)
#include <time.h>
#endif

void M_Change(char *oldstr, char *newstr, char *savestr);
void M_Replace(FLAG isquery);
FLAG M_Search(char *str, FLAG isforward);

/* ------------------------------------------------------------ */

/* If upper case, convert to lower case. */

int
ConvLower(int chr)
	{
	if (chr < 0 || chr > 255) return(chr);
	return((chr + *(unsigned char *)(c.g.lower + chr)) & 0xFF);
	}


/* ------------------------------------------------------------ */

/* If lower case, convert to upper case. */

int
ConvUpper(int chr)
	{
	if (chr < 0 || chr > 255) return(chr);
	return((chr + *(unsigned char *)(c.g.upper + chr)) & 0xFF);
	}


/* ------------------------------------------------------------ */

/* Move backward to grayspace. */

void
GoToGrayB(void)
	{
	MoveToB(IsGray);
	}


/* ------------------------------------------------------------ */

/* Move forward to grayspace. */

void
GoToGrayF(void)
	{
	MoveToF(IsGray);
	}


/* ------------------------------------------------------------ */

/* Move backward to. */

void
GoToNotGrayB(void)
	{
	MovePastB(IsGray);
	}


/* ------------------------------------------------------------ */

/* Move forward to non-grayspace. */

void
GoToNotGrayF(void)
	{
	MovePastF(IsGray);
	}


/* ------------------------------------------------------------ */

/* Tell if the current character is gray space. */

FLAG
IsGray(void)
	{
	return(c.g.ctype[BGetChar()] & (CSETMASK_NL | CSETMASK_WHI));
	}


/* ------------------------------------------------------------ */

/* Tell if the current character is a newline. */

FLAG
IsNL(void)
	{
	return(c.g.ctype[BGetChar()] & CSETMASK_NL);
	}


/* ------------------------------------------------------------ */

/* Tell if the current character is whitespace. */

FLAG
IsWhite(void)
	{
	return(c.g.ctype[BGetChar()] & CSETMASK_WHI);
	}


/* ------------------------------------------------------------ */

/* Abort the current command prefix. */

void
MAbort(void)
	{
	TBell();
	DModeLine();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Do the About command. */

void
MAbout(void)
	{
	char dir[FNAMEMAX];
	char buf[BUFFSIZE];
	FLAG wastext = xprintf_get_text();

	FGetDir(dir);
	xprintf_set_text(FALSE);
	xsprintf(buf, Res_String(NULL, RES_MSGS, RES_ABOUTSTR),
		c.g.madefor,
		key_method,
		screen_type,
		Res_Char(RES_CONF, RES_SCRNMETHOD),
		TMaxRow(),
		TMaxCol(),
		BPagesUsed(),
		BPagesMax(),
		BPagesSize(),
		dir);
	DShow('T', buf, FALSE);
	TGetKey();
	xprintf_set_text(wastext);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Do the Apropos command. */

void
MApropos(void)
	{
	char what[LINEBUFFSIZE];
	int wlen;
	KEYCODE key;
	int table;
	char *hptr;
	char *cptr;

	uarg = 0;
	if (!FMakeSys(Res_String(NULL, RES_MSGS, RES_SYSHELP), TRUE)) return;

	*what = NUL;
	if (KGetStr(RES_PROMPTAPROPOS, what, sizeof(what)) != 'Y') return;
	wlen = strlen(what);

	DEchoNM(Res_String(NULL, RES_MSGS, RES_WORKING));
	for (table = 0; table < 3; table++) {
		for (key = 0; key < 128; key++) {
			hptr = TabHelp(key, table);
			if (*hptr == '@' && *hptr == '^') continue;

			for (cptr = hptr; *cptr != NUL;
				  cptr = sindex(cptr, SP)) {
				if (*cptr == SP) cptr++;
				if (strnequ(what, cptr, wlen)) break;
				}
			if (*cptr == NUL) continue;

			BInsStr(TabDescr(key, table));
			while (BGetCol() < 16) BInsChar(TAB);
			BInsStr(hptr);
			BInsChar(NL);
			}
		}
	for (key = 0; key < TabNumFunc(); key++) {
		hptr = TabHelp(key, 3);
		if (*hptr == '@' && *hptr == '^') continue;

		for (cptr = hptr; *cptr != NUL; cptr = sindex(cptr, SP)) {
			if (*cptr == SP) cptr++;
			if (strnequ(what, cptr, wlen)) break;
			}
		if (*cptr == NUL) continue;

		BInsStr(TabDescr(key, 3));
		while (BGetCol() < 16) BInsChar(TAB);
		BInsStr(hptr);
		BInsChar(NL);
		}
#if !defined(NOCALC)
	for (key = 0; key < UNumOps(); key++) {
		hptr = UHelp(key);
		for (cptr = hptr; *cptr != NUL;
			  cptr = sindex(cptr, SP)) {
			if (*cptr == SP) cptr++;
			if (strnequ(what, cptr, wlen)) break;
			}
		if (*cptr == NUL) continue;

		BInsStr(UDescr(key));
		while (BGetCol() < 16) BInsChar(TAB);
		BInsStr(hptr);
		BInsChar(NL);
		}
#endif

	BMoveToStart();
	DWindSwap();
	DNewDisplay();
	}


/* ------------------------------------------------------------ */

/* Do the Show Attributes command. */

#if defined(MSDOS)
void
MAttr(void)
	{
	char buf[BUFFSIZE];
	FLAG wastext = xprintf_get_text();
	int cnt;
	int cnt2;

	xprintf_set_text(FALSE);
	for (cnt = 0; cnt < 16; cnt++) {
		for (cnt2 = 0; cnt2 < 16; cnt2++) {
			buf[cnt2 * 2    ] = cnt2 + 'A';
			buf[cnt2 * 2 + 1] = cnt * 16 + cnt2;
			}
		TSetPoint(cnt, 0);
		TAttrBlock(buf, 16, FALSE, 0);
		}
	KGetChar();
	DNewDisplay();
	xprintf_set_text(wastext);
	uarg = 0;
	}
#endif


/* ------------------------------------------------------------ */

/* Do the show char at point command. */

void
MCharAt(void)
	{
	char buf[LINEBUFFSIZE];

	uarg = 0;
	xsprintf(buf, Res_String(NULL, RES_MSGS, RES_CHARAT),
		BGetChar(),
		BGetChar(),
		BGetChar());
	DView(buf);
	}


/* ------------------------------------------------------------ */

/* Handle the Control-X prefix. */

void
MCtrlX(void)
	{
	FLAG disp = FALSE;

	table = TabTable(key, table);
	do	{
		disp = KDelayPrompt(RES_PROMPTCTX) || disp;
		key = KGetChar();
		if (key == KEYQUIT) return;
		} while (key == KEYREGEN);
	if (disp) DModeLine();
	TabDispatch(key, table);
	}


/* ------------------------------------------------------------ */

/* Do the Date command. */

void
MDate(void)
	{
	char buf[SMALLBUFFSIZE];
	struct tm t;

	DNow(&t);
	xsprintf(buf, "%4d-%02d-%02d", t.tm_year, t.tm_mon + 1, t.tm_mday);
	BInsStr(buf);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Build a bindings table. */

void
MDescBind(void)
	{
	KEYCODE key;
	int table;
	char *cptr;

	uarg = 0;
	if (!FMakeSys(Res_String(NULL, RES_MSGS, RES_SYSHELP), TRUE)) return;

	DEchoNM(Res_String(NULL, RES_MSGS, RES_WORKING));
	for (table = 0; table < 3; table++) {
		for (key = 0; key < 128; key++) {
			cptr = TabHelp(key, table);
			if (*cptr != '@' && *cptr != '^') {
				BInsStr(TabDescr(key, table));
				while (BGetCol() < 16) BInsChar(TAB);
				BInsStr(cptr);
				BInsChar(NL);
				}
			}
		}
	for (key = 0; key < TabNumFunc(); key++) {
		cptr = TabHelp(key, 3);
		if (*cptr != '@' && *cptr != '^') {
			BInsStr(TabDescr(key, 3));
			while (BGetCol() < 16) BInsChar(TAB);
			BInsStr(cptr);
			BInsChar(NL);
			}
		}
#if !defined(NOCALC)
	for (key = 0; key < UNumOps(); key++) {
		BInsStr(UDescr(key));
		while (BGetCol() < 16) BInsChar(TAB);
		BInsStr(UHelp(key));
		BInsChar(NL);
		}
#endif
	BMoveToStart();
	DWindSwap();
	DNewDisplay();
	}


/* ------------------------------------------------------------ */

/* Do the describe key command. */

void
MDescKey(void)
	{
	char buf[LINEBUFFSIZE];
	char *cptr;
	KEYCODE key;
	int table;

	do	{
		DEcho(Res_String(NULL, RES_MSGS, RES_PROMPTKEY));
		} while ((key = KGetChar()) == KEYREGEN);

	table = TabTable(key, 0);
	if (table == 1 || table == 2) {
		xsprintf(buf, "%s ", TabDescr(key, 0));
		do	{
			DEcho(buf);
			} while ((key = KGetChar()) == KEYREGEN);
		}

	cptr = TabHelp(key, table);
	if (*cptr == '@')
		cptr = (char *)Res_String(NULL, RES_MSGS, RES_NOTCMD);
	else if (*cptr == '^')
		cptr = (char *)Res_String(NULL, RES_MSGS, RES_SELFINSERT);

	xsprintf(buf, "%s   %s", TabDescr(key, table), cptr);
	DView(buf);
	DNewDisplay();
	}


/* ------------------------------------------------------------ */

/* Do the function key specified by the arg. */

void
MDoKey(void)
	{
	if (!isuarg) {
		DError(RES_EXPLARG);
		}
	else	{
		KPush(0x100 | uarg);
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Exit the editor. */

void
MExit(void)
	{
	struct buffer *bptr;
	int k;

	uarg = 0;
	for (bptr = buffers; bptr < &buffers[NUMBUFFERS]; bptr++) {
		if (!BIsFree(bptr) && !IS_SYS(bptr->fname) && BIsMod(bptr)) {
			if (isuarg) {
				if (!BFileWrite()) return;
				}
			else	{
				BBufGoto(bptr);
				DIncrDisplay();
				k = KAsk(RES_ASKSAVEEXIT);
				if (k == KEYABORT) {
#if defined(SYSMGR)
					JNoFini();
#endif
					return;
					}
				else if (k == KEYQUIT) BFileWrite();
				else if (k == 'Y' && !BFileWrite()) return;
				}
			}
		}
	doabort = TRUE;
	}


/* ------------------------------------------------------------ */

/* Cycle/set the font. */

void
MFont(void)
	{
	char *cptr = Res_String(NULL, RES_CONF, RES_TYPELIST);
	char *cptr2;

	if (!isuarg) {
		for (cptr2 = cptr; *cptr2 != 101; cptr2++) {
			if (*cptr2 == screen_type) break;
			}
		if (*cptr2 == screen_type) cptr2++;
		if (*cptr2 == 101) cptr2 = cptr;
		uarg = *cptr2;
		}
	else if (uarg == 103) {
		uarg = Res_Number(RES_CONF, RES_SCRNTYPE);
		}
	else if (KAsk(RES_ASKSURE) != 'Y') {
		uarg = 0;
		return;
		}
	DFont(uarg, FALSE);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Insert the arg. */

void
MIns8(void)
	{
	if (!isuarg) {
		DError(RES_EXPLARG);
		}
	else	{
		BInsChar(uarg);
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Self-insert. */

void
MInsChar(void)
	{
	struct mark *mptr;

	if (cbuf->c.fill == 'F' && BGetCol() >= cbuf->c.right_margin) {
		mptr = BMarkCreate();
		do	{
			GoToGrayB();
			MovePastB(IsWhite);
			} while (BGetCol() > cbuf->c.right_margin);
		if (BGetCol() == 0) {
			MovePastF(IsWhite);
			GoToGrayF();
			}
		if (BIsBeforeMark(mptr)) {
			MovePastF(IsWhite);
			BMoveBy(-1);
			BCharChange(NL);
			BInsSpaces(cbuf->c.left_margin);
			if (BIsBeforeMark(mptr)) BPointToMark(mptr);
			}
		else	BPointToMark(mptr);
		BMarkDelete(mptr);
		}
	BInsChar(key);
	}


/* ------------------------------------------------------------ */

/* Insert a page break. */

void
MInsPgBk(void)
	{
	if (!IsNL()) BInsChar(NL);
	BInsStr("\f\n");
	}


/* ------------------------------------------------------------ */

/* Evaluate a keyboard macro.  */

void
MMacEval(void)
	{
	if (xisdigit(key)) {
		KMacDo(RES_KEYM, key - '0');
		}
	else if (key >= (0x100 + 120) && key <= (0x100 + 128)) {
		KMacDo(RES_KEYM, key - (0x100 + 120) + 1);
		}
	else	{	/* default to M-0 */
		KMacDo(RES_KEYM, 0);
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Make the previous command a deletion command. */

void
MMakeDelete(void)
	{
	}


/* ------------------------------------------------------------ */

/* Execute the menu by number. */

void
MMenu(void)
	{
	KMenu(isuarg ? uarg : 0);
	}


/* ------------------------------------------------------------ */

/* Execute the help menu. */

void
MMenuH(void)
	{
	KMenu(1);
	}


/* ------------------------------------------------------------ */

/* Execute the main menus. */

void
MMenuM(void)
	{
	KMenu(0);
	}


/* ------------------------------------------------------------ */

/* Handle the Meta prefix. */

void
MMeta(void)
	{
	FLAG disp = FALSE;

	table = TabTable(key, table);
	do	{
		disp = KDelayPrompt(RES_PROMPTESC) || disp;
		key = KGetChar();
		if (key == KEYQUIT) return;
		} while (key == KEYREGEN);
	if (disp) DModeLine();
	TabDispatch(key, table);
	}


/* ------------------------------------------------------------ */

/* Not implemented. */

void
MNotImpl(void)
	{
	DError(RES_UNKCMD);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Quote the next character. */

void
MQuote(void)
	{
	FLAG disp = FALSE;
	KEYCODE c;

	do	{
		disp = KDelayPrompt(RES_PROMPTQUOTE) || disp;
		c = KGetChar();
		if (c == KEYQUIT) return;
		} while (c == KEYREGEN);
	while (uarg-- > 0) {
		BInsChar(c);
		}
	if (disp) DModeLine();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Replace string. */

void
MReplace(void)
	{
	M_Replace(FALSE);
	}


/* ------------------------------------------------------------ */

/* Query replace string. */

void
MReplaceQ(void)
	{
	M_Replace(TRUE);
	}


/* ------------------------------------------------------------ */

/* Backward string search. */

void
MSearchB(void)
	{
	if (KGetStr(RES_PROMPTREVSRCH, stringarg, sizeof(stringarg)) != 'Y') {
		uarg = 0;
		return;
		}
	BMarkToPoint(cwin->point);
	while (uarg-- > 0) {
		if (!M_Search(stringarg, BACKWARD)) {
			BPointToMark(cwin->point);
			DError(RES_STRNOTFOUND);
			break;
			}
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Forward string search. */

void
MSearchF(void)
	{
	if (KGetStr(RES_PROMPTFWDSRCH, stringarg, sizeof(stringarg)) != 'Y') {
		uarg = 0;
		return;
		}
	BMarkToPoint(cwin->point);
	while (uarg-- > 0) {
		if (!M_Search(stringarg, FORWARD)) {
			BPointToMark(cwin->point);
			DError(RES_STRNOTFOUND);
			break;
			}
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Do the time command. */

void
MTime(void)
	{
	char buf[SMALLBUFFSIZE];
	struct tm t;

	DNow(&t);
	xsprintf(buf, "%2d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
	BInsStr(buf);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Toggle the PgUp/Dn and Up/Down Arrow commands. */

void
MTogPgLn(void)
	{
	unsigned char *fkeys;
	unsigned char c;

	fkeys = Res_String(NULL, RES_FUNC, 0);

	c = *(fkeys + (KEYPGUP & 0xFF));
	*(fkeys + (KEYPGUP & 0xFF)) = *(fkeys + (KEYUP & 0xFF));
	*(fkeys + (KEYUP & 0xFF)) = c;

	c = *(fkeys + (KEYPGDN & 0xFF));
	*(fkeys + (KEYPGDN & 0xFF)) = *(fkeys + (KEYDOWN & 0xFF));
	*(fkeys + (KEYDOWN & 0xFF)) = c;

	uarg = 0;
	if (togpgln == 2) {
		togpgln = 1;
		}
	else	{
		togpgln = !togpgln;
		DModeLine();
		}
	}


/* ------------------------------------------------------------ */

/* Handle argument prefix. */

void
MUArg(void)
	{
	FLAG numflag = FALSE;
	FLAG wasechoed = FALSE;

	uarg *= 4;
	do	{
		wasechoed = KUArg(uarg) || wasechoed;
		while (xisdigit(key = KGetChar()))  {
			if (!numflag) {
				uarg = 0;
				numflag = TRUE;
				}
			uarg = uarg * 10 + key - '0';
			wasechoed = KUArg(uarg) || wasechoed;
			}
		} while (key == KEYREGEN);
	if (wasechoed) DModeLine();
	isuarg = TRUE;
	TabDispatch(key, table);
	}


/* ------------------------------------------------------------ */

/* Move back past a type of text. */

void
MovePastB(FLAG (*pred)())
	{
	BMoveBy(-1);
	while (!BIsStart() && (*pred)()) BMoveBy(-1);
	if (!BIsStart()) BMoveBy(1);
	}


/* ------------------------------------------------------------ */

/* Move forward past a type of text. */

void
MovePastF(FLAG (*pred)())
	{
	while (!BIsEnd() && (*pred)()) BMoveBy(1);
	}


/* ------------------------------------------------------------ */

/* Move back to a type of text. */

void
MoveToB(FLAG (*pred)())
	{
	BMoveBy(-1);
	while (!BIsStart() && !(*pred)()) BMoveBy(-1);
	if (!BIsStart()) BMoveBy(1);
	}


/* ------------------------------------------------------------ */

/* Move forward to a type of text. */

void
MoveToF(FLAG (*pred)())
	{
	while (!BIsEnd() && !(*pred)()) BMoveBy(1);
	}


/* ------------------------------------------------------------ */

/* Reverse search for the next newline. */

FLAG
SearchNLB(void)
	{
	return(BSearchB(NL, SNL));
	}


/* ------------------------------------------------------------ */

/* Search for the next newline. */

FLAG
SearchNLF(void)
	{
	return(BSearchF(NL, SNL));
	}


/* ------------------------------------------------------------ */

/* Change old string into new */

void
M_Change(char *oldstr, char *newstr, char *savestr)
	{
	int newlen = strlen(newstr);
	int oldlen = strlen(oldstr);
	int cnt;

	BMoveBy(-oldlen);
	if (savestr != NULL) {
		for (cnt = 0; cnt < oldlen; ++cnt) {
			*savestr++ = BGetCharAdv();
			}
		*savestr = NUL;
		BMoveBy(-oldlen);
		}
	if (oldlen > newlen) BCharDelete(oldlen - newlen);

	cnt = min(oldlen, newlen);
	while (cnt-- > 0) BCharChange(*newstr++);

	while (*newstr != NUL) BInsChar(*newstr++);
	}


/* ------------------------------------------------------------ */

/* Do query replace and replace string */

void
M_Replace(FLAG isquery)
	{
	KEYCODE key;
	char from[STRMAX];
	char to[STRMAX];
	char save[STRMAX];
	char buf[LINEBUFFSIZE];
	struct mark *mptr;

	uarg = 0;
	*from = NUL;
	*to = NUL;

	do	{
		if (KGetStr(isquery ? RES_PROMPTQUERY :
			RES_PROMPTREPL, from, sizeof(from)) != 'Y') return;
		} while (*from == NUL);
	if (KGetStr(RES_PROMPTWITH, to, sizeof(to)) != 'Y') return;

	mptr = BMarkCreate();
	key = isquery ? ',' : SP;

	xsprintf(buf, Res_String(NULL, RES_MSGS, RES_DISPLREPL), from, to);

	while (M_Search(from, FORWARD)) {
		if (!isquery) {
			M_Change(from, to, NULL);
			continue;
			}
		DIncrDisplay();
		DEchoNM(buf);
		key = ConvUpper(KGetChar());
		if (key == KEYQUIT || key == KEYABORT || key == BEL ||
			 key == ESC) {
			BMarkToPoint(mptr);
			break;
			}
		else if (key == KEYREGEN) {
			}
		else if (key == KEYHELP || key == '?') {
			M_Search(from, BACKWARD);
			DIncrDisplay();
			DView(Res_String(NULL, RES_MSGS, RES_DISPLCMDS));
			key = ',';
			}
		else if (key == '!') {
			isquery = FALSE;
			M_Change(from, to, save);
			DIncrDisplay();
			if (key == ',' && KAsk(RES_ASKCONFIRM) != 'Y')
				M_Change(to, save, NULL);
			if (key == ';') BMoveToEnd();
			}
		else if (key == ',' || key == ';' ||
			 *sindex(Res_String(NULL, RES_MSGS, RES_YESSTRING),
				key) != NUL) {
			M_Change(from, to, save);
			DIncrDisplay();
			if (key == ',' && KAsk(RES_ASKCONFIRM) != 'Y')
				M_Change(to, save, NULL);
			if (key == ';') BMoveToEnd();
			}
		else if (key == '.') {
			BMoveToEnd();
			}
		else if (*sindex(Res_String(NULL, RES_MSGS, RES_NOSTRING),
				key) != NUL) {
			}
		else	{
			TBell();
			}
		}
	DModeLine();
	BPointToMark(mptr);
	BMarkDelete(mptr);
	}


/* ------------------------------------------------------------ */

/* Search for STR in the specified direction. */

FLAG
M_Search(char *str, FLAG isforward)
	{
	int cnt;
	char c1;
	char c2;

	for (cnt = 0; str[cnt] != NUL; ) {
		c1 = *str;
		c2 = ConvUpper(c1);
		if (isforward) {
			if (!BSearchF(c1, c2)) break;
			}
		else	{
			if (!BSearchB(c1, c2)) break;
			BMoveBy(1);
			}

		for (cnt = 1; !BIsEnd() && str[cnt] != NUL; ++cnt) {
			c1 = str[cnt];
			c2 = ConvUpper(c1);
			if (BGetChar() != c1 && BGetChar() != c2) break;
			BMoveBy(1);
			}
		if (!isforward || str[cnt] != NUL)
			BMoveBy((isforward ? 1 : 0) - cnt);
		}
	return(str[cnt] == NUL);
	}


/* end of FMISC.C -- Miscellaneous Commands */
