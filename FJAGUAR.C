/* FJAGUAR.C -- HP95LX (Jaguar) Commands and Support

	Written July 1991 by Craig A. Finseth
	Copyright 1991,2,3 by Craig A. Finseth

NOTE: This entire file is only compiled and linked on IBM PC (MSDOS)
systems.  Therefore, no #ifdef's are required.
*/

#include "freyja.h"

/* ------------------------------------------------------------ */

void
JInit(void)
	{
	}


/* ------------------------------------------------------------ */

void
JFini(void)
	{
	}


/* ------------------------------------------------------------ */

/* End a redisplay. */

void
JDisEnd(void)
	{
	JLightOn();
	}


/* ------------------------------------------------------------ */

/* Start a redisplay. */

void
JDisStart(void)
	{
	JLightOff();
	}


/* ------------------------------------------------------------ */

/* Get a key and handle Jaguar-specific translation. */

KEYCODE
JGetKey(void)
	{
	KEYCODE chr;

	TCurOn();
	chr = BGetKeyE();
	if (chr & 0xFF)
		chr &= 0xFF;
	else	chr = ((chr >>= 8) & 0xFF) + 0x100;
	TCurOff();
	return(chr);
	}


/* ------------------------------------------------------------ */

/* Check for a key press.  Return as KIsKey */

char
JIsKey(void)
	{
	return(BIsKeyE() ? 'Y' : 'N');
	}


/* end of FJAGUAR.C -- HP95LX (Jaguar) Commands */
