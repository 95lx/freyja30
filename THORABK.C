/* THORABK.C -- Convert HP95LX Appointment Book Files To ASCII

	Written September 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "lib.h"

/* -------------------- Phone Book Records -------------------- */

#define ABKNAMEMAX	30
#define ABKNUMBERMAX	30
#define ABKADDRESSMAX	39
#define ABKADDRESSLINES	8

struct abk_id {
	unsigned char	ProductCodeL;	/* must be -1 (0xFFFF) */
	unsigned char	ProductCodeH;
	unsigned char	ReleaseNumL;	/* must be 1 */
	unsigned char	ReleaseNumH;
	unsigned char	FileType;	/* must be 1 */
	};

struct abk_settings {		/* always the second record */
	unsigned char	StartTimeL;	/* display start as minutes */
	unsigned char	StartTimeH;
	unsigned char	GranularityL;	/* line granularity in minutes */
	unsigned char	GranularityH;
	unsigned char	AlarmEnable;	/* 1)on, 0)off */
	unsigned char	LeadTime;	/* default lead time in minutes */
	unsigned char	CarryForward;	/* 1)on, 0)off */
	};

struct abk_daily {
	unsigned char	RecordType;	/* must be 1 */
	unsigned char	RecordLengthL;	/* number of bytes after, may be padded */
	unsigned char	RecordLengthH;
	unsigned char	ApptState;	/* 2^0=1)alarm on, 2^0=0)off */
	unsigned char	Year;		/* - 1900 */
	unsigned char	Month;		/* 1 - 12 */
	unsigned char	Day;		/* 1 - 31 */
	unsigned char	StartTimeH;	/* in minutes since midnight */
	unsigned char	StartTimeL;
	unsigned char	EndTimeL;	/* in minutes since midnight */
	unsigned char	EndTimeH;
	unsigned char	LeadTime;
	unsigned char	ApptLength;	/* length of appointment text */
	unsigned char	NoteLengthL;	/* length of note text */
	unsigned char	NoteLengthH;
	unsigned char	data[27 + 11* 40];	/* order is appt text, note
					text (NUL is line delimiter) */
	};

struct abk_weekly {
	unsigned char	RecordType;	/* must be 2 */
	unsigned char	RecordLengthL;	/* number of bytes after, may be padded */
	unsigned char	RecordLengthH;
	unsigned char	ApptState;	/* 2^0=1)alarm on, 2^0=0)off */
	unsigned char	DayOfWeek;	/* 1)Sun... */
	unsigned char	StartTimeH;	/* in minutes since midnight */
	unsigned char	StartTimeL;
	unsigned char	StartYear;	/* - 1900 */
	unsigned char	StartMonth;	/* 1 - 12 */
	unsigned char	StartDay;	/* 1 - 31 */
	unsigned char	EndTimeL;	/* in minutes since midnight */
	unsigned char	EndTimeH;
	unsigned char	EndYear;	/* - 1900 */
	unsigned char	EndMonth;	/* 1 - 12 */
	unsigned char	EndDay;		/* 1 - 31 */
	unsigned char	LeadTime;
	unsigned char	ApptLength;	/* length of appointment text */
	unsigned char	NoteLengthL;	/* length of note text */
	unsigned char	NoteLengthH;
	unsigned char	data[27 + 11* 40];	/* order is appt text, note
					text (NUL is line delimiter) */
	};

struct abk_mondate {
	unsigned char	RecordType;	/* must be 3 */
	unsigned char	RecordLengthL;	/* number of bytes after, may be padded */
	unsigned char	RecordLengthH;
	unsigned char	ApptState;	/* 2^0=1)alarm on, 2^0=0)off */
	unsigned char	DayOfMonth;	/* 1 - 31 */
	unsigned char	StartTimeH;	/* in minutes since midnight */
	unsigned char	StartTimeL;
	unsigned char	StartYear;	/* - 1900 */
	unsigned char	StartMonth;	/* 1 - 12 */
	unsigned char	StartDay;	/* 1 - 31 */
	unsigned char	EndTimeL;	/* in minutes since midnight */
	unsigned char	EndTimeH;
	unsigned char	EndYear;	/* - 1900 */
	unsigned char	EndMonth;	/* 1 - 12 */
	unsigned char	EndDay;		/* 1 - 31 */
	unsigned char	LeadTime;
	unsigned char	ApptLength;	/* length of appointment text */
	unsigned char	NoteLengthL;	/* length of note text */
	unsigned char	NoteLengthH;
	unsigned char	data[27 + 11* 40];	/* order is appt text, note
					text (NUL is line delimiter) */
	};

