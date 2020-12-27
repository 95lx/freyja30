/* LIB.C -- Miscellaneous Library Functions

	Written July 1988 by Craig Finseth
*/

#include "lib.h"

/* ------------------------------------------------------------ */

/* Return the larger of the two numbers. */

int
max(a, b)
	int a;
	int b;
	{
	return(a > b ? a : b);
	}


/* ------------------------------------------------------------ */

/* Return the smaller of the two numbers. */

int
min(a, b)
	int a;
	int b;
	{
	return(a < b ? a : b);
	}


/* ------------------------------------------------------------ */

/* Set the extension part of a name to EXT.  If FORCE is TRUE, set the
extension even if there is one.  Return TRUE if the extension was set
or FALSE if it wasn't.  EXT doesn't include the period!! */

FLAG
PSetExt(name, ext, force)
	char *name;
	char *ext;
	FLAG force;
	{
	char *cptr;
	char *c1;

	cptr = c1 = name + strlen(name);
	while (cptr > name && *cptr != '.' && *cptr != '/'
#if defined(MSDOS)
		 && *cptr != '\\' && *cptr != ':'
#endif
		) cptr--;
	if (*cptr != '.') cptr = c1;
	if (force || !*cptr) {
		*cptr++ = '.';
		xstrcpy(cptr, ext);
		return(TRUE);
		}
	return(FALSE);
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the first occurrance in STR of any of the
characters in ANY. */

char *
sfindin(str, any)
	char *str;
	char *any;
	{
	while (*str != NUL && !*sindex(any, *str)) ++str;
	return(str);
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the first occurrance in STR of any of the
characters NOT in ANY. */

char *
sfindnotin(str, any)
	char *str;
	char *any;
	{
	while (*str != NUL && *sindex(any, *str)) ++str;
	return(str);
	}


/* ------------------------------------------------------------ */

/* Return a pointer to the first occurrance of C in STR. */

char *
sindex(str, chr)
	char *str;
	char chr;
	{
	while (*str != NUL && *str != chr) str++;
	return(str);
	}


/* ------------------------------------------------------------ */

/* Convert the value in STR to a number and return it in N.  Return
TRUE if the conversion was successful, FALSE if the string was not a
valid number.

This routine allows for leading whitespace and can handle leading + or
- signs.

The number's base is determined by inspection.  Forms are:

	###b, ###B	binary number		#=0,1

	###o, ###O	octal number		#=0-7
	###q, ###Q
	0###

	###		decimal number		#=0-9
	###.
	###d, ###D

	###h, ###H	hexadecimal number	#=0-9, a-f, A-F
	0x###, 0X###

	###k, ###K	decimal number * 1024	#=0-9
*/

FLAG
SToAny(str, n)
	char *str;
	int *n;
	{
	char buf[INPMAX];
	int base;
	char *cptr;
	char c;

	while (*str == SP || *str == TAB)	/* skip leading whitespace */
		str++;
	if (!*str) return(FALSE);	/* null string is not a number */
	xstrcpy(buf, str);

	cptr = buf;
	if (*cptr == '-' || *cptr == '+') cptr++;

	base = 10;
	if (*cptr == '0') {
		if (xtoupper(*(cptr + 1)) == 'X') {
			base = 16;
			*(cptr + 1) = '0';
			}
		else	base = 8;
		}
	else	{
		cptr = sindex(cptr, NUL) - 1;
		c = xtoupper(*cptr);
		if (c == 'B')			{ base = 2; *cptr = NUL; }
		else if (c == 'O' || c == 'Q')	{ base = 8; *cptr = NUL; }
		else if (c == '.')		{ base = 10; }
		else if (c == 'D')		{ base = 10; *cptr = NUL; }
		else if (c == 'H')		{ base = 16; *cptr = NUL; }
		else if (c == 'K') {
			base = 10;
			*cptr = NUL;
			if (!SToN(buf, n, base)) return(FALSE);
			*n *= 1024;
			return(TRUE);
			}
		}
	return(SToN(buf, n, base));
	}


/* ------------------------------------------------------------ */

/* Convert the value in STR to a decimal number and return it in N. 
The number is in base BASE.  Return TRUE if the conversion was
successful, FALSE if the string was not a valid number.

This routine allows for leading whitespace and can handle leading + or
- signs. */

FLAG
SToN(str, n, base)
	char *str;
	int *n;
	int base;
	{
	unsigned val;
	int minus;
	char chr;

	while (*str == SP || *str == TAB) str++;
	if (*str == '-') {
		minus = -1;
		str++;
		}
	else	{
		minus = 1;
		if (*str == '+') ++str;
		}
	for (val = 0; *str; ++str) {
		chr = xtoupper(*str);
		if (xisalpha(chr)) chr -= 'A' - 10;
		else if (xisdigit(chr)) chr -= '0';
		else return(FALSE);
		if (chr >= base) return(FALSE);
		val = val * base + chr;
		}
	*n = val * minus;
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Compare two strings.  Return FALSE on not equal, TRUE on equal.
Comparison is case-independant. */

FLAG
strequ(a, b)
	char *a;
	char *b;
	{
	while (*a && *b) if (xtolower(*a++) != xtolower(*b++)) return(FALSE);
	if (*a || *b) return(FALSE);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Compare two strings.  Return FALSE on not equal, TRUE on equal.
Comparison is case-independant.  Compare at most LEN characters. */

FLAG
strnequ(a, b, len)
	char *a;
	char *b;
	int len;
	{
	while (*a && *b && len-- > 0) {
		if (xtolower(*a++) != xtolower(*b++)) return(FALSE);
		}
	if (len <= 0) return(TRUE);
	if (*a || *b) return(FALSE);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Zap a trailing newline character from the string, if one is
present.  Return the original string. */

char *
TrimNL(str)
	char *str;
	{
	char *savestr = str;

	if (*str != NUL) {
		str += strlen(str) - 1;
		if (*str == NL) *str = NUL;
		}
	return(savestr);
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

FLAG
xisalnum(c)
	int c;
	{
	return(xisalpha(c) || xisdigit(c));
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

FLAG
xisalpha(c)
	int c;
	{
	return(xisupper(c) || xislower(c));
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

FLAG
xisdigit(c)
	int c;
	{
	return(c >= '0' && c <= '9');
	}


/* ------------------------------------------------------------ */

/* Return TRUE if C is a grayspace character (Space, Tab, CR, LF, FF:
see iswhite). */

FLAG
xisgray(c)
	char c;
	{
	return(c == TAB || c == SP || c == CR || c == NL || c == FF);
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

FLAG
xislower(c)
	int c;
	{
	return(c >= 'a' && c <= 'z');
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

FLAG
xisupper(c)
	int c;
	{
	return(c >= 'A' && c <= 'Z');
	}


/* ------------------------------------------------------------ */

/* Return TRUE if C is a whitespace character (Space or Tab only: see
isgray). */

FLAG
xiswhite(c)
	char c;
	{
	return(c == TAB || c == SP);
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

FLAG
xisxdigit(c)
	int c;
	{
	return((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F'));
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but handle overlapping strings. */

void
xstrcpy(dest, src)
	char *dest;
	char *src;
	{
	do	{
		*dest++ = *src;
		} while (*src++);
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

int
xtolower(c)
	int c;
	{
	return(xisupper(c) ? c + ('a' - 'A') : c);
	}


/* ------------------------------------------------------------ */

/* As ANSI version, but no domain limits. */

int
xtoupper(c)
	int c;
	{
	return(xislower(c) ? c + ('A' - 'a') : c);
	}


/* end of LIB.C -- Miscellaneous Library Functions */
