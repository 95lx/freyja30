/* LIBPARSE.C -- File Parser Routines

	Written October 1987 by Craig Finseth
	Copyright 1987,8,9,90,1,2,3 by Craig A. Finseth
*/

#include "lib.h"

char *Parse__Token();		/* char *cptr, struct parse_data *pptr */

/* ------------------------------------------------------------ */

/* Open the input file and initialize the structure variables. Return
0 on success or -1 on error. */

int
Parse_Open(pptr, fname)
	struct parse_data *pptr;
	char *fname;
	{
	if (pptr->p_length < 1) return(-1);
	if (strequ(fname, "")) pptr->p_fptr = stdin;
	else if ((pptr->p_fptr = fopen(fname, "r")) == NULL) return(-1);
	pptr->p_line = 0;
	xstrcpy(pptr->p_fname, fname);

	if (xisgray(pptr->p_separator)) {
		pptr->p_separator = SP;
		pptr->p_trim = TRUE;
		}
	return(0);
	}


/* ------------------------------------------------------------ */

/* Close the file and finish reading. */

void
Parse_Close(pptr)
	struct parse_data *pptr;
	{
	if (pptr->p_fptr != NULL && pptr->p_fptr != stdin)
		fclose(pptr->p_fptr);
	pptr->p_fptr = NULL;
	}


/* ------------------------------------------------------------ */

/* Parse the line according to the description.  Refer to PARSE.DOC
for a complete description of this function. */

int
Parse_A_Line(pptr, tokens, num_tokens)
	struct parse_data *pptr;
	char *tokens[];
	int num_tokens;
	{
	char *cptr;
	char *cptr2;
	char *begptr;
	char *endptr;
	int cnt;
	char flushbuf[BUFFSIZE];

	if (pptr->p_fptr == NULL) return(0);

	do	{

	/* read a line */

		cptr = fgets(pptr->p_buffer, pptr->p_length, pptr->p_fptr);
		pptr->p_line++;
		if (cptr == NULL) {	/* cannot separate EOF and error */
			Parse_Close(pptr);
			return(0);
			}

	/* if no newline, flush the rest of the line */

		if (*sindex(pptr->p_buffer, NL) == NUL) {
			do	{
				cptr2 = fgets(flushbuf, sizeof(flushbuf),
					pptr->p_fptr);
				if (cptr2 == NULL) {
					Parse_Close(pptr);
					break;
					}
				} while (*sindex(flushbuf, NL) == NUL);
			}

	/* check for blank and blank/comment lines */

		for (cptr = pptr->p_buffer; *cptr; cptr++) {
			if (*cptr == pptr->p_comment) {
				*cptr = NUL;		/* mark as no data */
				break;
				}
			if (*cptr == NL) {		/* at end of line */
				*cptr = NUL;
				break;
				}
			if (!xisgray(*cptr)) break;	/* we have data */
			}
		if (*cptr == NUL) *pptr->p_buffer = NUL;

		} while (*pptr->p_buffer == NUL);

	/* parse the line */

	if (num_tokens <= 0) return(0);		/* what else can we do? */

	cptr = pptr->p_buffer;
	for (cnt = 0; cnt < num_tokens; cnt++) {
		if (xisgray(pptr->p_separator)) {
			if (pptr->p_trim) { while (xisgray(*cptr)) cptr++; }
			if (*cptr == NUL || *cptr == pptr->p_comment)
				return(cnt);
			}
		else	{
			if (*cptr == NUL || *cptr == pptr->p_comment)
				return(cnt);
			if (pptr->p_trim) { while (xiswhite(*cptr)) cptr++; }
			}
		tokens[cnt] = cptr;
		begptr = cptr;

		cptr = Parse__Token(begptr, pptr);
		if (cptr == NULL) return(cnt - 1);

		if (pptr->p_trim) {		/* trim trailing whitespace */
			for (endptr = begptr + strlen(begptr) - 1;
				 endptr > begptr && xisgray(*endptr); ) {
				*endptr-- = NUL;
				}
			}
		}

	if (cptr != NULL) tokens[num_tokens - 1] = cptr; /* get all the rest */
	return(num_tokens);
	}


