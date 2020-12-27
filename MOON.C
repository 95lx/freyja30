/* MOON.C -- Print the moon, the time and the phase of the moon
coordinates, for your safety and convenience.

This version taken from MIT-AMG and translated from the PL/I version
(and a previously translated FORTRAN one) into C on 8/28/83 by BNH.
Then re-modified into current form by FIN May 1984, then rewritten Mar
1990.  */

#include "lib.h"
#include <math.h>
#include <time.h>

long time();
struct tm *localtime();

#define STAMPSIZE	100

char *monstr[] = { "January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December" };
char *daystr[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday" };
char *lphasestr[] = { "New Moon", "First Quarter", "Full Moon",
	"Last Quarter" };
char *sphasestr[] = { "NM", "FQ", "FM", "LQ" };
char *whatstr[] = { "Waxing Crescent, DARK:BRIGHT",
	"Waxing Gibbous, DARK:BRIGHT",
	"Waning Gibbous, BRIGHT:DARK",
	"Waning Crescent, BRIGHT:DARK" };

int mndays[] = { 0, 28, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
int lpdays[] = {
	0,			/* 1975 */
	1, 1, 1, 1, 		/* 1976-1979 */
	2, 2, 2, 2,		/* 1980-1983 */
	3, 3, 3, 3,		/* 1984-1987 */
	4, 4, 4, 4, 		/* 1988-1991 */
	5, 5, 5, 5,		/* 1992-1995 */
	6, 6, 6, 6 };		/* 1996-1999 */

struct ttime {
	int year;		/* year-1900 */
	int mon;		/* 0-11 */
	int day;		/* 1-31 */
	int wday;		/* 0 Sun, ... 6 Sat, -1 don't print */
	int hour;		/* 0-23 */
	int min;		/* 0-59 */
	int sec;		/* 0-59 */
	};

struct mtime {
	int mqtr;		/* 0 NM, 1 FQ, 2 FM, 3 LQ */
	int mday;
	int mhour;
	int mmin;
	int msec;
	double frac;	/* 0.0 to 1.0 */
	};

static int opt_isbright = FALSE;
static int opt_isdate = FALSE;
static int opt_isfill = FALSE;
static int opt_islong = FALSE;
static int opt_ismoon = FALSE;
static int opt_isphase = FALSE;
static int opt_isshort = FALSE;
static int opt_iswhat = FALSE;

static int val_rows = 18;
static int val_cols = 79;
static int val_tz = 6;

int leapdays();		/* int yr, int mon */
void moon();		/* struct mtime *m */
int moon_charpos();	/* double x, double scale */
int ToJulian();		/* int mon, int day, int yr */
void tsdate();		/* char *s, struct ttime *t */
void tslong();		/* struct ttime *t, struct mtime *, char *s */
void tsmoon();		/* struct ttime *t, struct mtime *, char *s */
void tspom();		/* struct ttime *t, struct mtime *m */
void tsshort();		/* struct ttime *t, struct mtime *, char *s */
void tswhat();		/* struct ttime *t, struct mtime *, char *s */
void tstime();		/* char *s, struct ttime *t */

/* ------------------------------------------------------------ */

main(argc,argv)
	int argc;
	char *argv[];
	{
	struct ttime t;
	struct mtime m;
	struct tm *tm;
	long tick;
	int cnt;
	int acnt;
	int num;
	char stamp[STAMPSIZE];

	if (argc > 10) {
usage:
		xprintf(
"Display phase of moon program.\n\
usage is: moon [options] [year [mon [day [hour [min [sec]]]]]]\n\
options are:\n\
	[-b|bright]	For displays with bright characters on\n\
			a dark background.\n\
	[-c] <#>	Sets the number of columns (screen width) used by\n\
			the 'phase' picture to #.  Maximum / default #\n\
			is 79.\n\
	[-d|date]	Prints the date in ISO format.\n\
	[-f|fill]	Fill in the background.\n\
	[-j|jaguar]	Set the screen size for 'phase' to be right\n\
			for an HP95LX.\n\
	[-l|long|	Prints the long timestamp.\n\
	 -t|timestamp]\n\
	[-p|phase]	Prints the current phase picture.\n\
	[-m|moon]	Prints the moon timestamp.\n\
	[-r] <#>	Sets the number of val_rows used by the 'phase'\n\
			picture to #.  Default is 18.\n\
	[-s|short]	Prints the short timestamp.\n\
	[-w|what]	Prints the what stamp.\n\
	[-z|zone] <#>	Sets the local time zone offset to #.  Default is 6\n\
			(Central Standard Time)\n%s", COPY);
			exit(0);
		}

	t.year = -1900;
	t.mon = 0;
	t.day = 1;
	t.wday = -1;
	t.hour = 0;
	t.min = 0;
	t.sec = 0;

	for (cnt = 1, acnt = 0; cnt < argc; cnt++) {
		if (strequ(argv[cnt], "-b") || strequ(argv[cnt], "-bright")) {
			opt_isbright = TRUE;
			}
		else if (strequ(argv[cnt], "-c")) {
			if (++cnt >= argc) {
				xeprintf("Missing # for -c option.\n");
				exit(1);
				}
			if (!SToN(argv[cnt], &val_cols, 10)) {
				xeprintf("-c value must be a number.\n");
				exit(1);
				}
			if (val_cols < 16) val_cols = 16;
			if (val_cols > 79) val_cols = 79;
			}
		else if (strequ(argv[cnt], "-d") ||
			 strequ(argv[cnt], "-date")) {
			opt_isdate = TRUE;
			}
		else if (strequ(argv[cnt], "-f") ||
			 strequ(argv[cnt], "-fill")) {
			opt_isfill = TRUE;
			}
		else if (strequ(argv[cnt], "-j") ||
			 strequ(argv[cnt], "-jaguar")) {
			val_cols = 39;
			val_rows = 10;
			}
		else if (strequ(argv[cnt], "-l") ||
			 strequ(argv[cnt], "-long") ||
			 strequ(argv[cnt], "-t") ||
			 strequ(argv[cnt], "-timestamp")) {
			opt_islong = TRUE;
			}
		else if (strequ(argv[cnt], "-m") ||
			 strequ(argv[cnt], "-moon")) {
			opt_ismoon = TRUE;
			}
		else if (strequ(argv[cnt], "-p") ||
			 strequ(argv[cnt], "-phase")) {
			opt_isphase = TRUE;
			}
		else if (strequ(argv[cnt], "-r")) {
			if (++cnt >= argc) {
				xeprintf("Missing # for -r option.\n");
				exit(1);
				}
			if (!SToN(argv[cnt], &val_rows, 10)) {
				xeprintf("-r value must be a number.\n");
				exit(1);
				}
			if (val_rows < 8) val_rows = 8;
			}
		else if (strequ(argv[cnt], "-s") ||
			 strequ(argv[cnt], "-short")) {
			opt_isshort = TRUE;
			}
		else if (strequ(argv[cnt], "-w") ||
			 strequ(argv[cnt], "-what")) {
			opt_iswhat = TRUE;
			}
		else if (strequ(argv[cnt], "-z") ||
			 strequ(argv[cnt], "-zone")) {
			if (++cnt >= argc) {
				xeprintf("Missing # for -z option.\n");
				exit(1);
				}
			if (!SToN(argv[cnt], &val_tz, 10)) {
				xeprintf("-z value must be a number.\n");
				exit(1);
				}
			}
		else if (*argv[cnt] == '-') {
			goto usage;
			}
		else	{
			if (!SToN(argv[cnt], &num, 10)) {
				xeprintf("'%s' must be a number.\n",
					argv[cnt]);
				exit(1);
				}
			switch (acnt) {

			case 0:		t.year = num - 1900;	break;
			case 1:		t.mon = num;		break;
			case 2:		t.day = num;		break;
			case 3:		t.hour = num;		break;
			case 4:		t.min = num;		break;
			case 5:		t.sec = num;		break;
				}
			acnt++;
			}
		}

	if (t.year == -1900) {
		tick = time(0);
		tm = localtime(&tick);

		t.year = tm->tm_year;
		t.mon = tm->tm_mon;
		t.day = tm->tm_mday;
		t.wday = tm->tm_wday;
		t.hour = tm->tm_hour;
		t.min = tm->tm_min;
		t.sec = tm->tm_sec;
		}

	if (!opt_isdate && !opt_islong && !opt_ismoon && !opt_isphase &&
		 !opt_isshort && !opt_iswhat) {
		opt_isphase = TRUE;
		opt_ismoon = TRUE;
		}

	if (opt_isdate) {
		tsdate(&t, &m, stamp);
		xprintf("%s\n", stamp);
		}
	if (opt_isphase) {
		tsmoon(&t, &m, stamp);
		xprintf("\n");
		moon(&m);
		xprintf("\n");
		}
	if (opt_islong) {
		tslong(&t, &m, stamp);
		xprintf("%s\n", stamp);
		}
	if (opt_ismoon) {
		tsmoon(&t, &m, stamp);
		xprintf("%s\n", stamp);
		}
	if (opt_isshort) {
		tsshort(&t, &m, stamp);
		xprintf("%s\n", stamp);
		}
	if (opt_iswhat) {
		tswhat(&t, &m, stamp);
		xprintf("%s\n", stamp);
		}
	exit(0);
	}


/* ------------------------------------------------------------ */

/* Figure the leap days since 1975, valid through 1999. */

int
leapdays(yr, mon)
	int yr;
	int mon;
{
	if (mon < 2) yr--;
	return(lpdays[yr - 75]);
	}


/* ------------------------------------------------------------ */

#define BRIGHT		'@'	/* shows up most screen dots */
#define EDGE		'='	/* the dark edge of the moon */
#define SP		' '
#define NUL		'\0'

#define FULLMOON	0.5	/* the new moon is 0.0 */
#define PI		3.1415926535
#define FUZZ		0.03	/* how far off we must be from an even 0.0 */

/* Draw the phase in shaded representation. */

void
moon(m)
	struct mtime *m;
	{
	double leftedge;	/* left edge of moon */
	double rightedge;	/* right edge of moon */
	double leftdrawn;	/* left edge of drawn part */
	double rightdrawn;	/* right edge of drawn part */
	double line;		/* scan line that we are doing */
	double scale;
	int ileftedge;
	int irightedge;
	int ileftdrawn;
	int irightdrawn;
	int i;
	char buffer[STAMPSIZE];
	double lineheight;

	lineheight = 2.0 / val_rows;
	leftedge = -1.;
	rightedge = 1.;
	if (m->frac < FULLMOON) {
		if ((opt_isbright && !opt_isfill) ||
			 (!opt_isbright && opt_isfill)) {
			leftdrawn = cos(2.0 * PI * m->frac);
			rightdrawn = rightedge;
			}
		else	{
			leftdrawn = leftedge;
			rightdrawn = cos(2.0 * PI * m->frac);
			}
		}
	else	{
		if ((opt_isbright && !opt_isfill) ||
			 (!opt_isbright && opt_isfill)) {
			leftdrawn = leftedge;
			rightdrawn = cos(2.0 * PI * (m->frac - FULLMOON));
			}
		else	{
			leftdrawn = cos(2.0 * PI * (m->frac - FULLMOON));
			rightdrawn = rightedge;
			}
		}

	if (opt_isfill) {
		memset(buffer, opt_isfill ? BRIGHT : SP, val_cols);
		buffer[val_cols] = NUL;
		xprintf("%s\n", buffer);
		}

	for (line = 0.93; line > -1.0; line -= lineheight) {
		memset(buffer, opt_isfill ? BRIGHT : SP, val_cols);
		buffer[val_cols] = NUL;

		scale = sin(acos(line));
		ileftedge = moon_charpos(leftedge, scale);
		irightedge = moon_charpos(rightedge, scale);
		ileftdrawn = moon_charpos(leftdrawn, scale);
		irightdrawn = moon_charpos(rightdrawn, scale);

		buffer[ileftedge] = EDGE;
		buffer[irightedge] = EDGE;

#if defined(DEBUG)
		printf("sc %f, le %f, re %f, ld %f, rd %f\n", scale,
			leftedge, rightedge, leftdrawn, rightdrawn);
		printf("ile %d, ire %d, ild %d, ird %d\n",
			ileftedge, irightedge, ileftdrawn, irightdrawn);
#endif

		if (m->frac > FUZZ && m->frac < (1.0 - FUZZ))  {
			for (i = ileftdrawn; i <= irightdrawn; i++) {
				buffer[i] = opt_isfill ? SP : BRIGHT;
				}
			}

		if (!opt_isfill) buffer[irightedge + 1] = NUL;
		xprintf("%s\n", buffer);
		}

	if (opt_isfill) {
		memset(buffer, opt_isfill ? BRIGHT : SP, val_cols);
		buffer[val_cols] = NUL;
		xprintf("%s\n", buffer);
		}
	}


/* ------------------------------------------------------------ */

/* Compute the position of the character. */

int
moon_charpos(x, scale)
	double x;
	double scale;
	{
	return((val_cols / 2) + x  * scale * val_cols / 3.3);
	}


/* ------------------------------------------------------------ */

/* Convert to Julian day. */

int
ToJulian(mon, day, yr)
	int mon;
	int day;
	int yr;
	{
	day += mndays[mon];
	if (mon > 2 && (yr % 4) == 0) {
		if ((yr % 100) != 0) day++;
		else if ((yr / 100) % 4 == 0) day++;
		else ;
		}
	return(day);
	}


/*------------------------------------------------------------ */

/* Date format:
	1985-05-06 12:04:38
*/

void
tsdate(t, m, s)
	struct ttime *t;
	struct mtime *m;
	char *s;
	{
	xsprintf(s, "%d-%02d-%02d %02d:%02d:%02d",
		t->year + 1900, t->mon + 1, t->day, t->hour, t->min, t->sec);
	}


/*------------------------------------------------------------ */

/* Long format:
	12:04:38pm Sunday, 6 May 1985: FQ+1D 3H 14M 12S
*/

void
tslong(t, m, s)
	struct ttime *t;
	struct mtime *m;
	char *s;
	{
	tstime(s, t);
	tspom(t, m);
	s += strlen(s);

	if (t->wday != -1) {
		xsprintf(s, " %s", daystr[t->wday]);
		s += strlen(s);
		}
	xsprintf(s, ", %d %s %d: %s+%dd %dh %dm %ds",
		t->day, monstr[t->mon], t->year + 1900,
		lphasestr[m->mqtr], m->mday, m->mhour, m->mmin, m->msec);
	}


/* ------------------------------------------------------------ */

/* Moon format:
	1 day, 3 hours, 14 minutes, 12 seconds since the First Quarter
*/

void
tsmoon(t, m, s)
	struct ttime *t;
	struct mtime *m;
	char *s;
	{
	tspom(t, m);

	if (m->mday == 0 && m->mhour == 0 && m->mmin == 0 && m->msec == 0) {
		xsprintf(s, "It is exactly the %s", lphasestr[m->mqtr]);
		return;
		}

	if (m->mday > 0) {
		xsprintf(s, " %d day%s%s", m->mday,
			(m->mday != 1)  ? "s" : "",
			(m->mhour > 0 || m->mmin > 0 || m->msec > 0) ?
				"," : "");
		s += strlen(s);
		}
	if (m->mhour > 0) {
		xsprintf(s, " %d hour%s%s", m->mhour,
			(m->mhour != 1)  ? "s" : "",
			(m->mmin > 0 || m->msec > 0) ? "," : "");
		s += strlen(s);
		}
	if (m->mmin > 0) {
		xsprintf(s, " %d minute%s%s", m->mmin,
			(m->mmin != 1)  ? "s" : "",
			(m->msec > 0) ? "," : "");
		s += strlen(s);
		}
	if (m->msec > 0) {
		xsprintf(s, " %d second%s", m->msec,
			(m->msec != 1)  ? "s" : "");
		s += strlen(s);
		}
	xsprintf(s, " since the %s", lphasestr[m->mqtr]);
	}


/* ------------------------------------------------------------ */

/* Calculate the moon time & fraction.	The base time is 10:21am
1/12/75 (GMT) */

void
tspom(t, m)
	struct ttime *t;
	struct mtime *m;
	{
	int julian;
	int yr;
	long phaselength = 42532;	/* minutes/lunar cycle (approx.) */
	long base;
	long now;
	long diff;
	long phase;
	long hourdiff;
	long daydiff;

	base = 11;		/* 1/12 is 11 days into the year */
	base = base * 24 + 10 - val_tz;		/* now in hours */
	base = base * 60 + 21;			/* now in minutes */

	julian = ToJulian(t->mon, t->day, t->year) - 1;
	yr = t->year - 75;

	now = yr;				/* for year */
	now = now * 365 + julian;		/* now in days */
	now += leapdays(t->year, t->mon);	/* add in leap days */
	now = now * 24 + t->hour;		/* now in hours */
	now = now * 60 + t->min;		/* now in minutes */

	diff = now - base;

	phase = diff - diff / phaselength * phaselength;
	m->frac = (double)phase / (double)phaselength;

	phaselength = phaselength / 4;
	phase = phase / phaselength + 1;
	diff = diff - diff / phaselength * phaselength;
	hourdiff = diff / 60;

	m->mmin = diff - hourdiff * 60;
	m->mday = hourdiff / 24;
	m->mhour = hourdiff - m->mday * 24;
	m->msec = t->sec;
	m->mqtr = phase - 1;
	}


/* ------------------------------------------------------------ */

/* Short format:
	12:04:38pm 5/6/85: FQ+1d 3h 14m 32s
*/

void
tsshort(t, m, s)
	struct ttime *t;
	struct mtime *m;
	char *s;
	{
	tstime(s, t);
	tspom(t, m);
	s += strlen(s);

	xsprintf(s, " %d/%d/%d: %s+%dd %dh %dm %ds",
		t->mon, t->day, t->year,
		sphasestr[m->mqtr], m->mday, m->mhour, m->mmin, m->msec);
	}


/* ------------------------------------------------------------ */

/* Do the time of day */

void
tstime(s, t)
	char *s;
	struct ttime *t;
	{
	if (t->hour == 0) {
		xsprintf(s, "12:%02d:%02dam", t->min, t->sec);
		}
	else if (t->hour < 12) {
		xsprintf(s, "%2d:%02d:%02dam", t->hour, t->min, t->sec);
		}
	else if (t->hour==12) {
		xsprintf(s, "12:%02d:%02dpm", t->min, t->sec);
		}
	else	{
		xsprintf(s, "%2d:%02d:%02dpm", t->hour - 12, t->min, t->sec);
		}
	}


/*------------------------------------------------------------ */

/* What format:
	First Quarter + 1d 3h 14m 32s: Waxing Gibbous, DARK:BRIGHT
*/

void
tswhat(t, m, s)
	struct ttime *t;
	struct mtime *m;
	char *s;
	{
	tstime(s, t);
	tspom(t, m);

	xsprintf(s, " %s + %dd %dh %dm %ds: %s",
		lphasestr[m->mqtr], m->mday, m->mhour, m->mmin, m->msec,
		whatstr[m->mqtr]);
	}


/* end of MOON.C -- Print moon time stamp */
