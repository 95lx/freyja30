/* FWORD.C -- Word-Oriented Commands

	Written July 1991 by Craig A. Finseth
	Copyright 1991,2,3 by Craig A. Finseth
*/

#include "freyja.h"

void W_Fill(void);
FLAG W_IsClose(void);
FLAG W_IsCNumber(void);
FLAG W_IsNLPunct(void);
FLAG W_IsParaEnd(void);
FLAG W_IsSentEnd(void);
FLAG W_IsToken(void);
void W_MoveBlock(struct mark *from, struct mark *to);
void W_ToWord(void);

/* ------------------------------------------------------------ */

/* "Rotate" the case of the current word L -> C -> U -> L. */

void
WCaseRotate(void)
	{
	FLAG waslower;

	BMarkToPoint(cwin->point);
	if (BIsEnd() || !W_IsToken()) WWordB();
	while (!BIsEnd() && !(c.g.ctype[BGetChar()] & CSETMASK_TOK))
		BMoveBy(1);
	if (BIsEnd()) {
		BPointToMark(cwin->point);
		return;
		}

	if (c.g.ctype[BGetChar()] & CSETMASK_LOW) {
		BCharChange(ConvUpper(BGetChar()));
		waslower = FALSE;
		}
	else	{
		BMoveBy(1);
		waslower = c.g.ctype[BGetChar()] & CSETMASK_LOW;
		BMoveBy(-1);
		}
	while (!BIsEnd() && W_IsToken()) {
		BCharChange(waslower ? ConvUpper(BGetChar()) :
			ConvLower(BGetChar()));
		}
	BPointToMark(cwin->point);
	}


/* ------------------------------------------------------------ */

/* Move backward one number. */

void
WNumB(void)
	{
	MoveToB(W_IsCNumber);
	MovePastB(W_IsCNumber);
	}


/* ------------------------------------------------------------ */

/* Move forward one number. */

void
WNumF(void)
	{
	MoveToF(W_IsCNumber);
	MovePastF(W_IsCNumber);
	if (!c.g.wordend) MoveToF(W_IsCNumber);
	}


/* ------------------------------------------------------------ */

/* Mark the current number. */

void
WNumMark(void)
	{
	MovePastB(W_IsCNumber);
	BMarkToPoint(mark);
	MovePastF(W_IsCNumber);
	}


/* ------------------------------------------------------------ */

/* Move backward one paragraph. */

void
WParaB(void)
	{
	GoToNotGrayB();
	if (cbuf->c.fill != 'W') {
		while (SearchNLB()) {
			BMoveBy(1);
			if (W_IsParaEnd()) break;
			BMoveBy(-1);
			}
		}
	else	BSearchB(NL, NL);
	GoToNotGrayF();
	}


/* ------------------------------------------------------------ */

/* Move forward one paragraph. */

void
WParaF(void)
	{
	GoToNotGrayF();
	if (cbuf->c.fill != 'W')
		while (SearchNLF() && !W_IsParaEnd()) ;
	else	BSearchF(NL, NL);
	GoToNotGrayB();
	}


/* ------------------------------------------------------------ */

/* Fill the paragraph. */

