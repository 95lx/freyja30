/* FTABLE.C -- Command Table Routines

	Written March 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"

/* command table conventions are:

	base		0	RES_BASE
	control-x	1	RES_CTX
	meta		2	RES_ESC
	function keys	3	RES_FUNC
*/

/* In help strings:

	@	no command
	^	self-insert
*/

void (*cmddisp[])() = { MNotImpl, MInsChar,

BBufUnmod,

CCharB, CCharBD, CCharF, CCharFD, CCharTran, CLineA, CLineAED, CLineB,
CLineE, CLineF, CLineFD, CScrnB, CScrnBOW, CScrnF, CScrnFOW,

DCal, DLeft, DNewDisplay, DNext, DPrev, DRight, DScreenRange,
DTogVisGray, DWindGrow, DWindOne, DWindSwap, DWindTog, DWindTwo,
DWindTwoO,

FBufBeg, FBufDel, FBufEnd, FBufList, FBufNext, FBufPrev, FBufPrint,
FCommand, FDIRED, FFileDelete, FFileFind, FFileIns, FFilePath,
FFileRead, FFileSave, FFileWrite, FPrintSel, FSaveAll, FToDIRED,

#if defined(SYSMGR)
JCopy, JCut, JPaste,
#else
MNotImpl, MNotImpl, MNotImpl,
#endif

KBegMac, KColon, KEndMac, KFromMac, KLoadMac,

MAbort, MAbout, MApropos,
#if defined(MSDOS)
MAttr,
#else
MNotImpl,
#endif
MCharAt, MCtrlX, MDate, MDescBind, MDescKey, MDoKey, MExit, MFont,
MIns8, MInsPgBk, MMacEval, MMakeDelete, MMenu, MMenuH, MMenuM, MMeta,
MQuote, MReplace, MReplaceQ, MSearchB, MSearchF, MTime, MTogPgLn,
MUArg,

RDelWhite, RHardToSoft, RIndent, RLower, RMarkSet, RMarkSwap, RMeta,
ROutdent, RPrint, RRegCopy, RRegDelete, RRegFill, RSave, RSoftToHard,
RTabify, RUntabify, RUpper, RYank,

#if !defined(NOCALC)
UCalc, UEnter, UPrintX,
#else
MNotImpl, MNotImpl, MNotImpl,
#endif

WCaseRotate, WIndDel, WIndNext, WIndThis, WInsNL, WInsNLA, WJoinGray,
WLineCenter,
#if !defined(NOCALC)
WNumB, WNumF, WNumMark,
#else
MNotImpl, MNotImpl, MNotImpl,
#endif
WParaB, WParaF, WParaFill, WParaMark, WPrintLine, WPrintMar,
WPrintPos, WSentB, WSentBD, WSentF, WSentFD, WSetFill, WSetLeft,
WSetRight, WSetTabs, WWordB, WWordBD, WWordCap, WWordF, WWordFD,
WWordLow, WWordTran, WWordUp };

static char descrbuf[SMALLBUFFSIZE];

void *Tab_GetCmd(KEYCODE key, int table);
char *Tab_Help(int restable, KEYCODE key);
int Tab_Proc(int restable, KEYCODE key);

/* ------------------------------------------------------------ */

/* Return the command's description. */

char *
TabDescr(KEYCODE key, int table)
	{
	if (key == KEYQUIT)
		return((char *)Res_String(NULL, RES_MSGS, RES_QUITKEY));
	if (key == KEYABORT)
		return((char *)Res_String(NULL, RES_MSGS, RES_ABORTKEY));

	if (key >= 256) table = 3;
	switch (table) {

	case 0:
		if (key > 127 && c.g.meta_handle == 'M')
			return(TabDescr(key & 0x7f, 2));
		return(TPrintChar(key & 0x7F));
		/* break; */

	case 1:
		key &= 0x7F;
		xsprintf(descrbuf, "%s ", TPrintChar(ZCX));
		strcat(descrbuf, TPrintChar(key));
		return(descrbuf);
		/* break; */

	case 2:
		key &= 0x7F;
		xsprintf(descrbuf, "%s ", TPrintChar(ESC));
		strcat(descrbuf, TPrintChar(key));
		return(descrbuf);
		/* break; */

	case 3:
		if (key >= 256) key &= 0xFF;
		return((char *)Res_String(NULL, RES_KEYLABEL, key));
		/* break; */
		}
	}


/* ------------------------------------------------------------ */

/* Invoke the specifed key from the specified table. */