/* ------------------------------------------------------------ */

/* Parse the supplied string according to the description.  Refer to
PARSE.DOC for a complete description of this function. */

int
Parse_A_String(pptr, tokens, num_tokens, str)
	struct parse_data *pptr;
	char *tokens[];
	int num_tokens;
	char *str;
	{
	char *cptr;
	char *begptr;
	char *endptr;
	int cnt;

	if (num_tokens <= 0 || pptr->p_length < 1)
		return(0);	/* what else can we do? */

	strncpy(pptr->p_buffer, str, pptr->p_length);
	pptr->p_buffer[pptr->p_length - 1] = NUL;
	if (xisgray(pptr->p_separator)) {	/* normalize as Parse_Open */
		pptr->p_separator = SP;
		pptr->p_trim = TRUE;
		}

	cptr = pptr->p_buffer;
	for (cnt = 0; cnt < num_tokens; cnt++) {
		if (xisgray(pptr->p_separator)) {
			if (pptr->p_trim) { while (xisgray(*cptr)) cptr++; }
			if (*cptr == NUL || *cptr == pptr->p_comment)
				return(cnt);
			}
		else	{
			if (*cptr == NUL || *cptr == pptr->p_comment)
				return(cnt);
			if (pptr->p_trim) { while (xiswhite(*cptr)) cptr++; }
			}
		tokens[cnt] = cptr;
		begptr = cptr;

		cptr = Parse__Token(begptr, pptr);
		if (cptr == NULL) return(cnt - 1);

		if (pptr->p_trim) {		/* trim trailing whitespace */
			for (endptr = begptr + strlen(begptr) - 1;
				 endptr > begptr && xisgray(*endptr); ) {
				*endptr-- = NUL;
				}
			}
		}

	if (cptr != NULL) tokens[num_tokens - 1] = cptr; /* get all the rest */
	return(num_tokens);
	}


/* ------------------------------------------------------------ */

/* Return the current file name. */

char *
Parse_File(pptr)
	struct parse_data *pptr;
	{
	return(pptr->p_fname);
	}


/* ------------------------------------------------------------ */

/* Read and return a line, buf don't parse it.  Return 0 on success or
-1 on error. */

int Parse_Get_Line(pptr, buf, bufsize)
	struct parse_data *pptr;
	char *buf;
	int bufsize;
	{
	char *cptr;
	char flushbuf[BUFFSIZE];

/* read a line */

	cptr = fgets(buf, bufsize, pptr->p_fptr);
	pptr->p_line++;
	if (cptr == NULL) {	/* cannot separate EOF and error */
		Parse_Close(pptr);
		return(-1);
		}

/* if no newline, flush the rest of the line */

	if (*sindex(buf, NL) == NUL) {
		do	{
			cptr = fgets(flushbuf, sizeof(flushbuf), pptr->p_fptr);
			if (cptr == NULL) {
				Parse_Close(pptr);
				break;
				}
			} while (*sindex(flushbuf, NL) == NUL);
		}

	TrimNL(buf);
	return(0);
	}


/* ------------------------------------------------------------ */

/* Return the current line number. */

int
Parse_Line(pptr)
	struct parse_data *pptr;
	{
	return(pptr->p_line);
	}


/* ------------------------------------------------------------ */

/* Parse the next token from the string.  Return NULL when you have
run out of tokens. */

char *
Parse__Token(cptr, pptr)
	char *cptr;
	struct parse_data *pptr;
	{
	FLAG token;

	if (xisgray(pptr->p_separator)) {
		token = FALSE;
		while (*cptr && *cptr != pptr->p_comment &&
			 !xisgray(*cptr)) {
			cptr++;
			token = TRUE;
			}
		}
	else	{
		token = TRUE;
		while (*cptr && *cptr != pptr->p_comment && *cptr != NL &&
			 *cptr != pptr->p_separator) {
			cptr++;
			}
		}
	if (!token || !*cptr || *cptr == pptr->p_comment) {
		*cptr = NUL;
		return(NULL);
		}
	else	{
		*cptr = NUL;
		return(cptr + 1);
		}
	}

/* end of LIBPARSE.C -- File Parser Routines */