struct abk_monpos {
	unsigned char	RecordType;	/* must be 4 */
	unsigned char	RecordLengthL;	/* number of bytes after, may be padded */
	unsigned char	RecordLengthH;
	unsigned char	ApptState;	/* 2^0=1)alarm on, 2^0=0)off */
	unsigned char	WeekOfMonth;	/* 1 - 5 */
	unsigned char	DayOfWeek;	/* 1)Sun... */
	unsigned char	StartTimeH;	/* in minutes since midnight */
	unsigned char	StartTimeL;
	unsigned char	StartYear;	/* - 1900 */
	unsigned char	StartMonth;	/* 1 - 12 */
	unsigned char	StartDay;	/* 1 - 31 */
	unsigned char	EndTimeL;	/* in minutes since midnight */
	unsigned char	EndTimeH;
	unsigned char	EndYear;	/* - 1900 */
	unsigned char	EndMonth;	/* 1 - 12 */
	unsigned char	EndDay;		/* 1 - 31 */
	unsigned char	LeadTime;
	unsigned char	ApptLength;	/* length of appointment text */
	unsigned char	NoteLengthL;	/* length of note text */
	unsigned char	NoteLengthH;
	unsigned char	data[27 + 11* 40];	/* order is appt text, note
					text (NUL is line delimiter) */
	};

struct abk_year {
	unsigned char	RecordType;	/* must be 5 */
	unsigned char	RecordLengthL;	/* number of bytes after, may be padded */
	unsigned char	RecordLengthH;
	unsigned char	ApptState;	/* 2^0=1)alarm on, 2^0=0)off */
	unsigned char	MonthOfYear;	/* 1 - 12 */
	unsigned char	DayOfMonth;	/* 1 - 31 */
	unsigned char	StartTimeH;	/* in minutes since midnight */
	unsigned char	StartTimeL;
	unsigned char	StartYear;	/* - 1900 */
	unsigned char	StartMonth;	/* 1 - 12 */
	unsigned char	StartDay;	/* 1 - 31 */
	unsigned char	EndTimeL;	/* in minutes since midnight */
	unsigned char	EndTimeH;
	unsigned char	EndYear;	/* - 1900 */
	unsigned char	EndMonth;	/* 1 - 12 */
	unsigned char	EndDay;		/* 1 - 31 */
	unsigned char	LeadTime;
	unsigned char	ApptLength;	/* length of appointment text */
	unsigned char	NoteLengthL;	/* length of note text */
	unsigned char	NoteLengthH;
	unsigned char	data[27 + 11* 40];	/* order is appt text, note
					text (NUL is line delimiter) */
	};

struct abk_todo {
	unsigned char	RecordType;	/* must be 6 */
	unsigned char	RecordLengthL;	/* number of bytes after, may be padded */
	unsigned char	RecordLengthH;
	unsigned char	ToDoState;	/* 2^0=1)carry forward on, 2^0=0)off */
					/* 2^1=1)checked off, 2^1=0)not */
	unsigned char	Priority;	/* 1 - 9 */
	unsigned char	StartYear;	/* - 1900 */
	unsigned char	StartMonth;	/* 1 - 12 */
	unsigned char	StartDay;	/* 1 - 31 */
	unsigned char	CheckOffYear;	/* - 1900, 0 not checked off */
	unsigned char	CheckOffMonth;	/* 1 - 12, 0 not checked off */
	unsigned char	CheckOffDay;	/* 1 - 31, 0 not checked off */
	unsigned char	ToDoLength;	/* length of to do text */
	unsigned char	NoteLengthL;	/* length of note text */
	unsigned char	NoteLengthH;
	unsigned char	data[27 + 11* 40];	/* order is appt text, note
					text (NUL is line delimiter) */
	};

struct abk_eof {
	unsigned char	RecordType;	/* must be 50 (0x32) */
	unsigned char	RecordLengthL;	/* must be 0 */
	unsigned char	RecordLengthH;
	};

/* ------------------------------------------------------------ */

