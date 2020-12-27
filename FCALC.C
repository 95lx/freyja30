/* FCALC.C -- RPN Calculator

	Written March 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"
#include <math.h>

	/* ---------- configuration ---------- */

#define REGCOUNT	20	/* number of user registers */
#define BINSIZE		32	/* max size of a binary in bits */
#define WORKSIZE	20	/* amount of "working input" space */

#define SYS_CALC	"%calc%"

	/* ---------- commands ---------- */

enum CMDS { CM_ADD, CM_AND, CM_B, CM_CF, CM_CLRG, CM_CLST, CM_CLX,
CM_D, CM_DEFAULT, CM_DIGSEPOFF, CM_DIGSEPON, CM_DIV, CM_ENTER,
CM_FACT, CM_H, CM_HELP, CM_INV, CM_LASTX, CM_MEMVIEW, CM_MOD, CM_MUL,
CM_NEG, CM_NOT, CM_NULL, CM_NUM, CM_O, CM_OR, CM_PCT, CM_PCTCH,
CM_PCTTOT, CM_RADIXC, CM_RADIXD, CM_RCL, CM_RDN, CM_RUP, CM_SF, CM_SQ,
CM_SQRT, CM_STO, CM_SUB, CM_SWAP, CM_SWAPR, CM_WSIZE, CM_WSIZEQ,
CM_XEQ, CM_XOR, CM_XRND, CM_LAST };

	/* ---------- command list ---------- */

struct command {
	enum CMDS cmd;		/* command id */
	char *name;		/* command name */
	char desc[11];		/* command descriptor:
		desc[0], command suffix
			SP	none
			'B'	buffer name
			'L'	label
			'N'	number
			'P'	oPerator or register
			'R'	register
		desc[1], number of arguments dropped off the stack
			'0' zero, '1', one, '2' two, '3' three, '4' four
		desc[2345], argument descriptors
			SP	no argument for this entry
			'*'	any type
			'B'	coerce to binary
			'R'	coerce to real
		desc[6], number of results pushed to stack
			'0' zero, '1', one, '2' two, '3' three, '4' four
		desc[7], last X usage
			SP	not affected
			'L'	last x register is updated
		desc[8], stack lift
			SP	disabled
			'E'	enabled
		desc[9], trace info
			SP	nothing special
			R	register
			S	summation
		desc[10] NUL */

	char *help;		/* help string */
	};

#define NUMCMDS		(sizeof(commands) / sizeof(commands[0]))

	/* base commands */
static struct command commands[] = {
{ CM_NUM,	"",	" 0    1 E ",	"enter a number" },
{ CM_PCT,	"%",	" 2RR  2LE ",	"percent" },
{ CM_PCTCH,	"%CH",	" 2RR  1LE ",	"percent change" },
{ CM_PCTTOT,	"%TOT",	" 2RR  2LE ",	"percent of total" },
{ CM_MUL,	"*",	" 2**  1LE ",	"multiply" },
{ CM_ADD,	"+",	" 2**  1LE ",	"add" },
{ CM_SUB,	"-",	" 2**  1LE ",	"subtract" },
{ CM_DIV,	"/",	" 2RR  1LE ",	"divide" },
{ CM_INV,	"1/X",	" 1R   1LE ",	"inverse (use INV)" },
{ CM_RCL,	"<",	"P0    1 ER",	"recall" },
{ CM_STO,	">",	"P1*   1 ER",	"store" },
{ CM_AND,	"AND",	" 2BB  1LE ",	"bitwise and" },
{ CM_B,		"B",	" 0    0 E ",	"set binary mode" },
{ CM_CF,	"CF",	"N0    0 E ",	"clear flag" },
{ CM_CLRG,	"CLRG",	" 0    0 E ",	"clear registers" },
{ CM_CLST,	"CLST",	" 0    0 E ",	"clear stack" },
{ CM_CLX,	"CLX",	" 0    0 D ",	"clear x" },
{ CM_D,		"D",	" 0    0 E ",	"set decimal mode" },
{ CM_DEFAULT,	"DEFAULT"," 0    0 E ",	"restore default settings" },
{ CM_DIGSEPOFF,	"DIGSEPOFF"," 0    0 E ","set digit sep to not display" },
{ CM_DIGSEPON,	"DIGSEPON"," 0    0 E ","set digit sep to display" },
{ CM_ENTER,	"ENTER^"," 1*   2 D ",	"enter" },
{ CM_FACT,	"FACT",	" 1R   1LE ",	"factorial" },
{ CM_H,		"H",	" 0    0 E ",	"set hexadecimal mode" },
{ CM_HELP,	"HELP",	" 0    0 E ",	"help" },
{ CM_INV,	"INV",	" 1R   1LE ",	"inverse" },
{ CM_LASTX,	"L",	" 0    1 E ",	"recall last x" },
{ CM_LASTX,	"LASTX"," 0    1 E ",	"recall last x" },
{ CM_MEMVIEW,	"MEMVIEW"," 0    0 E ",	"view calculator memory" },
{ CM_MOD,	"MOD",	" 2RR  1LE ",	"modulus" },
{ CM_NEG,	"NEG",	" 1R   1LE ",	"negate" },
{ CM_NOT,	"NOT",	" 1B   1LE ",	"bitwise not" },
{ CM_NULL,	"NULL",	" 0    0 E ",	"no op" },
{ CM_O,		"O",	" 0    0 E ",	"set octal mode" },
{ CM_OR,	"OR",	" 2BB  1LE ",	"bitwise or" },
{ CM_RDN,	"R",	" 0    0 E ",	"roll down" },
{ CM_RADIXC,	"RADIX,"," 0    0 E ",	"set radix mark to ," },
{ CM_RADIXD,	"RADIX."," 0    0 E ",	"set radix mark to ." },
{ CM_RCL,	"RCL",	"P0    1 ER",	"recall" },
{ CM_RDN,	"RDN",	" 0    0 E ",	"roll down" },
{ CM_RUP,	"R^",	" 0    0 E ",	"roll up" },
{ CM_SWAP,	"S",	" 2**  2 E ",	"swap: x<>y" },
{ CM_SF,	"SF",	"N0    0 E ",	"set flag" },
{ CM_SQRT,	"SQRT",	" 1R   1LE ",	"square root" },
{ CM_STO,	"STO",	"P1*   1 ER",	"store" },
{ CM_STO,	"ST",	"P1*   1 ER",	"store" },
{ CM_ENTER,	"T",	" 1*   2 D ",	"enter" },
{ CM_WSIZE,	"WSIZE"," 1B   0 E ",	"set word size" },
{ CM_WSIZEQ,	"WSIZE?"," 0    1 E ",	"get word size" },
{ CM_SWAPR,	"X<>",	"R1*   1 ER",	"swap with" },
{ CM_XEQ,	"XEQ",	"B0    0 E ",	"execute a buffer" },
{ CM_XOR,	"XOR",	" 2BB  1LE ",	"bitwise xor" },
{ CM_XRND,	"XRND",	" 2BR  1LE ",	"round Y to X decimal places" },
{ CM_SQ,	"X^2",	" 1*   1LE ",	"square" },
{ CM_PCTCH,	"\\GD%"," 2RR  1LE ",	"delta %" } };

	/* ---------- flags ---------- */

