/* MEMAPP.C -- Get Application Information Routines

	Written April 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "memutil.h"
#undef unlink
#include <dos.h>

/* ------------------------------------------------------------ */

/* This routine returns the NUMAPPS ACBs.  It searches for the string
"JTASK0.EXE  ": the ACBs are assumed to start 80 bytes before that.
However, we use the constant string: */

#define STRING	"KUBTL1/FYF!!"
#define STRINGOFF	0

/* instead, so that we don't find this string by mistake.  The values
here are one higher than those of the actual target.

Return NUMAPPS on success or -1 on failure. */

int
GetApp(struct acb *a)
	{
	char far *fptr;
	int cnt;

/* find the starting string */

	if (!find_str(STRING, GetSysDS(), STRINGOFF)) {
		GetKey("Can't find ACBs");
		return(-1);
		}

/* found it, so fetch data. */

	appoff -= 32 + sizeof(struct acb);
	fptr = MK_FP(appseg, appoff);

	for (cnt = 0; cnt < NUMAPPS * sizeof(struct acb); cnt ++) {
		((char *)a)[cnt] = *fptr++;
		}
	return(NUMAPPS);
	}


/* end of MEMAPP.C -- Get Application Information Routines */
