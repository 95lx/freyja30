/* FREGION.C -- Region-Oriented Commands

	Written July 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"
#if defined(MSDOS)
#include <time.h>
#endif

static int outfd;
static char *outbuf;
static char *outptr;
static int outleft;
static int outline;
static int outcol;
static int outpage;
static struct tm outt;

void R_CopyToMark(struct mark *mptr, FLAG isforward);
void R_Dent(void);
void R_Do(void (*proc)());
void R_Fill(void);
void R_HToS(void);
void R_Lower(void);
void R_Meta(void);
void R_SToH(void);
void R_Tabify(void);
void R_Trim(void);
void R_Untabify(void);
void R_Upper(void);
void R_Write(void);
void R_WriteChr(int chr);
void R_WriteDone(void);
void R_WriteFF(void);
void R_WriteNL(int num);
void R_WriteSP(int num);
void R_WriteStr(char *str);

/* ------------------------------------------------------------ */

/* Remove the trailing whitespace from a region. */

void
RDelWhite(void)
	{
	R_Do(R_Trim);
	}


/* ------------------------------------------------------------ */

/* Convert hard newlines to soft. */

void
RHardToSoft(void)
	{
	R_Do(R_HToS);
	}


/* ------------------------------------------------------------ */

/* Delete the specified region in the specified direction. */

void
RKillToMark(struct mark *mptr, FLAG isforward)
	{
	R_CopyToMark(mptr, isforward);
	BRegDelete(mptr);
	}


/* ------------------------------------------------------------ */

/* Indent the region. */

void
RIndent(void)
	{
	if (!isuarg) uarg = cbuf->c.tab_spacing;
	R_Do(R_Dent);
	}


/* ------------------------------------------------------------ */

/* Lowercase the region. */

void
RLower(void)
	{
	R_Do(R_Lower);
	}


/* ------------------------------------------------------------ */

/* Set the mark to the point. */

void
RMarkSet(void)
	{
	BMarkToPoint(mark);
	}


/* ------------------------------------------------------------ */

/* Swap the point and the mark. */

