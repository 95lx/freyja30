/* SMJAGUAR.C -- HP95/100LX Commands and Support, System-Manager Compliant

	Written March 1992 by Craig A. Finseth
	Copyright 1992,3,4 by Craig A. Finseth

NOTE: This entire file is only compiled and linked on IBM PC (MSDOS)
systems and for the system-manager compliant version.  Therefore, no
#ifdef's are required. */

#include "freyja.h"
#include "sysmgr.h"

FLAG J_Copy(void);
FLAG J_Paste(void);

/* ------------------------------------------------------------ */

void
JInit(void)
	{
	m_init();
	}


/* ------------------------------------------------------------ */

void
JFini(void)
	{
	m_fini();
	}


/* ------------------------------------------------------------ */

/* Allocate a swap area. */

unsigned
JAlloc(unsigned amt)
	{
	return(m_alloc_large(amt));
	}


/* ------------------------------------------------------------ */

/* Copy to the clipboard. */

void
JCopy(void)
	{
	J_Copy();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Cut to the clipboard. */

void
JCut(void)
	{
	if (J_Copy()) BRegDelete(mark);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* End a redisplay. */

void
JDisEnd(void)
	{
	JLightOn();
	m_unlock();
	}


/* ------------------------------------------------------------ */

/* Start a redisplay. */

void
JDisStart(void)
	{
	m_lock();
	JLightOff();
	}


/* ------------------------------------------------------------ */

/* Free a swap area. */

void
JFree(unsigned what)
	{
	m_free_large(what);
	}


/* ------------------------------------------------------------ */

/* Return the current date. */

void
JGetDate(int *year, int *mon, int *day, int *hour, int *min, int *sec)
	{
	DTM d;

	m_getdtm(&d);
	*year = d.dt_year;
	*mon = d.dt_month - 1;
	*day = d.dt_day;
	*hour = d.dt_hour;
	*min = d.dt_minute;
	*sec = d.dt_second;
	}


/* ------------------------------------------------------------ */

/* Returns the default configuration file directory. DNAME must point
to an FNAMEMAX-sized buffer. */

void
JGetDir(char *dname)
	{
	m_get_sysdir(dname);
	}


/* ------------------------------------------------------------ */

/* Get a file name using the file getter.  Return True on success or
False on abort. */

FLAG
JGetFile(int promptnum, char *fname)
	{
	FILEINFO fi[100];
	FMENU f;
	EDITDATA ed;
	EVENT e;
	FLAG isdone = FALSE;
	FLAG ok = TRUE;
	char dn[FNAMEMAX];
	char fn[FNAMEMAX];
	char *cptr;
	int save_type = screen_type;

	if (screen_type == 25 || screen_type == 26) {
		TFini();
		screen_type = 24;
		TInit();
		DNewDisplay();
		}
	xstrcpy(dn, fname);
	for (cptr = dn + strlen(dn); cptr > dn; --cptr) {
		if (*cptr == ':' || *cptr == '/' || *cptr == '\\') break;
		}
	if (*cptr == ':' || *cptr == '/' || *cptr == '\\') {
		xstrcpy(fn, cptr + 1);
		*(cptr + 1) = NUL;
		}
	else	{
		xstrcpy(fn, cptr);
		*cptr = NUL;
		}

	if (*fn == NUL)
		xstrcpy(fn, Res_String(NULL, RES_CONF, RES_DEFPAT));
	if (strlen(fn) > 12) fn[12] = NUL;
	if (sindex(fn, '.') - fn > 8) fn[8] = NUL;
	if (*dn == NUL) xstrcpy(dn, "C:\\");

	f.fm_path = dn;
	f.fm_pattern = fn;
	f.fm_buffer = fi;
	f.fm_buf_size = sizeof(fi);
	f.fm_startline = -2;
	f.fm_startcol = 0;
	f.fm_numlines = 14;
	f.fm_numcols = 40;
	f.fm_filesperline = 3;

	ed.prompt_window = 1;
	ed.prompt_line_length = 0;
	cptr = (char *)Res_String(NULL, RES_MSGS, promptnum);
	ed.message_line = cptr;
	ed.message_line_length = strlen(cptr);

	m_clear(-3, 0, 15, 40);

	if (fmenu_init(&f, &ed, "", 0, 0) != RET_OK) {
		if (save_type == 25 || save_type == 26) {
			TFini();
			screen_type = save_type;
			TInit();
			DNewDisplay();
			}
		DError(RES_GETTER);
		return(FALSE);
		}

	VidCurOff();
	e.kind = E_KEY;
	while (!isdone) {
		if ((e.kind == E_KEY || e.kind == E_ACTIV) && KIsKey() != 'Y') {
			JLightOff();
			fmenu_dis(&f, &ed);
			JLightOn();
			}
		if (KMacIs() == 'Y') {
			e.kind = E_KEY;
			e.data = KGetChar();
			}
		else	{
			m_event(&e);
			if (e.kind == E_KEY) KMacRec(e.data);
			}

		switch (e.kind) {

		case E_ACTIV:
			DNewDisplay();
			DIncrDisplay();
			break;

		case E_TERM:
			KPush(KEYQUIT);
			isdone = TRUE;
			break;
		
		case E_BREAK:
			ok = FALSE;
			isdone = TRUE;
			break;

		case E_KEY:
			if (e.data == ESC || e.data == BEL) {
				ok = FALSE;
				isdone = TRUE;
				}
			else	{
				switch (fmenu_key(&f, &ed, e.data)) {

				case RET_UNKNOWN:
				case RET_BAD:
					TBell();
					break;

				case RET_OK:
					break;

				case RET_REDISPLAY:
					DModeLine();
					break;

				case RET_ACCEPT:
					isdone = TRUE;
					break;

				case RET_ABORT:
					ok = FALSE;
					isdone = TRUE;
					break;
					}
				}
			break;
			}
		}

	fmenu_off(&f, &ed);
	VidCurOn();
	xstrcpy(fname, ed.edit_buffer);
	if (save_type == 25 || save_type == 26) {
		TFini();
		screen_type = save_type;
		TInit();
		}
	DNewDisplay();
	return(ok);
	}


/* ------------------------------------------------------------ */

/* Get a key and handle Jaguar-specific translation. */

int
JGetKey(void)
	{
	int chr;
	FLAG ison = TRUE;
	EVENT e;

	for (;;) {
		if (ison) TCurOn();
		m_event(&e);
		if (ison) TCurOff();
		switch (e.kind) {

		case E_ACTIV:
			TInit();
			DNewDisplay();
			DIncrDisplay();
			return(KEYREGEN);
			/*break;*/

		case E_TERM:
			return(KEYQUIT);
			/*break;*/

		case E_BREAK:
			return(KEYABORT);
			/*break;*/

		case E_KEY:
			chr = e.data;
			if (chr & 0xFF)
				chr &= 0xFF;
			else	chr = ((chr >>= 8) & 0xFF) + 0x100;
			return(chr);
			/*break;*/

		case E_NONE:
			ison = !ison;
			break;
			}
		}
	}


/* ------------------------------------------------------------ */

/* Check for a key press.  Return as KIsKey */

char
JIsKey(void)
	{
	EVENT e;

	m_nevent(&e);
	switch (e.kind) {

	case E_ACTIV:
		TInit();
		DNewDisplay();
		DIncrDisplay();
		KPush(KEYREGEN);
		return('Y');
		/*break;*/

	case E_TERM:
		KPush(KEYQUIT);
		return('Y');
		/*break;*/
		
	case E_KEY:
		return('Y');
		/*break;*/
		}
	return('N');
	}


/* ------------------------------------------------------------ */

/* Cancel an exit operation. */

void
JNoFini(void)
	{
	EVENT e;

	m_no_fini(&e);
	}


/* ------------------------------------------------------------ */

/* Paste from the clipboard. */

void
JPaste(void)
	{
	J_Paste();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Execute a system command. */

void
JSystem(char *cmd_line)
	{
	char buf[BUFFSIZE];

	xstrcpy(buf, cmd_line);
	strcat(buf, "\r\n");
	m_spawn(buf, strlen(buf) - 2, 2, buf);
	}


/* ------------------------------------------------------------ */

/* Copy the region's contents into the buffer. Return True on
success or False if the caller should wait for a keyboard press. */

FLAG
J_Copy(void)
	{
	char chr;
	FLAG isafter;

	if (m_open_cb() != 0) {
		DError(RES_CLIPOPEN);
		return(FALSE);
		}
	if (m_reset_cb(Res_String(NULL, RES_CONF, RES_CBPROG)) != 0) {
		m_close_cb();
		DError(RES_CLIPINIT);
		return(FALSE);
		}
	m_new_rep(Res_String(NULL, RES_CONF, RES_CBTAG));

/* cycle over the region */

	if (isafter = BIsAfterMark(mark)) BMarkSwap(mark);
	BMarkToPoint(cwin->point);
	while (!BIsEnd() && BIsBeforeMark(mark)) {
		chr = BGetCharAdv();
		if (!isuarg) {
			if (cbuf->c.fill == 'W' && chr == SNL)
				chr = SP;
			else if (chr == NL)
				chr = CR;
			}
		m_cb_write(&chr, 1);
		}
	BPointToMark(cwin->point);
	if (isafter) BMarkSwap(mark);

	m_fini_rep();
	m_close_cb();
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Paste the clipboard's contents into the buffer. Return True on
success or False on error. */

FLAG
J_Paste(void)
	{
	int index;
	int len;
	char chr;
	int cnt;

	if (m_open_cb() != 0) {
		DError(RES_CLIPOPEN);
		return(FALSE);
		}
	if (m_rep_index(Res_String(NULL, RES_CONF, RES_CBTAG), &index,
		 &len) != 0) {
		m_close_cb();
		DError(RES_PASTE);
		return(FALSE);
		}

	BMarkToPoint(mark);
	for (cnt = 0; cnt < len; cnt++) {
		m_cb_read(index, cnt, &chr, 1);
		if (!isuarg && chr == '\xd') chr = NL;
		if (!BInsChar(chr)) break;
		}
	m_close_cb();
	return(TRUE);
	}


/* end of  SMJAGUAR.C -- HP95/100LX Commands and Support, System-Manager
Compliant */
