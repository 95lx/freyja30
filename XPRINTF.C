/* XPRINTF.C -- Printf with Extensions/Deletions

	Written February 1988 by Craig Finseth

THE NON-VARARGS VERSION ASSUMES THAT sizeof(pointer) >= sizeof(int)
*/

#include "lib.h"

#if defined(VARARGS)
enum XP_STATE { XP_ARG, XP_CASE, XP_COMMA, XP_C, XP_D, XP_H, XP_L,
	XP_O, XP_R, XP_S, XP_U, XP_X, XP_DONE };
enum XP_TYPE { XP_CPT, XP_IT, XP_LT, XP_RT, XP_DONET };
#endif

void x_printput();		/* const char c */
void x_printrput();		/* const char c */

static void (*put)() = x_printput;
static void (*rput)() = x_printrput;

static char printbuf[BUFFSIZE / 2];
static char *printptr;
static char *strptr;

static int outfd = 1;		/* output file descriptor */

static FLAG istext = TRUE;

#if defined(VARARGS)
static FLAG iszero;
static FLAG isarg;
static unsigned arg;
static char *format;
static enum XP_STATE state;

enum XP_TYPE x_chunk();		/* void */
void x_chunkcp();		/* char *val */
void x_chunki();		/* int val */
void x_chunkl();		/* long val */
void x_chunkr();		/* char *val1, int *val2 */
#endif

void x_printf2();		/* const char *fmt, int *parms */
void x_printnum();		/* unsigned value, const int base,
				const int len, const FLAG notfirst,
				const FLAG iszero */
void x_printlong();		/* long value, const int base,
				const int len, const FLAG notfirst,
				const FLAG iszero */
void x_printcomma();		/* unsigned value, const int len,
				const int depth */
void x_printsput();		/* const char c */

/* ------------------------------------------------------------ */

/* Set text mode to IST. */

void
xprintf_set_text(ist)
	FLAG ist;
	{
	istext = ist;
	}


/* ------------------------------------------------------------ */

/* Return whether text mode is set. */

FLAG
xprintf_get_text()
	{
	return(istext);
	}


/* ------------------------------------------------------------ */

void
#if defined(VARARGS)
#if defined(__STDC__)
xprintf(char *fmt, ...)
#else
xprintf(va_alist)
	va_dcl