/* The flag array (part of memory) assumes 16 flags/int.  That way,
the indices in this table don't have to be recomputed for different
word sizes. */

enum FLGS { FL_RADIX, FL_DIGGRP, FL_WSIZE, FL_BINMODE, FL_LAST };


struct flag {
	enum FLGS flg;		/* flag id */
	int start;		/* starting flag # */
	int bits;		/* number of bits */
	int index;		/* flag array index */
	int shift;		/* flag array element shift */
	int mask;		/* flag array element mask */
	char *desc;		/* description */
	};

#define NUMFLAGS	7	/* number of ints used to hold flags */
#define MAXFLAG		100	/* largest flag supported */

struct flag flags[] = {		/* must be in enum FLGS order */
	/* user flags */
{ FL_RADIX,	28, 1, 1, 12, 0x1, "radix mark: 0). 1)," },
{ FL_DIGGRP,	29, 1, 1, 13, 0x1, "digit groupings shown: 0)no 1)yes" },
	/* Freyja-local flags */
{ FL_WSIZE,	65, 8, 4,  0,0xFF, "binary integer word size" },
{ FL_BINMODE,	73, 2, 4,  8, 0x3, "binary numbers 0)dec 1)oct 2)bin 3)hex" } };
#define FLV_DEC		0x00
#define FLV_OCT		0x01
#define FLV_BIN		0x02
#define FLV_HEX		0x03

	/* ---------- number format ---------- */
struct number {
	union	{
		int b;		/* binary integer goes here */
		double r;	/* real number goes here */
		} n;
	char type;		/* 'B' binary, 'R' real */
	};

	/* ---------- memory ---------- */

#define NUMSTACK	5
#define NUMREGS		(REGCOUNT + NUMSTACK)
#define X		(NUMREGS)
#define Y		(NUMREGS + 1)
#define Z		(NUMREGS + 2)
#define T		(NUMREGS + 3)
#define L		(NUMREGS + 4)

#define NUMSUM		6
#define SUMX		0
#define SUMX2		1
#define SUMY		2
#define SUMY2		3
#define SUMXY		4
#define SUMN		5

struct memory {
	struct number r[NUMREGS + NUMSTACK];
	int flags[NUMFLAGS];	/* the flags */
	FLAG stack_lift;
	};

	/* ---------- variables ---------- */

static struct memory m;			/* memory */
static char input[WORKSIZE + 1];	/* buffer for holding input */
static char fmt_buf[BINSIZE + 2];	/* buffer for U_Fmt */
static int bin_mask;			/* word mask for binary operations */
static char doexit = NUL;		/* do we exit? NUL = no, N = yes,
					don't insert #, Y = yes, insert # */
static enum CMDS pushedcmd = CM_NULL;	/* pushed command: execute first */
static struct command *cmdptr;		/* current command */
static int cmdnum;			/* numeric argument for command */
static FLAG cmdind;			/* was it an indirect? */
static struct number *cmdreg;		/* register pointer */
static enum CMDS cmdcmd;		/* command for sto/rcl */
static char *cmdrest;			/* rest of command string */

	/* Arguments for commands.  Values have been coerced. */
