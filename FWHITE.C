/* FWHITE.C -- Whitespace-Oriented Commands

	Written July 1991 by Craig A. Finseth
	Copyright 1991,2,3 by Craig A. Finseth
*/

#include "freyja.h"


/* ------------------------------------------------------------ */

/* Delete the surounding whitespace. */

void
WDelFWhite(void)
	{
	BMarkToPoint(cwin->point);
	MovePastF(IsWhite);
	BRegDelete(cwin->point);
	}


/* ------------------------------------------------------------ */

/* Delete the spaces and tabs around the point. */

void
WDelWhite(void)
	{
	MovePastB(IsWhite);
	BMarkToPoint(cwin->point);
	MovePastF(IsWhite);
	BRegDelete(cwin->point);
	}


/* ------------------------------------------------------------ */

/* Perform consistency and reasonableness checking on the conf_buffer
data. */

void
WFixup(struct conf_buffer *cptr)
	{
	if (cptr->left_margin < 0) cptr->left_margin = 0;
	if (cptr->left_margin > 100) cptr->left_margin = 100;

	if (cptr->right_margin <= 0) cptr->right_margin = 9999;
	if (cptr->right_margin <= cptr->left_margin + 2)
		cptr->right_margin = cptr->left_margin + 3;
	if (cptr->right_margin > 9999) cptr->right_margin = 9999;

	if (cptr->tab_spacing < 1) cptr->tab_spacing = 1;
	if (cptr->tab_spacing > 50) cptr->tab_spacing = 50;

	if (cptr->fill != 'N' && cptr->fill != 'F' && cptr->fill != 'W')
		cptr->fill = 'N';
	}


/* ------------------------------------------------------------ */

/* Delete the indentation from the current line. */

void
WIndDel(void)
	{
	CLineA();
	BMarkToPoint(cwin->point);
	WDelWhite();
	BPointToMark(cwin->point);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Indent the next line the same as the current line. */

void
WIndNext(void)
	{
	int cnt;

	BInsChar(NL);
	BMoveBy(-1);
	CLineA();
	MovePastF(IsWhite);
	cnt = BGetCol();
	SearchNLF();
	BInsTabSpaces(cnt);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Open a new line indented the same as the current line. */

void
WIndThis(void)
	{
	int cnt;

	CLineA();
	MovePastF(IsWhite);
	cnt = BGetCol();
	CLineA();
	BInsTabSpaces(cnt);
	BInsChar(NL);
	BMoveBy(-1);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Insert a newline. */

void
WInsNL(void)
	{
	BInsChar(NL);
	}


/* ------------------------------------------------------------ */

/* Insert a newline after the current position. */

void
WInsNLA(void)
	{
	BInsChar(NL);
	BMoveBy(-1);
	}


/* ------------------------------------------------------------ */

/* Delete the surrounding grayspace and leave exactly one blank. */

void
WJoinGray(void)
	{
	GoToNotGrayB();
	BMarkToPoint(cwin->point);
	GoToNotGrayF();
	BRegDelete(cwin->point);
	BInsChar(SP);
	}


/* ------------------------------------------------------------ */

/* Center the current line. */

void
WLineCenter(void)
	{
	int cnt;
	int tmp;

	cnt = cbuf->c.right_margin;
	if ((cnt -= cbuf->c.left_margin) >= 1) {
		CLineA();
		WDelWhite();
		CLineE();
		tmp = BGetCol();
		if (tmp && tmp <= cnt) {
			CLineA();
			BInsTabSpaces(cbuf->c.left_margin + (cnt - tmp) / 2);
			}
		}
	CLineE();
	if (uarg > 1) BMoveBy(1);
	}


/* ------------------------------------------------------------ */

/* Print the line count of the point and the buffer. */

void
WPrintLine(void)
	{
	char buf[LINEBUFFSIZE];
	int pointline = 1;
	int bufline = 1;
	int markline = 1;

	uarg = 0;
	BMarkToPoint(cwin->point);
	BMoveToStart();
	while (!BIsEnd()) {
		if (SearchNLF()) ++bufline;
		if (!BIsAfterMark(cwin->point)) pointline = bufline;
		if (!BIsAfterMark(mark)) markline = bufline;
		}
	BPointToMark(cwin->point);

	xsprintf(buf, Res_String(NULL, RES_MSGS, RES_DISPLLINE),
		pointline, bufline, markline);
	DView(buf);
	}


/* ------------------------------------------------------------ */

/* Print the margin settings */

void
WPrintMar(void)
	{
	char buf[LINEBUFFSIZE];

	uarg = 0;
	if (isuarg) {
		xsprintf(buf, "\013R %d,T %d,L %d",
			cbuf->c.right_margin,
			cbuf->c.tab_spacing,
			cbuf->c.left_margin);
		BMarkToPoint(cwin->point);
		BMoveToStart();
		BInsStr(buf);
		BInsChar(NL);
		BPointToMark(cwin->point);
		DView(Res_String(NULL, RES_MSGS, RES_DISPLRULER));
		}
	else	{
		xsprintf(buf, Res_String(NULL, RES_MSGS, RES_DISPLCOLS),
			cbuf->c.left_margin,
			cbuf->c.right_margin,
			cbuf->c.tab_spacing);
		DView(buf);
		}
	}


/* ------------------------------------------------------------ */

/* Print the current position. */

void
WPrintPos(void)
	{
	char buf[LINEBUFFSIZE];
	long tmp;

	BMarkSwap(mark);
	tmp = BGetLocation();
	BMarkSwap(mark);
	xsprintf(buf, Res_String(NULL, RES_MSGS, RES_DISPLPOINT),
		BGetLocation(),
		BGetLength(cbuf),
		BGetCol(),
		tmp);
	DView(buf);
	}


/* ------------------------------------------------------------ */

/* Ask for the fill mode and set this buffer's fill mode. */

void
WSetFill(void)
	{
	if (!isuarg) KMenuDo(RES_FILL2, 0);
	else if (uarg == 0) cbuf->c.fill = 'N';
	else if (uarg == 1) cbuf->c.fill = 'F';
	else if (uarg == 2) cbuf->c.fill = 'W';
	else DError(RES_ERRFILL);
	DModeLine();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Set this buffer's left margin to the argument. */

void
WSetLeft(void)
	{
	if (!isuarg) {
		DError(RES_EXPLARG);
		}
	else	{
		cbuf->c.left_margin = uarg;
		WFixup(&cbuf->c);
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Set this buffer's right margin to the argument. */

void
WSetRight(void)
	{
	if (!isuarg) {
		DError(RES_EXPLARG);
		}
	else	{
		cbuf->c.right_margin = uarg;
		WFixup(&cbuf->c);
		}
	DModeLine();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Set this buffer's tab spacing to the argument. */

void
WSetTabs(void)
	{
	if (!isuarg) {
		DError(RES_EXPLARG);
		}
	else	{
		cbuf->c.tab_spacing = uarg;
		WFixup(&cbuf->c);
		DNewDisplay();
		}
	uarg = 0;
	}


/* end of FWHITE.C -- Whitespace-Oriented Commands */