#endif
	{
	va_list ap;
	char *cp;
	int *ip;
	long l;
	int i;

#if defined(__STDC__)
	va_start(ap, fmt);
	format = fmt;
#else
	va_start(ap);
	format = va_arg(ap, char *);
#endif
	printptr = printbuf;
	state = XP_DONE;
	do	{
		switch (x_chunk()) {

		case XP_CPT:
			cp = va_arg(ap, char *);
			x_chunkcp(cp);
			break;

		case XP_IT:
			i = va_arg(ap, int);
			x_chunki(i);
			break;

		case XP_LT:
			l = va_arg(ap, long);
			x_chunkl(l);
			break;

		case XP_RT:
			cp = va_arg(ap, char *);
			ip = va_arg(ap, int *);
			x_chunkr(cp, ip);
			break;
			}
		} while (state != XP_DONE);
	va_end(ap);
#else
xprintf(format, args)
	char *format;
	int args;
	{
	printptr = printbuf;
	x_printf2(format, &args);
#endif
	write(1, printbuf, printptr - printbuf);
	}


/* ------------------------------------------------------------ */

void
#if defined(VARARGS)
#if defined(__STDC__)
xdprintf(int fd, char *fmt, ...)
	{
#else
xdprintf(va_alist)
	va_dcl
	{
	int fd;
#endif
	va_list ap;
	char *cp;
	int *ip;
	long l;
	int i;

#if defined(__STDC__)
	va_start(ap, fmt);
	format = fmt;
#else
	va_start(ap);
	fd = va_arg(ap, int);
	format = va_arg(ap, char *);
#endif
	outfd = fd;
	printptr = printbuf;
	state = XP_DONE;
	do	{
		switch (x_chunk()) {

		case XP_CPT:
			cp = va_arg(ap, char *);
			x_chunkcp(cp);
			break;

		case XP_IT:
			i = va_arg(ap, int);
			x_chunki(i);
			break;

		case XP_LT:
			l = va_arg(ap, long);
			x_chunkl(l);
			break;

		case XP_RT:
			cp = va_arg(ap, char *);
			ip = va_arg(ap, int *);
			x_chunkr(cp, ip);
			break;
			}
		} while (state != XP_DONE);
	va_end(ap);
#else
xdprintf(fd, format, args)
	int fd;
	char *format;
	int args;
	{
	outfd = fd;
	printptr = printbuf;
	x_printf2(format, &args);
#endif
	write(fd, printbuf, printptr - printbuf);
	outfd = 1;
	}


/* ------------------------------------------------------------ */

void
#if defined(VARARGS)
#if defined(__STDC__)
xeprintf(char *fmt, ...)
#else
xeprintf(va_alist)
	va_dcl
#endif
	{
	va_list ap;
	char *cp;
	int *ip;
	long l;
	int i;

#if defined(__STDC__)
	va_start(ap, fmt);
	format = fmt;
#else
	va_start(ap);
	format = va_arg(ap, char *);
#endif
	outfd = 2;
	printptr = printbuf;
	state = XP_DONE;
	do	{
		switch (x_chunk()) {

		case XP_CPT:
			cp = va_arg(ap, char *);
			x_chunkcp(cp);
			break;

		case XP_IT:
			i = va_arg(ap, int);
			x_chunki(i);
			break;

		case XP_LT:
			l = va_arg(ap, long);
			x_chunkl(l);
			break;

		case XP_RT:
			cp = va_arg(ap, char *);
			ip = va_arg(ap, int *);
			x_chunkr(cp, ip);
			break;
			}
		} while (state != XP_DONE);
	va_end(ap);
#else
xeprintf(format, args)
	char *format;
	int args;
	{
	outfd = 2;
	printptr = printbuf;
	x_printf2(format, &args);
#endif
	write(2, printbuf, printptr - printbuf);
	outfd = 1;
	}


/* ------------------------------------------------------------ */

void
#if defined(VARARGS)
#if defined(__STDC__)
xsprintf(char *string, char *fmt, ...)
	{
#else
xsprintf(va_alist)
	va_dcl
	{
	char *string;
#endif
	va_list ap;
	char *cp;
	int *ip;
	long l;
	int i;

#if defined(__STDC__)
	va_start(ap, fmt);
	format = fmt;
#else
	va_start(ap);
	string = va_arg(ap, char *);
	format = va_arg(ap, char *);
#endif
	put = x_printsput;
	rput = x_printsput;
	strptr = string;
	state = XP_DONE;
	do	{
		switch (x_chunk()) {

		case XP_CPT:
			cp = va_arg(ap, char *);
			x_chunkcp(cp);
			break;

		case XP_IT:
			i = va_arg(ap, int);
			x_chunki(i);
			break;

		case XP_LT:
			l = va_arg(ap, long);
			x_chunkl(l);
			break;

		case XP_RT:
			cp = va_arg(ap, char *);
			ip = va_arg(ap, int *);
			x_chunkr(cp, ip);
			break;
			}
		} while (state != XP_DONE);
	va_end(ap);
#else
xsprintf(string, format, args)
	char *string;
	char *format;
	int args;
	{
	put = x_printsput;
	rput = x_printsput;
	strptr = string;
	x_printf2(format, &args);
#endif
	*strptr = NUL;
	put = x_printput;
	rput = x_printrput;
	}


/* ------------------------------------------------------------ */

/* Process the format string up to the next % argument, updating the
static FORMAT.  Return the expected type.  Update STATE with the new
state and ARG, ISARG, and ISZERO with the argument information. */

#if defined(VARARGS)
enum XP_TYPE
x_chunk()
	{

	while (*format != NUL) {
		if (state != XP_ARG && state != XP_CASE) {
			if (*format != '%') {
#if defined(MSDOS)
				if (istext && *format == NL) {
					(*put)(CR);
					}
#endif
				(*put)(*format++);
				continue;
				}

			arg = 0;
			isarg = FALSE;
			if (*++format == '*') {
				format++;
				state = XP_ARG;
				return(XP_IT);
				}
			else if (xisdigit(*format)) {
				iszero = *format == '0';
				while (xisdigit(*format)) {
					arg = arg * 10 + *format++ - '0';
					}
				isarg = TRUE;
				}
			else arg = 1;
			}

		if (state == XP_CASE) {
			while (arg-- > 0) {
				while (*format != NUL && (*format++ != '%' ||
					 (*format != ';' && *format != ':' &&
					  *format != ']'))) {
					if (*(format - 1) == '%' &&
						 *format == '%') format++;
					}
				if (*format != ';') arg = 0;
				}
			if (*format == ';' || *format == ':' || *format == ']')
				format++;
			if (*format == NUL) format--;
			state = XP_DONE;
			continue;
			}
		state = XP_DONE;

		switch (xtoupper(*format++)) {

		case '%':
			while (arg-- > 0) (*rput)('%');
			break;

		case '[':		/* CASE statment */
			state = XP_CASE;
			return(XP_IT);
			/*break;*/

		case ']':
			break;

		case ';':
		case ':':
			while (*format != NUL &&
				 (*format++ != '%' || *format != ']')) {
				if (*(format - 1) == '%' && *format == '%')
					format++;
				}
			if (*format == ']') format++;
			if (*format == NUL) format--;
			break;

		case 'R':		/* recursion */
			state = XP_R;
			return(XP_RT);
			/*break;*/

		case ',':		/* numbers */
			state = XP_COMMA;
			return(XP_IT);
			/*break;*/

		case 'D':
			state = XP_D;
			return(XP_IT);
			/*break;*/

		case 'H':		/* long hexadecimal */
			state = XP_H;
			return(XP_LT);
			/*break;*/

		case 'L':
			state = XP_L;
			return(XP_LT);
			/*break;*/

		case 'O':
			state = XP_O;
			return(XP_IT);
			/*break;*/

		case 'X':
			state = XP_X;
			return(XP_IT);
			/*break;*/

		case 'U':
			state = XP_U;
			return(XP_IT);
			/*break;*/

		case 'S':		/* strings */
			state = XP_S;
			return(XP_CPT);
			/*break;*/

		case 'C':		/* characters */
			state = XP_C;
			return(XP_IT);
			/*break;*/

		case 'Z':		/* NUL's */
			while (arg-- > 0) (*rput)(NUL);
			break;

		case DEL:
			while (arg-- > 0) (*rput)(DEL);
			break;

		case SP:
			while (arg-- > 0) (*rput)(SP);
			break;

		default:
			break;
			}
		}
	state = XP_DONE;
	return(XP_DONET);
	}
#endif


/* ------------------------------------------------------------ */

/* Print value VAL.  Import STATE, ARG, and ISARG as a static. */

#if defined(VARARGS)
void
x_chunkcp(val)
	char *val;
	{
	int tmp;

	switch (state) {

	case XP_S:
		if (isarg) tmp = strlen(val);
		while (*val != NUL) {
#if defined(MSDOS)
			if (istext && *val == NL) {
				(*put)(CR);
				}
#endif
			(*put)(*val++);
			}
		if (isarg) while (tmp++ < arg) (*put)(SP);
		break;
		}
	}
#endif


/* ------------------------------------------------------------ */

/* Print value VAL.  Import STATE, ARG, and ISZERO as a static. */

#if defined(VARARGS)
void
x_chunki(val)
	long val;
	{
	switch (state) {

	case XP_ARG:
	case XP_CASE:
		arg = val;
		isarg = TRUE;
		iszero = FALSE;
		break;

	case XP_COMMA:
		if (val < 0) {
			(*put)('-');
			val = -val;
			}
		x_printcomma(val, arg, 0);
		break;

	case XP_C:
		(*put)(val);
		break;

	case XP_D:
		if (val < 0) {
			(*put)('-');
			val = -val;
			}
		x_printnum(val, 10, arg, FALSE, iszero);
		break;

	case XP_O:
		x_printnum(val, 8, arg, FALSE, iszero);
		break;

	case XP_U:
		x_printnum(val, 10, arg, FALSE, iszero);
		break;

	case XP_X:
		x_printnum(val, 16, arg, FALSE, iszero);
		break;
		}
	}
#endif


/* ------------------------------------------------------------ */

/* Print value VAL.  Import STATE, ARG, and ISZERO as a static. */

#if defined(VARARGS)
void
x_chunkl(val)
	long val;
	{
	switch (state) {

	case XP_H:
		x_printlong(val, 16, arg, FALSE, iszero);
		break;

	case XP_L:
		if (val < 0) {
			(*put)('-');
			val = -val;
			}
		x_printlong(val, 10, arg, FALSE, iszero);
		break;
		}
	}
#endif


/* ------------------------------------------------------------ */

/* Print values VAL1 and VAL2.  Import STATE as a static. */

#if defined(VARARGS)
void
x_chunkr(val1, val2)
	char *val1;
	int *val2;
	{
	switch (state) {

	case XP_R:
		x_printf2(val1, val2);
		break;
		}
	}
#endif


/* ------------------------------------------------------------ */

void
x_printf2(fmt, parms)
	char *fmt;
	int *parms;
	{
	FLAG iszero;
	FLAG isarg;
	unsigned arg;
	char *cptr;
	char **cptrptr;
	int tmp;
	long *lptr;
	long ltmp;

	while (*fmt != NUL) {
		if (*fmt != '%') {
#if defined(MSDOS)
			if (istext && *fmt == NL) {
				(*put)(CR);
				}
#endif
			(*put)(*fmt++);
			continue;
			}

		arg = 0;
		isarg = FALSE;
		if (*++fmt == '*') {
			arg = *parms++;
			fmt++;
			isarg = TRUE;
			}
		else if (xisdigit(*fmt)) {
			iszero = *fmt == '0';
			while (xisdigit(*fmt)) {
				arg = arg * 10 + *fmt++ - '0';
				}
			isarg = TRUE;
			}
		else	arg = 1;

		switch (xtoupper(*fmt++)) {

		case '%':
			while (arg-- > 0) (*rput)('%');
			break;

		case '[':		/* CASE statment */
			arg = *parms++;
			while (arg-- > 0) {
				while (*fmt != NUL && (*fmt++ != '%' ||
					 (*fmt != ';' && *fmt != ':' &&
					  *fmt != ']'))) {
					if (*(fmt - 1) == '%' &&
						*fmt == '%') fmt++;
					}
				if (*fmt != ';') arg = 0;
				}
			if (*fmt == NUL) fmt--;
			break;

		case ']':
			break;

		case ';':
		case ':':
			while (*fmt != NUL &&
				 (*fmt++ != '%' || *fmt != ']')) {
				if (*(fmt - 1) == '%' && *fmt == '%')
					fmt++;
				}
			if (*fmt == NUL) fmt--;
			break;

		case 'R':		/* recursion */
			cptrptr = (char **)parms;
			cptr = *cptrptr++;
			parms = (int *)cptrptr;
			x_printf2(cptr, (int *)*parms++);
			break;

		case ',':		/* numbers */
			tmp = *parms++;
			if (tmp < 0) {
				(*put)('-');
				tmp = -tmp;
				}
			x_printcomma(tmp, arg, 0);
			break;

		case 'D':
			tmp = *parms++;
			if (tmp < 0) {
				(*put)('-');
				tmp = -tmp;
				}
			x_printnum(tmp, 10, arg, FALSE, iszero);
			break;

		case 'H':		/* long hexadecimal */
			lptr = (long *)parms;
			x_printlong(*lptr++, 16, arg, FALSE, iszero);
			parms = (int *)lptr;
			break;

		case 'L':
			lptr = (long *)parms;
			ltmp = *lptr++;
			if (ltmp < 0) {
				(*put)('-');
				ltmp = -ltmp;
				}
			x_printlong(ltmp, 10, arg, FALSE, iszero);
			parms = (int *)lptr;
			break;

		case 'O':
			x_printnum(*parms++, 8, arg, FALSE, iszero);
			break;

		case 'X':
			x_printnum(*parms++, 16, arg, FALSE, iszero);
			break;

		case 'U':
			x_printnum(*parms++, 10, arg, FALSE, iszero);
			break;

		case 'S':		/* strings */
			cptrptr = (char **)parms;
			cptr = *cptrptr++;
			parms = (int *)cptrptr;
			if (isarg) tmp = strlen(cptr);
			while (*cptr) {
#if defined(MSDOS)
				if (istext && *cptr == NL) {
					(*put)(CR);
					}
#endif
				(*put)(*cptr++);
				}
			if (isarg) while (tmp++ < arg) (*put)(SP);
			break;

		case 'C':		/* characters */
			(*put)(*parms++);
			break;

		case 'Z':		/* NUL's */
			while (arg-- > 0) (*rput)(NUL);
			break;

		case DEL:
			while (arg-- > 0) (*rput)(DEL);
			break;

		case SP:
			while (arg-- > 0) (*rput)(SP);
			break;

		default:
			break;
			}
		}
	}


/* ------------------------------------------------------------ */

/* Print out an integer. */

void
x_printnum(value, base, len, notfirst, iszero)
	unsigned value;
	int base;
	int len;
	FLAG notfirst;
	FLAG iszero;
	{
	if (value >= base || (len - 1) > 0)
		x_printnum(value / base, base, len - 1, TRUE, iszero);
	if (!value && len > 0 && notfirst) (*put)(iszero ? '0' : SP);
	else	{
		value %= base;
		if (value > 9) (*put)(value + 'a' - 10);
		else (*put)(value + '0');
		}
	}


/* ------------------------------------------------------------ */

/* Print out a long. */

void
x_printlong(value, base, len, notfirst, iszero)
	long value;
	int base;
	int len;
	FLAG notfirst;
	FLAG iszero;
	{
	if (value >= base || (len - 1) > 0)
		x_printlong(value / base, base, len - 1, TRUE, iszero);
	if (!value && len > 0 && notfirst) (*put)(iszero ? '0' : SP);
	else	{
		value %= base;
		if (value > 9) (*put)((int) value + 'a' - 10);
		else (*put)((int) value + '0');
		}
	}


/* ------------------------------------------------------------ */

/* Print out a decimal int with commas. */

void
x_printcomma(value, len, depth)
	unsigned value;
	int len;
	int depth;
	{
	if (value >= 10 || (len - 1) > 0)
		x_printcomma(value / 10, len - 1, depth + 1);
	value %= 10;
	(*put)(value + '0');
	if (depth && !(depth % 3)) (*put)(',');
	}


/* ------------------------------------------------------------ */

/* Print a normal character on the terminal. */

void
x_printput(c)
	char c;
	{
	if (printptr >= &printbuf[sizeof(printbuf) - 1]) {
		write(outfd, printbuf, printptr - printbuf);
		printptr = printbuf;
		}
	*printptr++ = c;
	}


/* ------------------------------------------------------------ */

/* Print a raw character on the terminal. */

void
x_printrput(c)
	char c;
	{
	if (printptr >= &printbuf[sizeof(printbuf) - 1]) {
		write(outfd, printbuf, printptr - printbuf);
		printptr = printbuf;
		}
	*printptr++ = c;
	}


/* ------------------------------------------------------------ */

/* Print a character to a string. */

void
x_printsput(c)
	char c;
	{
	*strptr++ = c;
	}


/* end of XPRINTF.C -- Printf with Extensions/Deletions (ANSI Version) */