static char *weekdays[] =
	{ "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

static FLAG opt_v = FALSE;

static char *from_file = NULL;
static char *to_file = NULL;

static char format_buf[20];

char *Format_Time();	/* char high, char low */
FLAG From();		/* void */
void From_Daily();	/* int ofd, unsigned char *buf */
void From_Weekly();	/* int ofd, unsigned char *buf */
void From_MonDate();	/* int ofd, unsigned char *buf */
void From_MonPos();	/* int ofd, unsigned char *buf */
void From_Year();	/* int ofd, unsigned char *buf */
void From_ToDo();	/* int ofd, unsigned char *buf */
void Note();		/* int ofd, unsigned char *data, int apptlen,
			int notelen */

/* ------------------------------------------------------------ */

int
main(argc,argv)
	int argc;
	char *argv[];
	{
	int cnt;
	FLAG ok;

	if (argc < 3) {
usage:
		xprintf("Appointment book conversion program\n\
usage is: thorabk <from> <to>\n\
options are:\n\
	[-v|verbose]	show records as processed\n%s", COPY);
		exit(1);
		}

	for (cnt = 1; cnt < argc; ++cnt) {
		if (strequ(argv[cnt], "-v") ||
			 strequ(argv[cnt], "-verbose")) {
			opt_v = TRUE;
			}
		else if (*argv[cnt] == '-' && *(argv[cnt] + 1) != NUL) {
			xprintf("Unknown option '%s'.\n", argv[cnt]);
			goto usage;
			}
		else if (from_file == NULL) {
			from_file = argv[cnt];
			}
		else	to_file = argv[cnt];
		}

	ok = From();
	exit(ok ? 0 : 1);
	}


/* ------------------------------------------------------------ */

/* Format HIGH and LOW into a time and return a pointer to a static
buffer. */

char *
Format_Time(high, low)
	unsigned char high;
	unsigned char low;
	{
	int min = high * 256 + low;

	xsprintf(format_buf, "%d:%02d", min / 60, min % 60);
	return(format_buf);
	}


/* ------------------------------------------------------------ */

/* Convert an appointment book to an ASCII file.  Return True on
success or False on failure. */

FLAG
From()
	{
	struct abk_id ai;
	struct abk_settings as;
	unsigned char buf[sizeof(struct abk_year) /* largest */];
	int ifd;
	int ofd;
	int size;
	int amt;

/* open input */

	if (strequ(from_file, "-")) {
		ifd = 0;
		}
	else if ((ifd = open(from_file, O_RDONLY)) < 0) {
		xeprintf("Unable to open input file '%s'.\n", from_file);
		return(FALSE);
		}
	if ((amt = read(ifd, (char *)&ai, 5)) != 5 ||
		 ai.ProductCodeL != 0xFF ||
		 ai.ProductCodeH != 0xFF ||
		 ai.ReleaseNumL != 1 ||
		 ai.ReleaseNumH != 0 ||
		 ai.FileType != 1) {
		if (opt_v) {
			xprintf("amt %d: %d %d %d %d %d\n", amt,
				ai.ProductCodeL, ai.ProductCodeH,
				ai.ReleaseNumL, ai.ReleaseNumH,
				ai.FileType);
			}
		xeprintf("Input fine %s is not a valid appointment book.\n",
			from_file);
		close(ifd);
		return(FALSE);
		}

	if ((amt = read(ifd, (char *)&as, 7)) != 7) {
		xeprintf("Input fine %s is not a valid appointment book.\n",
			from_file);
		close(ifd);
		return(FALSE);
		}

/* open output */

	if (strequ(to_file, "-")) {
		ofd = 1;
		}
	else if ((ofd = creat(to_file, 0666)) < 0) {
		xeprintf("Unable to create output file '%s'.\n", to_file);
		close(ifd);
		return(FALSE);
		}

	xdprintf(ofd,
"settings start %s   granul. %d   alarms %s (%d)   lead %d   carryfwd %s (%d)\n\n",
		Format_Time(as.StartTimeH, as.StartTimeL),
		as.GranularityH * 256 + as.GranularityL,
		(as.AlarmEnable & 0x1) ? "on" : "off",
		as.AlarmEnable,
		as.LeadTime,
		(as.CarryForward & 0x1) ? "on" : "off",
		as.CarryForward);

/* read the input */

	while ((amt = read(ifd, (char *)buf, 3)) == 3) {
		if (buf[0] == 0x32) {
			amt = 0;
			break;
			}

		size = (buf[2] << 8) | buf[1];
		if (size < 4 || size > sizeof(buf) + 3) {
			xeprintf("Invalid record size %d.\n", size);
			break;
			}
		if (opt_v) {
			xprintf("record type %d and size %d\n", buf[0], size);
			}
		if ((amt = read(ifd, &buf[3], size)) != size) {
			break;
			}

		switch (buf[0]) {

		case 1:	From_Daily(ofd, buf);	break;
		case 2:	From_Weekly(ofd, buf);	break;
		case 3:	From_MonDate(ofd, buf);	break;
		case 4:	From_MonPos(ofd, buf);	break;
		case 5:	From_Year(ofd, buf);	break;
		case 6:	From_ToDo(ofd, buf);	break;
		default:
			xeprintf("Unknown appointment book record type %d.\n",
				buf[0]);
			break;
			}
		}

/* handle read error */

	if (amt != 0) {
		xeprintf("Read error on input file %s.\n", from_file);
		close(ifd);
		close(ofd);
		return(FALSE);
		}

/* ok */

	close(ifd);
	close(ofd);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Print the daily view. */

void
From_Daily(ofd, buf)
	int ofd;
	unsigned char *buf;
	{
	struct abk_daily *adaily = (struct abk_daily *)buf;
	
	xdprintf(ofd, "daily %d-%d-%d %s-",
		adaily->Year + 1900,
		adaily->Month,
		adaily->Day,
		Format_Time(adaily->StartTimeH, adaily->StartTimeL));

	xdprintf(ofd, "%s alarm %s(%d) lead %d\n",
		Format_Time(adaily->EndTimeH, adaily->EndTimeL),
		(adaily->ApptState & 0x1) ? "on" : "off",
		adaily->ApptState,
		adaily->LeadTime);

	Note(ofd, adaily->data, adaily->ApptLength,
		adaily->NoteLengthH * 256 + adaily->NoteLengthL);
	}


/* ------------------------------------------------------------ */

/* Print the weekly view. */

void
From_Weekly(ofd, buf)
	int ofd;
	unsigned char *buf;
	{
	struct abk_weekly *aweekly = (struct abk_weekly *)buf;

	xdprintf(ofd, "weekly %s %d-%d-%d %s-",
		weekdays[aweekly->DayOfWeek],
		aweekly->StartYear + 1900,
		aweekly->StartMonth,
		aweekly->StartDay,
		Format_Time(aweekly->StartTimeH, aweekly->StartTimeL));

	xdprintf(ofd, "%s end %d-%d-%d alarm %s(%d) lead %d\n",
		Format_Time(aweekly->EndTimeH, aweekly->EndTimeL),
		aweekly->EndYear + 1900,
		aweekly->EndMonth,
		aweekly->EndDay,
		(aweekly->ApptState & 0x1) ? "on" : "off",
		aweekly->ApptState,
		aweekly->LeadTime);

	Note(ofd, aweekly->data, aweekly->ApptLength,
		aweekly->NoteLengthH * 256 + aweekly->NoteLengthL);
	}
	

/* ------------------------------------------------------------ */

/* Print the mondate view. */

void
From_MonDate(ofd, buf)
	int ofd;
	unsigned char *buf;
	{
	struct abk_mondate *amondate = (struct abk_mondate *)buf;

	xdprintf(ofd, "monthly/date %d %d-%d-%d %s-",
		amondate->DayOfMonth,
		amondate->StartYear + 1900,
		amondate->StartMonth,
		amondate->StartDay,
		Format_Time(amondate->StartTimeH, amondate->StartTimeL));

	xdprintf(ofd, "%s end %d-%d-%d alarm %s(%d) lead %d\n",
		Format_Time(amondate->EndTimeH, amondate->EndTimeL),
		amondate->EndYear + 1900,
		amondate->EndMonth,
		amondate->EndDay,
		(amondate->ApptState & 0x1) ? "on" : "off",
		amondate->ApptState,
		amondate->LeadTime);

	Note(ofd, amondate->data, amondate->ApptLength,
		amondate->NoteLengthH * 256 + amondate->NoteLengthL);
	}
	

/* ------------------------------------------------------------ */

/* Print the monpos view. */

void
From_MonPos(ofd, buf)
	int ofd;
	unsigned char *buf;
	{
	struct abk_monpos *amonpos = (struct abk_monpos *)buf;

	xdprintf(ofd, "monthly/position %d %s %d-%d-%d %s-",
		amonpos->WeekOfMonth,
		weekdays[amonpos->DayOfWeek],
		amonpos->StartYear + 1900,
		amonpos->StartMonth,
		amonpos->StartDay,
		Format_Time(amonpos->StartTimeH, amonpos->StartTimeL));

	xdprintf(ofd, "%s end %d-%d-%d alarm %s(%d) lead %d\n",
		Format_Time(amonpos->EndTimeH, amonpos->EndTimeL),
		amonpos->EndYear + 1900,
		amonpos->EndMonth,
		amonpos->EndDay,
		(amonpos->ApptState & 0x1) ? "on" : "off",
		amonpos->ApptState,
		amonpos->LeadTime);

	Note(ofd, amonpos->data, amonpos->ApptLength,
		amonpos->NoteLengthH * 256 + amonpos->NoteLengthL);
	}
	

/* ------------------------------------------------------------ */

/* Print the year view. */

void
From_Year(ofd, buf)
	int ofd;
	unsigned char *buf;
	{
	struct abk_year *ayear = (struct abk_year *)buf;

	xdprintf(ofd, "year %d-%d %d-%d-%d %s-",
		ayear->MonthOfYear,
		ayear->DayOfMonth,
		ayear->StartYear + 1900,
		ayear->StartMonth,
		ayear->StartDay,
		Format_Time(ayear->StartTimeH, ayear->StartTimeL));

	xdprintf(ofd, "%s end %d-%d-%d alarm %s(%d) lead %d\n",
		Format_Time(ayear->EndTimeH, ayear->EndTimeL),
		ayear->EndYear + 1900,
		ayear->EndMonth,
		ayear->EndDay,
		(ayear->ApptState & 0x1) ? "on" : "off",
		ayear->ApptState,
		ayear->LeadTime);

	Note(ofd, ayear->data, ayear->ApptLength,
		ayear->NoteLengthH * 256 + ayear->NoteLengthL);
	}
	

/* ------------------------------------------------------------ */

/* Print the todo view. */

void
From_ToDo(ofd, buf)
	int ofd;
	unsigned char *buf;
	{
	struct abk_todo *atodo = (struct abk_todo *)buf;

	xdprintf(ofd, "todo %d %d-%d-%d carry %s checked %s(%d) %d-%d-%d\n",
		atodo->Priority,
		atodo->StartYear + 1900,
		atodo->StartMonth,
		atodo->StartDay,
		(atodo->ToDoState & 0x1) ? "on" : "off",
		(atodo->ToDoState & 0x2) ? "off" : "not",
		atodo->ToDoState,
		atodo->CheckOffYear + 1900,
		atodo->CheckOffMonth,
		atodo->CheckOffDay);

	Note(ofd, atodo->data, atodo->ToDoLength,
		atodo->NoteLengthH * 256 + atodo->NoteLengthL);
	}


/* ------------------------------------------------------------ */

/* Print the text. */

void
Note(ofd, data, apptlen, notelen)
	int ofd;
	unsigned char *data;
	int apptlen;
	int notelen;
	{
	unsigned char buf[BUFFSIZE];
	unsigned char *cptr;
	int datasize;
	int where;

	if (opt_v) {
		xprintf("apptlen %d, notelen %d\n", apptlen, notelen);
		}
	datasize = apptlen + notelen;
	if (datasize < 0 || datasize > sizeof(struct abk_year)) {
		xeprintf("Internal inconsistency.\n");
		return;
		}

	where = 0;

	memmove(buf, &data[where], apptlen);
	where += apptlen;
	buf[apptlen] = NUL;
	xdprintf(ofd, "\t%s\n", buf);

	memmove(buf, &data[where], notelen);
	buf[notelen] = NUL;
	for (cptr = buf; cptr < &buf[notelen]; cptr += strlen(cptr) + 1) {
		xdprintf(ofd, "\t%s\n", cptr);
		}
	xdprintf(ofd, "\n");
	}


/* end of THORABK.C -- Convert HP95LX Phone Book Files To/From ASCII */