void
TabDispatch(KEYCODE key, int table)
	{
	if (key == KEYQUIT) {
		MExit();
		return;
		}
	if (key == KEYABORT) {
		MAbort();
		return;
		}

	if (key >= 256) table = 3;
	switch (table) {

	case 0:
		if (key > 127) {
			if (c.g.meta_handle == 'M')
				(* (void(*)()) Tab_Proc(RES_ESC, key & 0x7F)
					)();
			else if (c.g.meta_handle == 'I')
				MInsChar();
			else	MNotImpl();
			}		
		else	(* (void(*)()) Tab_Proc(RES_BASE, key & 0x7F) )();
		break;

	case 1:
		(* (void(*)()) Tab_Proc(RES_CTX, key & 0x7F) )();
		break;

	case 2:
		(* (void(*)()) Tab_Proc(RES_ESC, key & 0x7F) )();
		break;

	case 3:
		if (key >= 256) key &= 0xff;
		(* (void(*)()) Tab_Proc(RES_FUNC, key) )();
		break;
		}
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the help string for the specified command. */

char *
TabHelp(KEYCODE key, int table)
	{
	if (key == KEYQUIT)
		return((char *)Res_String(NULL, RES_MSGS, RES_QUITHELP));
	if (key == KEYABORT)
		return((char *)Res_String(NULL, RES_MSGS, RES_ABORTHELP));
	if (key >= 256) table = 3;
	switch (table) {

	case 0:
		if (key > 127) {
			if (c.g.meta_handle == 'M')
				return(Tab_Help(RES_ESC, key & 0x7F));
			else if (c.g.meta_handle == 'I')
				return("^");
			else	return("@");
			}		
		return(Tab_Help(RES_BASE, key & 0x7F));
		/* break; */

	case 1:
		return(Tab_Help(RES_CTX, key & 0x7F));
		/* break; */

	case 2:
		return(Tab_Help(RES_ESC, key & 0x7F));
		/* break; */

	case 3:
		if (key >= 256) key &= 0xff;
		return(Tab_Help(RES_FUNC, key));
		/* break; */
		}
	}


/* ------------------------------------------------------------ */

/* Return the next command table or 0 if not a prefix. */

int
TabTable(KEYCODE key, int table)
	{
	if (key == KEYQUIT) return(0);
	if (key == KEYABORT) return(0);

	if (key >= 256) return(3);
	if (table == 0) {
		if (key == ZCX) return(1);
		else if (key == ESC) return(2);
		else if (key >= 256) return(3);
		}
	return(0);
	}


/* ------------------------------------------------------------ */

/* Return True if the command is a deletion (kill) command or False if
not. */

FLAG
TabIsDelete(KEYCODE key, int table)
	{
	void (*cmd)();

	cmd = (void (*)())Tab_GetCmd(key, table);
	return(cmd == CLineFD ||
		cmd == CLineAED ||
		cmd == MMakeDelete ||
		cmd == RRegDelete ||
		cmd == RRegCopy ||
		cmd == WSentFD ||
		cmd == WWordBD ||
		cmd == WWordFD);
	}


/* ------------------------------------------------------------ */

/* Return True if the command is a vertical motion command or False if
not. */

FLAG
TabIsVMove(KEYCODE key, int table)
	{
	void (*cmd)();

	cmd = (void (*)())Tab_GetCmd(key, table);
	return(cmd == CLineB || cmd == CLineF);
	}


/* ------------------------------------------------------------ */

/* Return the number of function keys. */

int
TabNumFunc(void)
	{
	return(256);
	}


/* ------------------------------------------------------------ */

/* Return the command implied by KEY and TABLE. */

void *
Tab_GetCmd(KEYCODE key, int table)
	{
	void (*cmd)();

	if (key == KEYQUIT) return((void *)MExit);
	if (key == KEYABORT) return((void *)MAbort);
	if (key >= 256) table = 3;
	switch (table) {

	case 0:		cmd = (void (*)())Tab_Proc(RES_BASE, key & 0x7f);break;
	case 1:		cmd = (void (*)())Tab_Proc(RES_CTX, key & 0x7f); break;
	case 2:		cmd = (void (*)())Tab_Proc(RES_ESC, key & 0x7f); break;
	case 3:		if (key >= 256) key &= 0xff;
			cmd = (void (*)()) Tab_Proc(RES_FUNC, key);
			break;
		}
	return((void *)cmd);
	}


/* ------------------------------------------------------------ */

/* Get help for KEY from resource table RESTABLE. */

char *
Tab_Help(int restable, KEYCODE key)
	{
	return((char *)Res_String(NULL, RES_HELPSTR,
		*((unsigned char *)Res_String(NULL, restable, 0) + key)));
	}


/* ------------------------------------------------------------ */

/* Return the procedure that does the KEY from resource table
RESTABLE. */

int
Tab_Proc(int restable, KEYCODE key)
	{
	return((int)cmddisp[
		*((unsigned char *)Res_String(NULL, restable, 0) + key) ]);
	}


/* end of FTABLE.C -- Command Table Routines */