static struct number x;
static struct number y;
static struct number z;
static struct number t;

void U_Cmd(void);
void U_CmdSetup(char arg, struct number *from, struct number *to);
int U_Dispatch(enum CMDS cmd);
struct command *U_FindCmd(enum CMDS cmd);
int U_FlGet(enum FLGS f);
void U_FlSet(enum FLGS f, int value);
char *U_Fmt(struct number *nptr);
char *U_FmtBin(char *buf, int value, FLAG first);
FLAG U_In(void);
void U_Status(void);
void U_ToBin(struct number *nptr);
void U_ToReal(struct number *nptr);
void U_View(void);

/* ------------------------------------------------------------ */

/* Initialize to default values. */

void
UInit(void)
	{
	int cnt;

	for (cnt = 0; cnt < NUMREGS; cnt++) {
		m.r[cnt].type = 'R';
		m.r[cnt].n.r = 0.0;
		}
	for (cnt = 0; cnt < NUMFLAGS; cnt++) {
		m.flags[cnt] = 0;
		}
	U_FlSet(FL_RADIX, (Res_Char(RES_CONF, RES_RADIX) == '.') ? 1 : 0);
	U_FlSet(FL_DIGGRP, 1);
	U_FlSet(FL_WSIZE, BINSIZE);
	bin_mask = ~0;
	U_FlSet(FL_BINMODE, FLV_HEX);
	m.stack_lift = TRUE;
	}


/* ------------------------------------------------------------ */

/* Calculator */