void
RMarkSwap(void)
	{
	BMarkSwap(mark);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Clear/set the meta bit in region. */

void
RMeta(void)
	{
	R_Do(R_Meta);
	}


/* ------------------------------------------------------------ */

/* Outdent the region. */

void
ROutdent(void)
	{
	if (!isuarg) uarg = cbuf->c.tab_spacing;
	uarg = -uarg;
	R_Do(R_Dent);
	}


/* ------------------------------------------------------------ */

/* Print the region. */

void
RPrint(void)
	{
	char buf[BUFFSIZE];
	int cnt;

	uarg = 0;
	DEcho(Res_String(NULL, RES_MSGS, RES_WRITING));

#if defined(MSDOS)
	if ((outfd = creat(Res_String(NULL, printer, RES_PRDEV), 0666)) <
		 0) {
#endif
#if defined(UNIX)
	if ((outfd = open(Res_String(NULL, printer, RES_PRDEV),
		 O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0) {
#endif
		DError(RES_ERROPEN);
		return;
		}
	outbuf = buf;
	outptr = outbuf;
	outleft = Res_Number(printer, RES_PRLEFT);
	outpage = 1;
	DNow(&outt);

		/* handle IPO */
	if (!isuarg) R_WriteNL(Res_Number(printer, RES_PRIPO));
	outline = 0;
	outcol = 0;
	R_Do(R_Write);
	R_WriteDone();
	}


/* ------------------------------------------------------------ */

/* Copy the region to the kill buffer. */

void
RRegCopy(void)
	{
	R_CopyToMark(mark, BIsBeforeMark(mark));
	}


/* ------------------------------------------------------------ */

/* Delete the region. */

void
RRegDelete(void)
	{
	if (!isuarg) R_CopyToMark(mark, BIsBeforeMark(mark));
	BRegDelete(mark);
	}


/* ------------------------------------------------------------ */

/* Fill the region. */

void
RRegFill(void)
	{
	DEchoNM(Res_String(NULL, RES_MSGS, RES_WORKING));
	if (isuarg) {
		BMoveToEnd();
		BMarkToPoint(mark);
		BMoveToStart();
		}
	uarg = 0;
	R_Do(R_Fill);
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Save the region. */

void
RSave(void)
	{
	char buf[BUFFSIZE];

	uarg = 0;
	xstrcpy(filearg, "");
	do	{
		if (KGetFile(RES_PROMPTSREG, filearg, FALSE) != 'Y') return;
		} while (*filearg == NUL);
	DEcho(Res_String(NULL, RES_MSGS, RES_WRITING));

	FFileFixName(filearg);
#if defined(MSDOS)
	if ((outfd = creat(filearg, 0666)) < 0) {
#endif
#if defined(UNIX)
	if ((outfd = open(cbuf->fname, O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0) {
#endif
		DError(RES_ERROPEN);
		return;
		}
	outbuf = buf;
	outptr = outbuf;
	outline = -1;
	R_Do(R_Write);
	R_WriteDone();
	}


/* ------------------------------------------------------------ */

/* Convert soft newlines to hard. */

void
RSoftToHard(void)
	{
	R_Do(R_SToH);
	}


/* ------------------------------------------------------------ */

/* Tabify the region. */

void
RTabify(void)
	{
	R_Do(R_Tabify);
	}


/* ------------------------------------------------------------ */

/* Untabify the region. */

void
RUntabify(void)
	{
	R_Do(R_Untabify);
	}


/* ------------------------------------------------------------ */

/* Uppercase the region. */

void
RUpper(void)
	{
	R_Do(R_Upper);
	}


/* ------------------------------------------------------------ */

/* Yank the kill buffer. */

void
RYank(void)
	{
	struct buffer *savebuf = cbuf;

	BMarkToPoint(mark);
	BBufGoto(kill_buf);
	BMoveToEnd();
	BMarkToPoint(cwin->point);
	BMoveToStart();
	BRegCopy(cwin->point, savebuf);
	BBufGoto(savebuf);
	}


/* ------------------------------------------------------------ */

/* Copy point to mark to delete buffer */

void
R_CopyToMark(struct mark *mptr, FLAG isforward)
	{
	struct mark *mptr2;
	struct buffer *savebuf = cbuf;

	BBufGoto(kill_buf);
	if (!TabIsDelete(lastkey, lasttable) && !(isrepeating &&
		 TabIsDelete(key, table))) {
		BMoveToEnd();
		mptr2 = BMarkCreate();
		BMoveToStart();
		BRegDelete(mptr2);
		BMarkDelete(mptr2);
		}
	if (isforward)
		BMoveToEnd();
	else	BMoveToStart();
	BBufGoto(savebuf);
	BRegCopy(mptr, kill_buf);
	}


/* ------------------------------------------------------------ */

/* Indent iterator for R_Do. */

void
R_Dent(void)
	{
	int cnt;

	CLineA();
	if (BIsEnd() || !BIsBeforeMark(mark)) return;

	GoToNotGrayF();
	cnt = BGetCol();
	WDelWhite();
	BInsTabSpaces(cnt + uarg);
	SearchNLF();
	}


/* ------------------------------------------------------------ */

/* Do PROC over the region. */

void
R_Do(void (*proc)())
	{
	FLAG isafter;
	struct mark *mptr;

	if ((mptr = BMarkCreate()) == NULL) return;
	if (isafter = BIsAfterMark(mark)) BMarkSwap(mark);
	BMarkToPoint(mptr);
	while (!BIsEnd() && BIsBeforeMark(mark)) {
		(*proc)();
		}
	BPointToMark(mptr);
	if (isafter) BMarkSwap(mark);
	BMarkDelete(mptr);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Fill iterator for R_Do. */

void
R_Fill(void)
	{
	WParaFill();
	WParaF();
	GoToNotGrayF();
	}


/* ------------------------------------------------------------ */

/* Hard to Soft iterator for R_Do. */

void
R_HToS(void)
	{
	if (!BSearchF(NL, NL) || !BIsBeforeMark(mark)) {
		BMoveToEnd();
		return;
		}
	if (BGetChar() == NL)
		BCharDelete(1);
	else	{
		BMoveBy(-1);
		BCharChange(SNL);
		}
	}


/* ------------------------------------------------------------ */

/* Lowercase iterator for R_Do. */

void
R_Lower(void)
	{
	BCharChange(ConvLower(BGetChar()));
	}


/* ------------------------------------------------------------ */

/* Clear/set the meta bit iterator for R_Do. */

void
R_Meta(void)
	{
	int chr = BGetChar();

	if (!isuarg)
		chr &= 0x7F;
	else	chr |= 0x80;
	BCharChange(chr);
	}


/* ------------------------------------------------------------ */

/* Soft to Hard iterator for R_Do. */

void
R_SToH(void)
	{
	if (!BSearchF(NL, SNL) || !BIsBeforeMark(mark)) {
		BMoveToEnd();
		return;
		}
	BMoveBy(-1);
	if (BGetChar() == NL) {
		BMoveBy(1);
		BInsChar(NL);
		}
	else	BCharChange(NL);
	}


/* ------------------------------------------------------------ */

/* Tabify iterator for R_Do. */

void
R_Tabify(void)
	{
	struct mark *mptr;
	int cnt;

	if (!BSearchF(SP, SP) || !BIsBeforeMark(mark)) {
		BMoveToEnd();
		return;
		}
	if (!IsWhite()) return;

	BMoveBy(-1);
	mptr = BMarkCreate();
	cnt = BGetCol();
	MovePastF(IsWhite);
	cnt = BGetCol() - cnt;
	BRegDelete(mptr);
	BMarkDelete(mptr);
	while (TGetTabWidth(BGetCol()) <= cnt) {
		cnt -= TGetTabWidth(BGetCol());
		BInsChar(TAB);
		}
	BInsSpaces(cnt);
	}


/* ------------------------------------------------------------ */

/* Trim whitespace from region iterator for R_Do. */

void
R_Trim(void)
	{
	CLineE();
	WDelWhite();
	BMoveBy(1);
	}


/* ------------------------------------------------------------ */

/* Untabify iterator for R_Do. */

void
R_Untabify(void)
	{
	if (!BSearchF(TAB, TAB) || !BIsBeforeMark(mark)) {
		BMoveToEnd();
		return;
		}

	BMoveBy(-1);
	BInsSpaces(TGetTabWidth(BGetCol()));
	BCharDelete(1);		
	}


/* ------------------------------------------------------------ */

/* Uppercase iterator for R_Do. */

void
R_Upper(void)
	{
	BCharChange(ConvUpper(BGetChar()));
	}



/* ------------------------------------------------------------ */

/* Write iterator for R_Do. */

void
R_Write(void)
	{
	int chr = BGetCharAdv();
	int num;
	char buf[BUFFSIZE];

	if (isuarg) {
		R_WriteChr(chr);
		return;
		}
	if (outline == 0) {
		num = Res_Number(printer, RES_PRTOP);
		if (Res_Char(printer, RES_PRHEAD) == 'D') {
			if (num < 2) num = 2;
			R_WriteNL(num - 2);
			xsprintf(buf, Res_String(NULL, RES_MSGS, RES_PRINTDOC),
				cbuf->fname,
				outt.tm_year, outt.tm_mon + 1, outt.tm_mday,
				outt.tm_hour, outt.tm_min, outt.tm_sec,
				outpage);
			R_WriteStr(buf);
			R_WriteNL(2);
			}
		else	{
			R_WriteNL(num);
			}
		}
	if (chr == SNL) chr = NL;
	if (chr == NL) {
		R_WriteNL(1);
		if (outline >= Res_Number(printer, RES_PAGELEN) -
			 Res_Number(printer, RES_PRBOT)) {
			R_WriteFF();
			}
		}
	else if (chr == FF) {
		R_WriteFF();
		}
	else	{
		if (outline >= 0 && outcol == 0) {
			R_WriteSP(outleft);
			}
		if (chr == TAB) {
			R_WriteSP(TGetTabWidth(outcol - outleft));
			}
		else	{
			R_WriteChr(chr);
			outcol++;
			}
		}
	}


/* ------------------------------------------------------------ */

/* Write a character. */

void
R_WriteChr(int chr)
	{
	*outptr++ = chr;
	if (outptr > outbuf + BUFFSIZE) {
		write(outfd, outbuf, outptr - outbuf);
		outptr = outbuf;
		}
	}


/* ------------------------------------------------------------ */

/* Done writing region. */

void
R_WriteDone(void)
	{
	int num;
	char buf[BUFFSIZE];

	if (Res_Char(printer, RES_PRHEAD) == 'L') {
		num = Res_Number(printer, RES_PAGELEN) -
			Res_Number(printer, RES_PRBOT);
		R_WriteNL(num - outline + 1);
		if (outpage > 1) {
			xsprintf(buf, Res_String(NULL, RES_MSGS, RES_PRINTLET),
				outpage);
			R_WriteSP(((Res_Number(printer, RES_PAGEWID) -
				outleft -
				strlen(buf)) / 2) + outleft);
			R_WriteStr(buf);
			}
		R_WriteNL(1);
		}
	R_WriteNL(Res_Number(printer, RES_PAGELEN) - outline);
		/* handle IPO */
	num = Res_Number(printer, RES_PRIPO);
	if (!isuarg && num > 0) {
		num = Res_Number(printer, RES_PAGELEN) - num;
		R_WriteNL(num);
		}
	else	{
		R_WriteNL(1);
		}
	if (outptr > outbuf) {
		write(outfd, outbuf, outptr - outbuf);
		}
	close(outfd);
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Handle a form feed / page end. */

void
R_WriteFF(void)
	{
	char buf[BUFFSIZE];
	int plen = Res_Number(printer, RES_PAGELEN);

	if (outpage > 1 && Res_Char(printer, RES_PRHEAD) == 'L') {
		R_WriteNL(plen - outline - 4);
		xsprintf(buf, Res_String(NULL, RES_MSGS, RES_PRINTLET), outpage);
		R_WriteSP(((Res_Number(printer, RES_PAGEWID) - outleft -
			strlen(buf)) / 2) + outleft);
		R_WriteStr(buf);
		R_WriteNL(1);
		}
	R_WriteNL(plen - outline);
	outline = 0;
	outpage++;
	}


/* ------------------------------------------------------------ */

/* Output NUM newlines. */

void
R_WriteNL(int num)
	{
	outcol = 0;
	while (num-- > 0) {
		outline++;
		R_WriteChr(CR);
		R_WriteChr(LF);
		}
	}


/* ------------------------------------------------------------ */

/* Output NUM spaces. */

void
R_WriteSP(int num)
	{
	outcol += num;
	while (num-- > 0) {
		R_WriteChr(SP);
		}
	}


/* ------------------------------------------------------------ */

/* Output a string. */

void
R_WriteStr(char *str)
	{
	while (*str != NUL) {
		R_WriteChr(*str++);
		outcol++;
		}
	}


/* end of FREGION.C -- Region-Oriented Commands */
