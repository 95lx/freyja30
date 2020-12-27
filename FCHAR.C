/* FCHAR.C -- Character-, Line-, and Screen -Oriented Commands

	Written July 1991 by Craig A. Finseth
	Copyright 1991,2,3 by Craig A. Finseth
*/

#include "freyja.h"

static int last_col = 0;	/* last column for ^N and ^P */

/* ------------------------------------------------------------ */

/* Move backward one character. */

void
CCharB(void)
	{
	BMoveBy(-uarg);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Delete backward one character. */

void
CCharBD(void)
	{
	BMarkToPoint(cwin->point);
	BMoveBy(-uarg);
	BRegDelete(cwin->point);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Move forward one character. */

void
CCharF(void)
	{
	BMoveBy(uarg);
	uarg = 0;
	}

/* ------------------------------------------------------------ */

/* Delete forward one character. */

void
CCharFD(void)
	{
	BCharDelete(uarg);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Transpose the previous two characters. */

void
CCharTran(void)
	{
	int chr;

	if (Res_Char(RES_CONF, RES_TWIDDRAG) == 'D') {
		chr = BGetChar();
		BCharDelete(1);
		BMoveBy(-1);
		BInsChar(chr);
		BMoveBy(1);
		}
	else	{
		BMoveBy(-2);
		chr = BGetChar();
		BCharDelete(1);
		BMoveBy(1);
		BInsChar(chr);
		uarg = 0;
		}
	}


/* ------------------------------------------------------------ */

/* Move to the beginning of the current line. */

void
CLineA(void)
	{
	if (BSearchB(NL, SNL)) BMoveBy(1);
	}


/* ------------------------------------------------------------ */

/* Delete the entire current line. */

void
CLineAED(void)
	{
	BMarkToPoint(cwin->point);
	CLineA();
	RKillToMark(cwin->point, BACKWARD);
	SearchNLF();

		/* force append */
	lastkey = key; lasttable = table;

	RKillToMark(cwin->point, FORWARD);
	}


/* ------------------------------------------------------------ */

/* Move to the previous line. */

void
CLineB(void)
	{
	if (!TabIsVMove(lastkey, lasttable)) last_col = BGetCol();
	while (uarg-- > 0) SearchNLB();
	BMakeColB(last_col);
	}


/* ------------------------------------------------------------ */

/* Move to the end of the current line. */

void
CLineE(void)
	{
	if (BSearchF(NL, SNL)) BMoveBy(-1);
	}


/* ------------------------------------------------------------ */

/* Move to the next line. */

void
CLineF(void)
	{
	if (!TabIsVMove(lastkey, lasttable)) last_col = BGetCol();
	while (uarg-- > 0) SearchNLF();
	BMakeColB(last_col);
	}


/* ------------------------------------------------------------ */

/* Delete to the end of the current line. */

void
CLineFD(void)
	{
	BMarkToPoint(cwin->point);
	if (uarg > 0) {
		if (!BIsEnd() && IsNL())
			BMoveBy(1);
		else	CLineE();
		}
	else	CLineA();
	RKillToMark(cwin->point, uarg > 0);
	}


/* ------------------------------------------------------------ */

/* Move to the previous screen. */

void
CScrnB(void)
	{
	int cnt;
	int vover = Res_Number(RES_CONF, RES_VOVERLAP);

	if (Res_Char(RES_CONF, RES_SCRNMOVE) == 'L') {
		for (cnt = uarg * (DWindHeight() - vover); cnt > 0; --cnt) {
			SearchNLB();
			}
		}
	else	{
		BPointToMark(cwin->sstart);
		for (cnt = uarg * (DWindHeight() - vover) - DPrefLine();
			 cnt > 0; --cnt) {
			SearchNLB();
			}
		CLineA();
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Move the other window to the previous screen. */

void
CScrnBOW(void)
	{
	if (num_windows <= 1) return;
	DWindSwap();
	DIncrDisplay();
	CScrnB();
	DIncrDisplay();
	DWindSwap();
	}


/* ------------------------------------------------------------ */

/* Move to the next screen. */

void
CScrnF(void)
	{
	int cnt;
	int vover = Res_Number(RES_CONF, RES_VOVERLAP);

	if (Res_Char(RES_CONF, RES_SCRNMOVE) == 'L') {
		for (cnt = uarg * (DWindHeight() - vover); cnt > 0; --cnt) {
			SearchNLF();
			}
		}
	else	{
		DIncrDisplay();
		BPointToMark(cwin->sstart);
		for (cnt = uarg * (DWindHeight() - vover) + DPrefLine();
			 cnt > 0; --cnt) {
			SearchNLF();
			}
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Move the other window to the next screen. */

void
CScrnFOW(void)
	{
	if (num_windows <= 1) return;
	DWindSwap();
	DIncrDisplay();
	CScrnF();
	DIncrDisplay();
	DWindSwap();
	}


/* end of FCHAR.C -- Character-, Line-, and Screen -Oriented Commands */