void
UCalc(void)
	{
	uarg = 0;

	for (doexit = NUL; doexit == NUL; ) {
		U_In();
		if (cmdptr != NULL) U_Cmd();
		}
	if (doexit == 'Y') BInsStr(U_Fmt(&m.r[X]));
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Return the operation's description. */

char *
UDescr(int op)
	{
	return(commands[op].name);
	}


/* ------------------------------------------------------------ */

/* Enter the current number into the calculator. */

void
UEnter(void)
	{
	char buf[WORKSIZE + 1];
	FLAG isafter;
	FLAG isfirst = TRUE;
	char *cptr = buf;
	KEYCODE chr;

	WNumMark();
	if (isafter = BIsAfterMark(mark)) BMarkSwap(mark);
	BMarkToPoint(cwin->point);
	while (cptr < &buf[sizeof(buf) - 2] && BIsBeforeMark(mark)) {
		chr = BGetCharAdv();
		if (isfirst) {
			if (chr == '-') *cptr++ = '0';
			isfirst = FALSE;
			}			
		if (chr == '-') chr = '~';
		*cptr++ = chr;
		}
	*cptr++ = SP;
	BPointToMark(cwin->point);
	if (isafter) BMarkSwap(mark);
	uarg = 0;

	KFromStr(buf, cptr - buf);
	UCalc();
	}


/* ------------------------------------------------------------ */

/* Return the operation's help text. */

char *
UHelp(int op)
	{
	return(commands[op].help);
	}


/* ------------------------------------------------------------ */

/* Return the number of operators. */

int
UNumOps(void)
	{
	return(NUMCMDS);
	}


/* ------------------------------------------------------------ */

/* Insert a copy of the X register. */

void
UPrintX(void)
	{
	uarg = 0;

	if (isuarg) {
		WNumMark();
		RRegDelete();
		}
	BInsStr(U_Fmt(&m.r[X]));
	}


/* ------------------------------------------------------------ */

/* Execute the current command. */

void
U_Cmd(void)
	{
	struct number tmp;
	int retval;
	int amt;

/* set up args */

	U_CmdSetup(cmdptr->desc[2], &m.r[X], &x);
	U_CmdSetup(cmdptr->desc[3], &m.r[Y], &y);
	U_CmdSetup(cmdptr->desc[4], &m.r[Z], &z);
	U_CmdSetup(cmdptr->desc[5], &m.r[T], &t);

/* handle indirect registers */

	if (cmdptr->desc[0] == 'R' || cmdptr->desc[0] == 'P') {
		if (cmdind) {
			tmp = *cmdreg;
			U_ToBin(&tmp);
			if (tmp.n.b < 0 || tmp.n.b >= REGCOUNT) {
				DError(RES_UNKINDREG);
				cmdptr = NULL;
				return;
				}
			cmdreg = &m.r[tmp.n.b];
			}
		}

/* execute */

	retval = U_Dispatch(cmdptr->cmd);
	if (retval < 0) {

/* lastx */

		if (cmdptr->desc[7] == 'L') m.r[L] = m.r[X];

/* drop stack */

		if (cmdptr->desc[1] == '0') {
			}
		else if (cmdptr->desc[1] == '1') {
			m.r[X] = m.r[Y];
			m.r[Y] = m.r[Z];
			m.r[Z] = m.r[T];
			}
		else if (cmdptr->desc[1] == '2') {
			m.r[X] = m.r[Z];
			m.r[Y] = m.r[T];
			m.r[Z] = m.r[T];
			}
		else	{
			m.r[X] = m.r[T];
			m.r[Y] = m.r[T];
			m.r[Z] = m.r[T];
			}

/* save results */

		amt = cmdptr->desc[6] - '0';

/* stack lift disabled and new operation lifts the stack */
		if (!m.stack_lift && cmdptr->desc[1] < cmdptr->desc[6]) {
			amt = cmdptr->desc[6] - cmdptr->desc[1] - 1;
			if (amt == 0) m.r[X] = x;
			}

		if (amt <= 0) {
			}
		else if (amt == 1) {
			m.r[T] = m.r[Z];
			m.r[Z] = m.r[Y];
			m.r[Y] = m.r[X];
			m.r[X] = x;
			}
		else if (amt == 2) {
			m.r[T] = m.r[Y];
			m.r[Z] = m.r[X];
			m.r[Y] = y;
			m.r[X] = x;
			}
		else if (amt == 3) {
			m.r[T] = m.r[X];
			m.r[Z] = z;
			m.r[Y] = y;
			m.r[X] = x;
			}
		else	{
			m.r[T] = t;
			m.r[Z] = z;
			m.r[Y] = y;
			m.r[X] = x;
			}

/* stack lift */

		m.stack_lift = cmdptr->desc[8] == 'E';
		}
	else	DError(retval);
	}


/* ------------------------------------------------------------ */

/* Setup the register according to the argument type. */

void
U_CmdSetup(char arg, struct number *from, struct number *to)
	{
	if (arg != SP) {
		*to = *from;
		if (arg == 'B') U_ToBin(to);
		else if (arg == 'R') U_ToReal(to);
		}
	}


/* ------------------------------------------------------------ */

/* The command dispatch table.  Return -1 on success or a number of an
error message if a failure. */

int
U_Dispatch(enum CMDS cmd)
	{
	struct number tmp;
	struct buffer *bptr;
	int cnt;
	int num;
	char buf[WORKSIZE];

	switch (cmd) {

	case CM_ADD:
		if (x.type == 'B' && y.type == 'B') {
			x.n.b += y.n.b;
			x.n.b &= bin_mask;
			}
		else	{
			U_ToReal(&x);
			U_ToReal(&y);
			x.type = 'R';
			x.n.r += y.n.r;
			}
		break;

	case CM_AND:
		x.n.b &= y.n.b;
		x.n.b &= bin_mask;
		break;

 	case CM_B:
		U_FlSet(FL_BINMODE, FLV_BIN);
		break;

	case CM_CF:
		if (cmdnum < 1 || cmdnum > MAXFLAG) return(RES_CALCFLAG);
		num = cmdnum - 1;
		m.flags[num >> 4] &= ~(1 << (num & 0xF));
		break;

 	case CM_CLRG:
		for (cnt = 0; cnt < REGCOUNT; cnt++) {
			m.r[cnt].type = 'R';
			m.r[cnt].n.r = 0.0;
			}
		break;

 	case CM_CLST:
		for (cnt = X; cnt < L; cnt++) {
			m.r[cnt].type = 'R';
			m.r[cnt].n.r = 0.0;
			}
		break;

 	case CM_CLX:
		m.r[X].type = 'R';
		m.r[X].n.r = 0.0;
		break;

 	case CM_D:
		U_FlSet(FL_BINMODE, FLV_DEC);
		break;

	case CM_DEFAULT:
		UInit();
		break;

	case CM_DIGSEPOFF:
		U_FlSet(FL_DIGGRP, 0);
		break;

	case CM_DIGSEPON:
		U_FlSet(FL_DIGGRP, 1);
		break;

 	case CM_DIV:
		if (x.n.r == 0.0) return(RES_CALCDIVZERO);
		x.n.r = y.n.r / x.n.r;
		break;

 	case CM_ENTER:
		y = x;
		m.stack_lift = TRUE;
		break;

 	case CM_FACT:
		if (x.n.r < 0.0) return(RES_CALCNEG);
		for (cnt = x.n.r, x.n.r = 1.0; cnt > 1; cnt--) {
			x.n.r *= cnt;
			}
		break;

 	case CM_H:
		U_FlSet(FL_BINMODE, FLV_HEX);
		break;

 	case CM_HELP:
		MMenuH();
		DIncrDisplay();
		break;

	case CM_INV:
		if (x.n.r == 0.0) return(RES_CALCDIVZERO);
		x.n.r = 1.0 / x.n.r;
		break;

 	case CM_LASTX:
		x = m.r[L];
		break;

	case CM_MEMVIEW:
		U_View();
		break;

 	case CM_MOD:
		if (x.n.r == 0.0) return(RES_CALCMODZERO);
		x.n.r = y.n.r - x.n.r * floor(y.n.r / x.n.r);
		break;

 	case CM_MUL:
		if (x.type == 'B' && y.type == 'B') {
			x.n.b *= y.n.b;
			x.n.b &= bin_mask;
			}
		else	{
			U_ToReal(&x);
			U_ToReal(&y);
			x.type = 'R';
			x.n.r *= y.n.r;
			}
		break;

	case CM_NEG:
		x.n.r = -x.n.r;
		break;

 	case CM_NOT:
		x.n.b = ~x.n.b;
		x.n.b &= bin_mask;
		break;

 	case CM_NULL:
		break;

	case CM_NUM:
		break;

 	case CM_O:
		U_FlSet(FL_BINMODE, FLV_OCT);
		break;

 	case CM_OR:
		x.n.b |= y.n.b;
		x.n.b &= bin_mask;
		break;

 	case CM_PCT:
		x.n.r *= y.n.r / 100.0;
		break;

 	case CM_PCTCH:
		if (y.n.r == 0.0) return(RES_CALCPCTZERO);
		x.n.r = 100.0 * (x.n.r - y.n.r) / y.n.r;
		break;

	case CM_PCTTOT:
		if (y.n.r == 0.0) return(RES_CALCPCTZERO);
		x.n.r = 100.0 * x.n.r / y.n.r;
		break;

	case CM_RADIXC:
		U_FlSet(FL_RADIX, 0);
		break;

	case CM_RADIXD:
		U_FlSet(FL_RADIX, 1);
		break;

 	case CM_RCL:
		if (cmdnum < 0) {
			for (cnt = 0; KIsKey() == 'N'; cnt++) {
				xsprintf(buf, "   %d   ", cnt);
				DEcho(buf);
				}
			KGetChar();
			return(NULL);
			}
		y = x;
		x = *cmdreg;
		U_Dispatch(cmdcmd);
		break;

 	case CM_RDN:
		tmp = m.r[X];
		m.r[X] = m.r[Y];
		m.r[Y] = m.r[Z];
		m.r[Z] = m.r[T];
		m.r[T] = tmp;
		break;

 	case CM_RUP:
		tmp = m.r[X];
		m.r[X] = m.r[T];
		m.r[T] = m.r[Z];
		m.r[Z] = m.r[Y];
		m.r[Y] = tmp;
		break;

	case CM_SF:
		if (cmdnum < 1 || cmdnum > MAXFLAG) return(RES_CALCFLAG);
		num = cmdnum - 1;
		m.flags[num >> 4] |= 1 << (num & 0xF);
		break;

 	case CM_SQ:
		x.n.r *= x.n.r;
		break;

 	case CM_SQRT:
		if (x.n.r < 0.0) return(RES_CALCNEG);
		x.n.r = sqrt(x.n.r);
		break;

 	case CM_STO:
		y = *cmdreg;
		U_Dispatch(cmdcmd);
		*cmdreg = x;
		break;

	case CM_SUB:
		if (x.type == 'B' && y.type == 'B') {
			x.n.b = y.n.b - x.n.b;
			x.n.b &= bin_mask;
			}
		else	{
			U_ToReal(&x);
			U_ToReal(&y);
			x.type = 'R';
			x.n.r = y.n.r - x.n.r;
			}
		break;

 	case CM_SWAP:
		tmp = y;
		y = x;
		x = tmp;
		break;

 	case CM_SWAPR:
		tmp = x;
		x = *cmdreg;
		*cmdreg = tmp;
		break;

 	case CM_WSIZE:
		if (x.n.b < 0 || x.n.b > BINSIZE) return(RES_CALCRANGE);
		U_FlSet(FL_WSIZE, (int)x.n.b);
		if (x.n.b == BINSIZE)
			bin_mask = ~0;
		else	bin_mask = (1 << x.n.b) - 1;
		break;

 	case CM_WSIZEQ:
		x.type = 'B';
		x.n.b = U_FlGet(FL_WSIZE);
		break;

	case CM_XEQ:
		bptr = BBufFind(cmdrest);
		if (bptr == NULL) return(RES_CALCUNKPROG);
		KFromBuf(bptr);
		break;

 	case CM_XOR:
		x.n.b ^= y.n.b;
		x.n.b &= bin_mask;
		break;

 	case CM_XRND:
		num = x.n.b;
		for (cnt = 0; cnt < num; cnt++) y.n.r *= 10.0;
		y.n.r += (y.n.r > 0) ? 0.5 : -0.5;
		tmp.n.b = y.n.r;
		x.n.r = tmp.n.b;
		for (cnt = 0; cnt < num; cnt++) x.n.r /= 10.0;
		x.type = 'R';
		break;
		}
	return(-1);
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the command structure for the specified
command. */

struct command *
U_FindCmd(enum CMDS cmd)
	{
	struct command *cptr;

	for (cptr = commands; cptr < &commands[NUMCMDS]; cptr++) {
		if (cmd == cptr->cmd) return(cptr);
		}
	return(NULL);
	}


/* ------------------------------------------------------------ */

/* Return the value of the specified flag. */

int
U_FlGet(enum FLGS f)
	{
	struct flag *fptr = &flags[(int)f];

	return((m.flags[fptr->index] >> fptr->shift) & fptr->mask);
	}


/* ------------------------------------------------------------ */

/* Set the specified flag to the supplied value. */

void
U_FlSet(enum FLGS f, int v)
	{
	struct flag *fptr = &flags[(int)f];

	v = (v & fptr->mask) << fptr->shift;
	m.flags[fptr->index] &= ~(fptr->mask << fptr->shift);
	m.flags[fptr->index] |= v;
	}


/* ------------------------------------------------------------ */

/* Return a pointer to a static buffer that contains a formatted
version of the number. */

char *
U_Fmt(struct number *nptr)
	{
	int mode = U_FlGet(FL_BINMODE);
	int cnt;
	FLAG iscomma = U_FlGet(FL_RADIX) == 0;
	char *cptr;
	char *cptr2;

	if (nptr->type == 'B') {
		switch (mode) {

		case FLV_BIN:
			*fmt_buf = '#';
			U_FmtBin(&fmt_buf[1], nptr->n.b, TRUE);
			strcat(fmt_buf, "b");
			break;

		case FLV_OCT:
			xsprintf(fmt_buf, "#%oo", nptr->n.b);
			break;

		case FLV_DEC:
			xsprintf(fmt_buf, "#%ud", nptr->n.b);
			break;

		case FLV_HEX:
			xsprintf(fmt_buf, "#%xh", nptr->n.b);
			break;
			}
		}
	else	{
		sprintf(fmt_buf, "%.9lg", nptr->n.r);
			/* handle comma */
		if (iscomma) {
			for (cptr = fmt_buf; *cptr != NUL; cptr++) {
				if (*cptr == '.') *cptr = ',';
				}
			}
			/* handle digit separator */
		if (U_FlGet(FL_DIGGRP) == 1) {
			cptr2 = fmt_buf;
			if (*cptr2 == '-') cptr2++;

			cnt = 0;
			for (cptr = cptr2; xisdigit(*cptr); cptr++, cnt++) ;
			while (cnt > 3) {
				cptr -= 3;
				memmove(cptr + 1, cptr, strlen(cptr) + 1);
				*cptr = iscomma ? '.' : ',';
				cnt -= 3;
				}
			}
		}
	return(fmt_buf);
	}


/* ------------------------------------------------------------ */

/* Print out a binary integer. */

char *
U_FmtBin(char *buf, int value, FLAG first)
	{
	if (value >= 2) buf = U_FmtBin(buf, value / 2, FALSE);
	if (value == 0 && first)
		*buf++ = '0';
	else	*buf++ = (value % 2) + '0';
	*buf = NUL;
	return(buf);
	}


/* ------------------------------------------------------------ */

/* Input the next command. */

FLAG
U_In(void)
	{
	KEYCODE chr;
	int amt;
	int cnt;
	int base;
	char buf[BIGBUFFSIZE];
	char *cptr;
	char *cptr2;
	FLAG isdone = FALSE;
	FLAG iscomma;

	cmdptr = NULL;
	*input = NUL;
	if (pushedcmd != CM_NULL) {
		cmdptr = U_FindCmd(pushedcmd);
		pushedcmd = CM_NULL;
		return;
		}
	chr = KEYREGEN;
	while (!isdone) {
		amt = strlen(input);

		U_Status();
		chr = KGetChar();
		if (chr == KEYQUIT) {
			MExit();
			doexit = 'N';
			return;
			}
		else if (chr < 0) {
			continue;
			}

		if (chr >= 256) {	/* function key */
			chr -= 256;
			switch (chr) {

			case 59:	/* F1 */
				pushedcmd = CM_HELP;
				isdone = TRUE;
				break;

			case 67:	/* F9 */
			case 38:	/* Alt-L */
				chr = '-';
				goto chs;
				/*break;*/

			case 93:	/* Shift-F10 */
				doexit = 'N';
				MExit();
				return;
				/*break;*/

			case 48:	/* Alt-B */
				pushedcmd = CM_LASTX;
				isdone = TRUE;
				break;

			case 46:	/* Alt-C */
				pushedcmd = CM_SWAP;
				isdone = TRUE;
				break;

			case 50:	/* Alt-M */
				xstrcpy(input, "RCL");
				amt = strlen(input);
				break;

			case 49:	/* Alt-N */
				xstrcpy(input, "STO");
				amt = strlen(input);
				break;

			case 47:	/* Alt-V */
				pushedcmd = CM_RDN;
				isdone = TRUE;
				break;

			case 45:	/* Alt-X */
				pushedcmd = CM_INV;
				isdone = TRUE;
				break;

			case 44:	/* Alt-Z */
				pushedcmd = CM_SQRT;
				isdone = TRUE;
				break;

			default:
				TBell();
				return;
				/*break;*/
				}
			continue;
			}

		if (!isdone) {
			switch (chr) {

			case KEYQUIT:
			case KEYABORT:
			case ESC:
			case BEL:
			case '$':
				doexit = 'N';
				isdone = TRUE;
				break;

			case LF:
				doexit = 'Y';
				isdone = TRUE;
				break;

			case BS:
			case DEL:
				if (*input != NUL) input[amt - 1] = NUL;
				else	{
					cmdptr = U_FindCmd(CM_CLX);
					isdone = TRUE;
					}
				break;

			case SP:
			case CR:
				isdone = TRUE;
				break;

			case '%':
				if (*input == '\'') goto accumulate;
				pushedcmd = CM_PCT;
				isdone = TRUE;
				break;

			case '*':
				if (*input == '\'') goto accumulate;
				pushedcmd = CM_MUL;
				isdone = TRUE;
				break;

			case '+':
				if (*input == '\'') goto accumulate;
				pushedcmd = CM_ADD;
				isdone = TRUE;
				break;

			case '-':
				if (*input == '\'') goto accumulate;
				pushedcmd = CM_SUB;
				isdone = TRUE;
				break;

			case '/':
				if (*input == '\'') goto accumulate;
				pushedcmd = CM_DIV;
				isdone = TRUE;
				break;

			case '\'':
				if (*input == '\'') goto accumulate;
				if (amt >= sizeof(input) - 1) {
					input[amt - 1] = NUL;
					TBell();
					}
				memmove(input + 1, input, amt + 1);
				*input = '\'';
				break;

			case '`':
			case '~':
chs:
				if (*input == '\'') goto accumulate;
				if (*input == NUL) {
					cmdptr = U_FindCmd(CM_NEG);
					isdone = TRUE;
					}
				else	{
					cptr = input + amt;
					while (cptr > input &&
						xtoupper(*cptr) != 'E' &&
						*cptr != '-') cptr--;
					if (*cptr == '-') {
						xstrcpy(cptr, cptr + 1);
						}
					else	{
						if (amt >= sizeof(input) - 1) {
							input[amt - 1] = NUL;
							TBell();
							}
						if (xtoupper(*cptr) == 'E')
							cptr++;
						memmove(cptr + 1, cptr,
							strlen(cptr) + 1);
						*cptr = '-';
						}
					}
				break;

			default:
accumulate:
				if (amt >= sizeof(input) - 1) {
					input[amt - 1] = NUL;
					TBell();
					}

				input[amt] = chr;
				input[amt + 1] = NUL;
				break;
				}
			}
		}
	if (*input == NUL || cmdptr != NULL) return;
	if (*input == '\'') xstrcpy(input, input + 1);

/* check for binary numbers */

	if (*input == '#') {
		cnt = U_FlGet(FL_BINMODE);
		if (cnt == FLV_BIN) base = 2;
		else if (cnt == FLV_OCT) base = 8;
		else if (cnt == FLV_DEC) base = 10;
		else base = 16;
		if (!SToN(&input[1], &x.n.b, base)) {
			DError(RES_CALCINVBIN);
			return;
			}
		x.n.b &= bin_mask;
		x.type = 'B';
		cmdptr = U_FindCmd(CM_NUM);
		return;
		}

/* try for real number */

	/* handle radix mark, dig sep char, ~->- */
	iscomma = U_FlGet(FL_RADIX) == 0;
	for (cptr = input, cptr2 = buf; *cptr != NUL; cptr++) {
		chr = *cptr;
		if (chr == '~' || chr == '`') chr = '-';
		else if (iscomma) {
			if (chr == ',') chr = '.';
			else if (chr == '.') continue;
			}
		else	{
			if (chr == ',') continue;
			}
		*cptr2++ = chr;
		}
	*cptr2 = NUL;

	/* handle numbers that start with EEX */
	cptr = buf;
	if (*cptr == '-') cptr++;
	if (xtoupper(*cptr) == 'E') {
		memmove(cptr + 1, cptr, strlen(cptr) + 1);
		*cptr = '1';
		}

/* check for real number */
	if (sscanf(buf, "%lf", &x.n.r) == 1) {
		x.type = 'R';
		cmdptr = U_FindCmd(CM_NUM);
		return;
		}

/* else command */
	for (cmdptr = commands; cmdptr < &commands[NUMCMDS]; cmdptr++) {
		if (cmdptr->desc[0] == SP) {
			if (strequ(input, cmdptr->name)) break;
			}
		else	{
			if (strnequ(input, cmdptr->name,
				strlen(cmdptr->name))) break;
			}
		}
	if (cmdptr >= &commands[NUMCMDS]) {
		DError(RES_UNKCMD);
		cmdptr = NULL;
		return;
		}
	if (cmdptr->desc[0] == SP) return;

/* process suffix */

	cptr = &input[strlen(cmdptr->name)];
	cmdrest = cptr;
	cmdcmd = CM_NULL;
	cmdind = FALSE;
	cmdnum = 0;
	cmdreg = &m.r[0];

	if (cmdptr->desc[0] == 'B') return;

/* is operator allowed? */

	if (cmdptr->desc[0] == 'P') {
		if (*cptr == '*')	{ cmdcmd = CM_MUL; cptr++; }
		else if (*cptr == '+')	{ cmdcmd = CM_ADD; cptr++; }
		else if (*cptr == '-')	{ cmdcmd = CM_SUB; cptr++; }
		else if (*cptr == '/')	{ cmdcmd = CM_DIV; cptr++; }
		}

/* is indirect allowed? */

	if (cmdptr->desc[0] == 'R' || cmdptr->desc[0] == 'P') {
		if (*cptr == '.') {
			cmdind = TRUE;
			cptr++;
			}
		else if (strnequ(cptr, "IND", 3)) {
			cmdind = TRUE;
			cptr += 3;
			}
		/* check for register name */

		if (xisalpha(*cptr)) {
			if (*(cptr + 1) != NUL) {
				DError(RES_CALCUNKREG);
				cmdptr = NULL;
				return;
				}
			*cptr = xtoupper(*cptr);
			if (*cptr == 'X') { cmdreg = &m.r[X]; cmdnum = X; }
			else if (*cptr == 'Y') {cmdreg = &m.r[Y]; cmdnum = Y; }
			else if (*cptr == 'Z') {cmdreg = &m.r[Z]; cmdnum = Z; }
			else if (*cptr == 'T') {cmdreg = &m.r[T]; cmdnum = T; }
			else if (*cptr == 'L') {cmdreg = &m.r[L]; cmdnum = L; }
			else	{
				DError(RES_CALCUNKREG);
				cmdptr = NULL;
				return;
				}
			return;
			}
		}

/* number is always allowed by the time that you get here */

	if (cmdptr->cmd == CM_RCL && *cptr == '~' || *cptr == '-') {
		cmdnum = -78;
		return;
		}
	if (!SToN(cptr, &cmdnum, 10)) {
		DError(RES_CALCUNKREG);
		cmdptr = NULL;
		return;
		}
	if (cmdnum < 0 || cmdnum >= REGCOUNT) {
		DError(RES_CALCUNKREG);
		cmdptr = NULL;
		return;
		}
	cmdreg = &m.r[cmdnum];
	}


/* ------------------------------------------------------------ */

/* Display the calculator status line */

void
U_Status(void)
	{
	char buf[4 * LINEBUFFSIZE];
	char *cptr = buf;
	int amt;

	if (KIsKey() == 'Y') return;

	xsprintf(cptr, "L)%s  ", U_Fmt(&m.r[L]));
	cptr += strlen(cptr);
	xsprintf(cptr, "T)%s  ", U_Fmt(&m.r[T]));
	cptr += strlen(cptr);
	xsprintf(cptr, "Z)%s  ", U_Fmt(&m.r[Z]));
	cptr += strlen(cptr);
	xsprintf(cptr, "Y)%s  ", U_Fmt(&m.r[Y]));
	cptr += strlen(cptr);
	xsprintf(cptr, "X)%s ", U_Fmt(&m.r[X]));
	cptr += strlen(cptr);

	if (TMaxCol() >= 80)
		amt = (TMaxCol() - WORKSIZE) - (cptr - buf);
	else	amt = (TMaxCol() - 12) - (cptr - buf);
	if (amt > 0) {
		while (amt-- > 0) *cptr++ = SP;
		*cptr = NUL;
		}
	else	{
		xstrcpy(buf, buf - amt);
		cptr = buf + strlen(buf);
		}
	*cptr++ = '>';
	*cptr = NUL;
	xstrcpy(cptr, input);
	DEcho(buf);
	TSetPoint(TMaxRow() - 1, strlen(buf));
	}


/* ------------------------------------------------------------ */

/* Convert the number to a binary number, if required. */

void
U_ToBin(struct number *nptr)
	{
	double r;

	if (nptr->type == 'R') {
		r = nptr->n.r;
		nptr->n.b = r;
		nptr->type = 'B';
		}
	}


/* ------------------------------------------------------------ */

/* Convert the number to a real number, if required. */

void
U_ToReal(struct number *nptr)
	{
	int b;

	if (nptr->type == 'B') {
		b = nptr->n.b;
		nptr->n.r = b;
		nptr->type = 'R';
		}
	}


/* ------------------------------------------------------------ */

/* View interpreted calculator memory. */

void
U_View(void)
	{
	char buf[LINEBUFFSIZE];
	struct flag *fptr;
	int cnt;
	FLAG wastext = xprintf_get_text();

	if (!FMakeSys(SYS_CALC, TRUE)) return;
	xprintf_set_text(FALSE);

	BInsStr("Stack:\n");

	xsprintf(buf, "X\t%s\n", U_Fmt(&m.r[X]));
	BInsStr(buf);
	xsprintf(buf, "Y\t%s\n", U_Fmt(&m.r[Y]));
	BInsStr(buf);
	xsprintf(buf, "Z\t%s\n", U_Fmt(&m.r[Z]));
	BInsStr(buf);
	xsprintf(buf, "T\t%s\n", U_Fmt(&m.r[T]));
	BInsStr(buf);
	xsprintf(buf, "L\t%s\n", U_Fmt(&m.r[L]));
	BInsStr(buf);

	BInsStr(
"\nFlags (start/bits, value, description (NS = \"not supported\"):\n");

	for (fptr = flags; fptr < &flags[(int) FL_LAST]; fptr++) {
		xsprintf(buf, "%d/%d\t%d\t%s\n",
			fptr->start,
			fptr->bits,
			U_FlGet(fptr - flags),
			fptr->desc);
		BInsStr(buf);
		}

	BInsStr("\nFlags (values):\n");

	for (cnt = 0; cnt < NUMFLAGS; cnt++) {
		xsprintf(buf, "%3d - %3d    %04x\n",
			cnt * 16 + 16,
			cnt * 16 + 1,
			m.flags[cnt]);
		BInsStr(buf);
		}

	BInsStr("\nRegisters:\n");

	for (cnt = 0; cnt < REGCOUNT; cnt++) {
		xsprintf(buf, "R%02d\t%s\n", cnt, U_Fmt(&m.r[cnt]));
		BInsStr(buf);
		}

	xprintf_set_text(wastext);
	BMoveToStart();
	DIncrDisplay();
	}


/* end of FCALC.C -- RPN Calculator */
