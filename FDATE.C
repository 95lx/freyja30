/* FDATE.C -- Calendar Routines

	Written June 1991 by Craig A. Finseth
	Copyright 1991,2,3 by Craig A. Finseth
*/

#include "freyja.h"

#define CAL_WIDTH	35

	/* Cumulative month start day number.  Yes, I know this has 13
	months. */
static int mdays[] = {
	0,
	0 + 31,
	0 + 31 + 28,
	0 + 31 + 28 + 31,
	0 + 31 + 28 + 31 + 30,
	0 + 31 + 28 + 31 + 30 + 31,
	0 + 31 + 28 + 31 + 30 + 31 + 30,
	0 + 31 + 28 + 31 + 30 + 31 + 30 + 31,
	0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
	0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
	0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
	0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
	0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31 };

static FLAG initted = FALSE;	/* for current calendar */
static int cal_month;
static int cal_year;

static int cal_start = 0;	/* starting day of week, 0 = Sunday */

void D_Calendar(void);
void D_MakeConsistent(int *yearptr, int *monptr);

/* ------------------------------------------------------------ */

/* Create and insert a calendar. */

void
DCal(void)
	{
	struct tm t;

	if (!initted) {
		initted = TRUE;
		DNow(&t);
		cal_month = t.tm_mon;
		cal_year = t.tm_year;
		}
	if (!isuarg) ;
	else if (uarg < 100) {
		cal_month = uarg - 1;
		}
	else if (uarg < 10000) {
		cal_year = uarg;
		}
	else	{
		cal_year = uarg % 10000;
		cal_month = uarg / 10000 - 1;
		}
	D_MakeConsistent(&cal_year, &cal_month);
	D_Calendar();
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Change the calendar by NUM months. */

void
DMove(int num)
	{
	struct tm t;

	if (!initted) {
		initted = TRUE;
		DNow(&t);
		cal_month = t.tm_mon;
		cal_year = t.tm_year;
		}
	cal_month += num;
	D_MakeConsistent(&cal_year, &cal_month);
	D_Calendar();
	}


/* ------------------------------------------------------------ */

/* Advance the calendar by UARG months. */

void
DNext(void)
	{
	if (!isuarg) uarg = 1;
	DMove(uarg);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Return the current date and time in TPTR. */

void
DNow(struct tm *tptr)
	{
	time_t now;

#if defined(SYSMGR)
	JGetDate(&tptr->tm_year, &tptr->tm_mon, &tptr->tm_mday, &tptr->tm_hour,
		&tptr->tm_min, &tptr->tm_sec);
#else
	now = time(NULL);
	*tptr = *localtime(&now);
	tptr->tm_year += 1900;
#endif
	}


/* ------------------------------------------------------------ */

/* Return the day of the week (0 = Sunday) for the supplied day number. */

int
DOW(long day)
	{
	return((day + 0) % 7);
	}


/* ------------------------------------------------------------ */

/* Rewind the calendar by UARG months. */

void
DPrev(void)
	{
	if (!isuarg) uarg = 1;
	DMove(-uarg);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Setup the day of week start (0=sunday). */

void
DSetup(int weekstart)
	{
	cal_start = weekstart;
	}


/* ------------------------------------------------------------ */

/* Convert the date to a day number and return the day number.  CAL is
0=360 day, 2=365 day, other=actual. */

long
DToDayN(struct tm *tptr, int cal)
	{
	long tmp;
	int y;
	int m;
	int d;

	y = tptr->tm_year;
	m = tptr->tm_mon;
	d = tptr->tm_mday;

	D_MakeConsistent(&y, &m);
	if (d < 1 || d > 31) d = 1;

	switch (cal) {

	case 0:
		tmp = (360 * (long)y) + 30 * m + d - 1;
		break;

	case 2:
		tmp = (365 * (long)y) + mdays[m] + d - 1;
		break;

	default:
		tmp = (365 * (long)y) + mdays[m] + d - 1;

			/* Jan and Feb get previous year's leap year counts */
		if (m <= 1) y--;

		tmp += y / 4;		/* add leap years */
		tmp -= y / 100;		/* subtract non-leap cents. */
		tmp += y / 400;		/* add back 400 years */
		break;
		}
	return(tmp);
	}


/* ------------------------------------------------------------ */

/* Display the calendar. */

void
D_Calendar(void)
	{
	struct tm t;
	char buf[LINEBUFFSIZE];
	int cnt;
	int start;
	int numdays;
	long dayn;

	uarg = 0;
	if (!FMakeSys(Res_String(NULL, RES_MSGS, RES_SYSCAL), TRUE)) return;

	xsprintf(buf, "%s %4d", Res_String(NULL, RES_MONTHS, cal_month),
		cal_year);
	BInsSpaces((CAL_WIDTH - strlen(buf) - 1) / 2);
	BInsStr(buf);
	BInsChar(NL);

	for (cnt = 0; cnt < 7; cnt++) {
		xsprintf(buf, " %s    ", Res_String(NULL, RES_DAYS,
			(cnt + cal_start) % 7));
		buf[5] = NUL;
		BInsStr(buf);
		}
	BInsChar(NL);

	t.tm_year = cal_year;
	t.tm_mon = cal_month;
	t.tm_mday = 1;
	dayn = DToDayN(&t, 1);
	start = DOW(dayn);

	t.tm_mon++;
	numdays = DToDayN(&t, 1) - dayn;

	start -= cal_start;
	if (start < 0) start += 7;
	BInsSpaces(5 * start);
	for (cnt = 1; cnt < numdays + 1; cnt++) {
		xsprintf(buf, "  %2d%c", cnt,
			(cnt + start) % 7  == 0 ? NL : SP);
		BInsStr(buf);
		}
	BMoveBy(-1);
	BCharDelete(1);
	BInsChar(NL);
	BMoveToStart();
	}


/* ------------------------------------------------------------ */

/* Bring the month into the range 1-12 and adjust the year
accordingly. */

void
D_MakeConsistent(int *yearptr, int *monptr)
	{
	while (*monptr > 11) {
		(*monptr) -= 12;
		(*yearptr)++;
		}
	while (*monptr < 0) {
		(*monptr) += 12;
		(*yearptr)--;
		}
	if (*yearptr > 9999) {
		*monptr = 11;
		*yearptr = 9999;
		}
	if (*yearptr < 1583) {
		*monptr = 0;
		*yearptr = 1583;
		}
	}


/* end of FDATE.C -- Calendar Routines */