void
WParaFill(void)
	{
	if (cbuf->c.fill != 'W')
		W_Fill();
	else	WWrap();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Set the point and mark around paragraph. */

void
WParaMark(void)
	{
	if (isuarg) {
		BMoveToEnd();
		BMarkToPoint(mark);
		BMoveToStart();
		}
	else	{
		BMoveBy(-1);
		WParaF();
		BMarkToPoint(mark);
		WParaB();
		}
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Move backwards one sentence. */

void
WSentB(void)
	{
	FLAG waspend;

	WWordB();
	do	{
		MoveToB(W_IsNLPunct);
		if (BIsStart()) break;
		BMoveBy(-1);
		if (IsNL()) {
			BMoveBy(1);
			waspend = W_IsParaEnd();
			BMoveBy(-1);
			if (waspend) GoToNotGrayF();
			}
		else	{
			waspend = W_IsSentEnd();
			if (waspend && !BIsStart())
				MoveToF(W_IsToken);
			else	MoveToB(W_IsToken);
			if (waspend) GoToGrayB();
			}
		} while (!waspend);
	}


/* ------------------------------------------------------------ */

/* Delete the previous sentence. */

void
WSentBD(void)
	{
	BMarkToPoint(cwin->point);
	WSentB();
	RKillToMark(cwin->point, BACKWARD);
	}


/* ------------------------------------------------------------ */

/* Move forward a sentence. */

void
WSentF(void)
	{
	for (;;) {
		WWordF();
		MoveToF(W_IsNLPunct);
		if (BIsEnd()) break;
		if (IsNL()) {
			BMoveBy(1);
			if (W_IsParaEnd()) break;
			BMoveBy(-1);
			}
		else if (W_IsSentEnd()) break;
		}
	}


/* ------------------------------------------------------------ */

/* Delete the following sentence. */

void
WSentFD(void)
	{
	BMarkToPoint(cwin->point);
	if (uarg == 0) {
		WSentB();
		RKillToMark(cwin->point, BACKWARD);
		}
	else	{
		WSentF();
		RKillToMark(cwin->point, FORWARD);
		}
	}


/* ------------------------------------------------------------ */

/* Move backwards one word. */

void
WWordB(void)
	{
	MoveToB(W_IsToken);
	MovePastB(W_IsToken);
	}


/* ------------------------------------------------------------ */

/* Delete the previous word. */

void
WWordBD(void)
	{
	BMarkToPoint(cwin->point);
	WWordB();
	RKillToMark(cwin->point, BACKWARD);
	}


/* ------------------------------------------------------------ */

/* Capitalize the following word. */

void
WWordCap(void)
	{
	W_ToWord();
	if (BIsEnd()) return;
	BCharChange(ConvUpper(BGetChar()));
	if (W_IsToken()) WWordLow();
	if (!c.g.wordend) W_ToWord();
	}


/* ------------------------------------------------------------ */

/* Move foward one word. */

void
WWordF(void)
	{
	MoveToF(W_IsToken);
	MovePastF(W_IsToken);
	if (!c.g.wordend) W_ToWord();
	}


/* ------------------------------------------------------------ */

/* Delete the following word. */

void
WWordFD(void)
	{
	BMarkToPoint(cwin->point);
	WWordF();
	RKillToMark(cwin->point, FORWARD);
	}


/* ------------------------------------------------------------ */

/* Lowercase the following word. */

void
WWordLow(void)
	{
	W_ToWord();
	while (!BIsEnd() && W_IsToken()) {
		BCharChange(ConvLower(BGetChar()));
		}
	if (!c.g.wordend) W_ToWord();
	}


/* ------------------------------------------------------------ */

/* Transpose the adjoining words. */

void
WWordTran(void)
	{
	struct mark *mptr;

	MoveToF(W_IsToken);
	if (BIsEnd()) return;
	mptr = BMarkCreate();
	MovePastF(W_IsToken);
	BMarkToPoint(cwin->point);
	BPointToMark(mptr);
	MoveToB(W_IsToken);
	W_MoveBlock(mptr, cwin->point);

	MovePastB(W_IsToken);
	W_MoveBlock(mptr, cwin->point);
	BMarkDelete(mptr);
	BPointToMark(cwin->point);
	if (!c.g.wordend) W_ToWord();
	}


/* ------------------------------------------------------------ */

/* Uppercase the following word. */

void
WWordUp(void)
	{
	W_ToWord();
	while (!BIsEnd() && W_IsToken()) {
		BCharChange(ConvUpper(BGetChar()));
		}
	if (!c.g.wordend) W_ToWord();
	}


/* ------------------------------------------------------------ */

/* Refill the current paragraph in wrap mode. */

void
WWrap(void)
	{
	char markstat = NUL;
	int col;
	int chr;
	int wid;
	int lastlen = 0;

	BMarkToPoint(cwin->point);
	if (BSearchB(NL, NL)) BMoveBy(1);	/* to start of paragraph */

	wid = 0;
	for (col = 0; !BIsEnd(); col++, wid++) {
		chr = BGetCharAdv();
		if (chr == SP || chr == TAB) {
			if (markstat == NL && lastlen + col <
				 cbuf->c.right_margin) {
				BMoveBy(-wid - 1);
				BCharChange(SP);
				BMoveBy(wid + 1);
				col += lastlen + 1;
				}
			wid = 0;
			markstat = SP;
			}
		else if (chr == NL) {
			if (markstat == NL && lastlen + col <
				 cbuf->c.right_margin) {
				BMoveBy(-wid - 1);
				BCharChange(SP);
				}
			break;
			}
		else if (chr == SNL) {
			if (markstat == NL && lastlen + col <
				 cbuf->c.right_margin) {
				BMoveBy(-wid - 1);
				BCharChange(SP);
				BMoveBy(wid + 1);
				col += lastlen;
				}
			wid = 0;
			markstat = NL;
			lastlen = col;
			col = -1;
			}
			/* printing character */
		else if (markstat == SP && col >= cbuf->c.right_margin) {
			BMoveBy(-wid - 1);
			chr = BGetChar();
			if (chr != SP && chr != TAB) {
				BMoveBy(-1);
				wid++;
				}
			BCharChange(SNL);
			BMoveBy(wid + 1);
			markstat = NUL;
			col = wid;
			wid = 0;
			}
		}
	BPointToMark(cwin->point);
	}


/* ------------------------------------------------------------ */

/* Fill the current paragraph in hard newline mode. */

void
W_Fill(void)
	{
	struct mark *endptr;
	struct mark *mptr;
	FLAG isnl;

	mptr = BMarkCreate();

	CLineA();
	WParaF();
	endptr = BMarkCreate();
	WParaB();

	while (BIsBeforeMark(endptr)) {
		GoToGrayF();
		if (BGetCol() > cbuf->c.right_margin) {
			GoToGrayB();
			BMoveBy(-1);
			BCharChange(NL);
			BInsSpaces(cbuf->c.left_margin);
			GoToGrayF();
			}
		MovePastF(IsWhite);
		if (IsNL() && BIsBeforeMark(endptr)) {
			BCharChange(SP);
			WDelFWhite();
			}
		}
	BPointToMark(mptr);
	BMarkDelete(endptr);
	BMarkDelete(mptr);
	}


/* ------------------------------------------------------------ */

/* Do we have a closing character? */

FLAG
W_IsClose(void)
	{
	return(c.g.ctype[BGetChar()] & CSETMASK_CLOSE);
	}


/* ------------------------------------------------------------ */

/* Check for a calculator number digit. */

FLAG
W_IsCNumber(void)
	{
	return(c.g.ctype[BGetChar()] & CSETMASK_CNUM);
	}


/* ------------------------------------------------------------ */

/* Check for Newline or punctuation */

FLAG
W_IsNLPunct(void)
	{
	return(c.g.ctype[BGetChar()] & (CSETMASK_SENT | CSETMASK_NL));
	}


/* ------------------------------------------------------------ */

/* Check for the end of a paragraph. */

FLAG
W_IsParaEnd(void)
	{
	return(BIsEnd() || IsGray() || *sindex(".@", BGetChar()) != NUL);
	}


/* ------------------------------------------------------------ */

/* Check for the end of a sentence.  This routine assumes that it
starts at a sentence end (e.g., '.').  It then skips over as many of
')}]"' or "'" as it finds, and tells you whether you wind up at a
whitespace character. */

FLAG
W_IsSentEnd(void)
	{
	BMoveBy(1);
	MovePastF(W_IsClose);
	return(BIsEnd() || IsGray());
	}		


/* ------------------------------------------------------------ */

/* Tell if current char is part of a token */

FLAG
W_IsToken(void)
	{
	return(c.g.ctype[BGetChar()] & CSETMASK_TOK);
	}


/* ------------------------------------------------------------ */

/* Move a block of characters between Point and From to before To.
Assume Point is before From */

void
W_MoveBlock(struct mark *from, struct mark *to)
	{
	int chr;

	while (BIsBeforeMark(from)) {
		chr = BGetChar();
		BMarkSwap(to);
		BInsChar(chr);
		BMarkSwap(to);
		BCharDelete(1);
		}
	}


/* ------------------------------------------------------------ */

/* Move to the beginning of a word. */

void
W_ToWord(void)
	{
	MoveToF(W_IsToken);
	}


/* end of FWORD.C -- Word-Oriented Commands */
