/* FFILE.C -- File-, Buffer-, and System-Oriented Commands

	Written July 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"

#if defined(MSDOS)
#include <time.h>
#endif

char *getenv();

void F_FPathOpen_DirSep(char *name);

/* ------------------------------------------------------------ */

/* Move to the begining of the buffer. */

void
FBufBeg(void)
	{
	BMarkToPoint(mark);
	BMoveToStart();
	while (--uarg > 0) SearchNLF();
	}


/* ------------------------------------------------------------ */

/* Delete the current buffer. */

void
FBufDel(void)
	{
	struct window *wptr;

	uarg = 0;
	if (!IS_SYS(cbuf->fname) && BIsMod(cbuf) &&
		 KAsk(RES_PROMPTDELMOD) != 'Y')
		return;

	if (num_windows > 1) {
		wptr = (cwin == &windows[0]) ? &windows[1] : &windows[0];
		if (cbuf == BMarkBuf(wptr->sstart)) DWindOne();
		}
	BBufDelete();
	DScreenRange();
	}


/* ------------------------------------------------------------ */

/* Move to the end of the buffer. */

void
FBufEnd(void)
	{
	if (isuarg)
		FBufBeg();
	else	{
		BMarkToPoint(mark);
		BMoveToEnd();
		}
	}


/* ------------------------------------------------------------ */

/* Display a buffer list. */

void
FBufList(void)
	{
	char buf[LINEBUFFSIZE];
	struct buffer *bptr;

	uarg = 0;
	if (!FMakeSys(Res_String(NULL, RES_MSGS, RES_SYSBUFFLIST), TRUE))
		return;

	for (bptr = cbuf + 1; bptr != cbuf; bptr++) {
		if (bptr >= &buffers[NUMBUFFERS]) bptr = buffers;
		if (!BIsFree(bptr) &&
			 (!c.g.skip_system || isuarg || !IS_SYS(bptr->fname))) {
			xsprintf(buf, "%l	%s%s",
				BGetLength(bptr),
				BIsMod(bptr) ? "*" : " ",
				bptr->fname);
			BInsStr(buf);
			BInsChar(NL);
			}
		}
	BMoveToStart();
	DWindSwap();
	}


/* ------------------------------------------------------------ */

/* Switch to the next buffer in the ring. */

void
FBufNext(void)
	{
	struct buffer *bptr = cbuf;

	uarg = 0;
	while (1) {
		if (++bptr >= &buffers[NUMBUFFERS]) bptr = buffers;
		if (bptr == cbuf) return;
		if (BIsFree(bptr)) continue;
		if (c.g.skip_system && !isuarg && IS_SYS(bptr->fname)) continue;
		BBufGoto(bptr);
		return;
		}
	}


/* ------------------------------------------------------------ */

/* Switch to the previous buffer in the ring. */

void
FBufPrev(void)
	{
	struct buffer *bptr = cbuf;

	uarg = 0;
	while (1) {
		if (bptr == buffers) bptr = &buffers[NUMBUFFERS];
		bptr--;
		if (bptr == cbuf) return;

		if (BIsFree(bptr)) continue;
		if (c.g.skip_system && !isuarg && IS_SYS(bptr->fname)) continue;
		BBufGoto(bptr);
		return;
		}
	}


/* ------------------------------------------------------------ */

/* Print the buffer. */

void
FBufPrint(void)
	{
	BMoveToEnd();
	BMarkToPoint(mark);
	BMoveToStart();
	RPrint();
	}


/* ------------------------------------------------------------ */

/* Execute a shell command. */

void
FCommand(void)
	{
	char cmd_line[LINEBUFFSIZE];

	uarg = 0;
	*cmd_line = NUL;
	if (KGetStr(RES_PROMPTCMD, cmd_line, LINEBUFFSIZE) != 'Y') return;

	TSetPoint(TMaxRow() - 1, 0);
	TCLEOL();
	TFini();

#if defined(SYSMGR)
	JSystem(cmd_line);
#else
	system(cmd_line);

	xprintf(Res_String(NULL, RES_MSGS, RES_RETURN));

	while (getchar() != NL);
#endif

	TInit();
	DNewDisplay();
	}


/* ------------------------------------------------------------ */

/* Directory display. */

void
FDIRED(void)
	{
	char fname[FNAMEMAX];

	*fname = NUL;
	if (KGetFile(RES_PROMPTDIRNAME, fname, FALSE) != 'Y') return;
#if defined(MSDOS)
	if (*fname == NUL) xstrcpy(fname, "*.*");
#endif
	FDIREDDo(fname, isuarg, FALSE);
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Do the work for DIRED or completion.  FNAME is the name to match.
LONGDISP is True if a long display is to be done.  ISCOMPL is True on
file completion (don't include . and .. entries and omit the directory
part), False otherwise. Return True on success or False on failure. */

FLAG
FDIREDDo(char *fname, FLAG longdisp, FLAG iscompl)
	{
	char buf[LINEBUFFSIZE];
#if defined(UNIX)
	char cmd_line[LINEBUFFSIZE];
	FILE *fptr;
#endif
#if defined(MSDOS)
	struct DOSFCB {
		char DOSreserved[21];
		char DOSattr;
		int DOStime;
		int DOSdate;
		long DOSsize;
		char DOSname[13];
		} fcb;
	char dirname[FNAMEMAX];
#endif

	FFileFixName(fname);
	if (!FMakeSys(Res_String(NULL, RES_MSGS, RES_SYSDIRED), TRUE))
		return(FALSE);

#if defined(UNIX)
	xsprintf(cmd_line, "/bin/ls -ad%s %s 2>/dev/null",
		longdisp ? "lg" : "", fname);
	if ((fptr = popen(cmd_line, "r")) == NULL) {
		DError(RES_LISTERR);
		return(FALSE);
		}

	while (fgets(buf, sizeof(buf), fptr) != NULL) {
		if (iscompl) {
			FFileGetFile(buf, buf);
			if (strequ(buf, ".") || strequ(buf, "..")) continue;
			}
		BInsStr(buf);
		}
	pclose(fptr);
#endif
#if defined(MSDOS)
	PSystem(0x1A, &fcb);			/* set disk transfer address */
		/* look for subdirectory + system + hidden files */
	FFileGetDir(dirname, fname);
	if (PSystem(0x4E, fname, 16 + 4 + 2) >= 0) {
		do	{
			if (iscompl) {
				if (strequ(fcb.DOSname, ".") ||
					 strequ(fcb.DOSname, ".."))
					continue;
				}
			else	{
				if (*dirname != NUL) {
					BInsStr(dirname);
					if (dirname[strlen(dirname) - 1] !=
						  ':')
						BInsChar('/');
					}
				}
			if (!longdisp) {
				BInsStr(fcb.DOSname);
				BInsChar(NL);
				continue;
				}
			if (fcb.DOSattr & 16) {
				BInsStr("<DIR>");
				}
			else	{
				xsprintf(buf, "%l", fcb.DOSsize);
				BInsStr(buf);
				}
			xsprintf(buf, "\t%4d-%02d-%02d %2d:%02d %s%s%s%s %s",
				((fcb.DOSdate >> 9) & 0x7F) + 1980,
				 (fcb.DOSdate >> 5) &  0xF,
				  fcb.DOSdate       & 0x1F,
				(fcb.DOStime >> 11) & 0x1F,
				(fcb.DOStime >>  5) & 0x3F,
				fcb.DOSattr & 32 ? "Ar" : "  ",
				fcb.DOSattr &  4 ? "Sy" : "  ",
				fcb.DOSattr &  2 ? "Hd" : "  ",
				fcb.DOSattr &  1 ? "RO" : "  ",
				fcb.DOSname);
			BInsStr(buf);
			BInsChar(NL);
			} while (PSystem(0x4F) >= 0);
		}
#endif
	BMoveToStart();
	DWindSwap();
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Delete a file. */

void
FFileDelete(void)
	{
	uarg = 0;
	xstrcpy(filearg, cbuf->fname);
	do	{
		if (KGetFile(RES_PROMPTDELETE, filearg, TRUE) != 'Y') return;
		} while (*filearg == NUL);

	if (unlink(filearg) < 0) DError(RES_ERRDELETE);
	}


/* ------------------------------------------------------------ */

/* Find file. */

void
FFileFind(void)
	{
	struct buffer *bptr;

	uarg = 0;
	do	{
		if (KGetFile(RES_PROMPTFIND, filearg, TRUE) != 'Y') return;
		} while (*filearg == NUL);

	FFileFixName(filearg);
	bptr = BBufFind(filearg);

	if (bptr == NULL) {
		if (BBufCreate(filearg) == NULL) return;
		if (!BFileRead()) DError(RES_ERRREAD);
		}
	else	BBufGoto(bptr);
	}


/* ------------------------------------------------------------ */

/* Fix the directory part of the name.  "Fixing" involves:

- if the name has a directory part, do nothing.
- otherwise, add the directory part of cbuf->fname to it. */

void
FFileFixName(char *fname)
	{
	char *cptr;
	char new[FNAMEMAX];

	if (*fname == '/' ||
#if defined(MSDOS)
		 *fname == '\\' ||
		 strnequ(fname, ".\\", 2) ||
		 strnequ(fname, "..\\", 3) ||
		 (xisalpha(*fname) && *(fname + 1) == ':') ||
#endif
		 strnequ(fname, "./", 2) || strnequ(fname, "../", 3)) {
		return;
		}

/* do the fixup */

	FFileGetDir(new, cbuf->fname);
	cptr = new + strlen(new);
	if (cptr != new) {
#if defined(MSDOS)
		if (*(cptr - 1) != ':')
#endif
			*cptr++ = '/';
		}
	xstrcpy(cptr, fname);
	xstrcpy(fname, new);
	}


/* ------------------------------------------------------------ */

/* Return the file part of PATHNAME in FNAME, which should be at least
FNAMEMAX characters long. */

void
FFileGetFile(char *fname, char *pathname)
	{
	char *cptr;

	cptr = pathname + strlen(pathname);
	while (cptr > pathname && *cptr != '/'
#if defined(MSDOS)
		 && *cptr != '\\' && *cptr != ':'
#endif
		) cptr--;

	if (*cptr == '/'
#if defined(MSDOS)
		|| *cptr == '\\' || *cptr == ':'
#endif
		) cptr++;
	xstrcpy(fname, cptr);
	}


/* ------------------------------------------------------------ */

/* Return the directory part of cbuf->fname in FNAME, which which
should be at least FNAMEMAX characters long. */

void
FFileGetDir(char *fname, char *fullname)
	{
	char *cptr;

	xstrcpy(fname, fullname);

	cptr = fname + strlen(fname);
	while (cptr > fname && *cptr != '/'
#if defined(MSDOS)
		 && *cptr != '\\' && *cptr != ':'
#endif
		) cptr--;

	if (cptr == fname) {	/* no directory part */
		*fname = NUL;
		return;
		}
#if defined(MSDOS)
	if (*cptr == ':') {
		*(cptr + 1) = NUL;
		return;
		}
#endif
	*cptr = NUL;
	}


/* ------------------------------------------------------------ */

/* Insert a file. */

void
FFileIns(void)
	{
	struct buffer *bptr;
	struct buffer *savebuf = cbuf;

	uarg = 0;
	do	{
		if (KGetFile(RES_PROMPTINS, filearg, FALSE) != 'Y') return;
		} while (*filearg == NUL);

	if ((bptr = BBufFind(Res_String(NULL, RES_MSGS,
		 RES_SYSINCLUDE))) != NULL)
		BBufGoto(bptr);
	else if ((bptr = BBufCreate(Res_String(NULL, RES_MSGS,
		 RES_SYSINCLUDE))) == NULL) {
		DError(RES_ERRINS);
		return;
		}

	FFileFixName(filearg);
	xstrcpy(bptr->fname, filearg);
	if (BFileRead()) {
		BMoveToEnd();
		BRegCopy(mark, savebuf);
		}
	else	{
		DError(RES_ERRREAD);
		}
	BBufDelete();
	BBufGoto(savebuf);
	}


/* ------------------------------------------------------------ */

/* Find file using the search path. */

void
FFilePath(void)
	{
	struct buffer *bptr;
	char fname[FNAMEMAX];
	char fname2[FNAMEMAX];
	int fd;

	*fname = NUL;
	uarg = 0;
	do	{
		if (KGetFile(RES_PROMPTFIND, fname, TRUE) != 'Y') return;
		} while (*fname == NUL);

	if ((fd = FPathOpen(fname, fname2)) < 0) {
		DError(RES_ERROPEN);
		return;
		}
	close(fd);

	bptr = BBufFind(fname2);

	if (bptr == NULL) {
		if (BBufCreate(fname2) == NULL) return;
		if (!BFileRead()) DError(RES_ERRREAD);
		}
	else	BBufGoto(bptr);
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Re-read a file into the current buffer. */

void
FFileRead(void)
	{
	uarg = 0;
	xstrcpy(filearg, cbuf->fname);
	do	{
		if (KGetFile(RES_PROMPTREAD, filearg, FALSE) != 'Y') return;
		} while (*filearg == NUL);

	if (BIsMod(cbuf) && KAsk(RES_ASKTHROW) != 'Y') {
		DModeLine();
		return;
		}

	FFileFixName(filearg);
	xstrcpy(cbuf->fname, filearg);
	if (!BFileRead()) DError(RES_ERRREAD);
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Write the buffer to its file. */

void
FFileSave(void)
	{
	uarg = 0;
	if (!Res_Char(RES_CONF, RES_SAVEUNMOD) == 'N' && !BIsMod(cbuf)) return;
	DEcho(Res_String(NULL, RES_MSGS, RES_WRITING));
	if (!BFileWrite()) DError(RES_ERRSAVE);
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Write the buffer to a different file. */

void
FFileWrite(void)
	{
	uarg = 0;
	xstrcpy(filearg, cbuf->fname);
	do	{
		if (KGetFile(RES_PROMPTWRITE, filearg, FALSE) != 'Y') return;
		} while (*filearg == NUL);
	FFileFixName(filearg);
	xstrcpy(cbuf->fname, filearg);
	DEcho(Res_String(NULL, RES_MSGS, RES_WRITING));
	if (!BFileWrite()) DError(RES_ERRSAVE);
	DModeLine();
	}


/* ------------------------------------------------------------ */

/* Return the current directory in BUF, which should be at least
FNAMEMAX long. */

void
FGetDir(char *buf)
	{
#if defined(UNIX)
	FILE *fptr;

	*buf = NUL;
	if ((fptr = popen("pwd", "r")) == NULL) {
		return;
		}

	if (fgets(buf, FNAMEMAX - 1, fptr) == NULL) {
		pclose(fptr);
		return;
		}
	pclose(fptr);
	TrimNL(buf);
#endif
#if defined(MSDOS)
	*buf++ = (PSystem(0x19) & 0xFF) + 'A';
	*buf++ = ':';
	*buf++ = '\\';
	DOSGetDir(0, buf);
#endif
	}


/* ------------------------------------------------------------ */

/* Create and set up the system buffer. Return True on success or
False on failure.  Zap the buffer if ERASE is true.  Otherwise, just
move to the end. */

FLAG
FMakeSys(char *name, FLAG erase)
	{
	struct buffer *savebuf = cbuf;
	struct buffer *bptr;

/* find or make it */

	if ((bptr = BBufFind(name)) != NULL)
		BBufGoto(bptr);
	else if ((bptr = BBufCreate(name)) == NULL)
		return(FALSE);

/* see if it is visible already */

	if (BMarkBuf(cwin->sstart) != cbuf) {
		DWindTwo();
		BBufGoto(savebuf);
		DWindSwap();
		BBufGoto(bptr);
		DIncrDisplay();
		}

/* clean buffer */

	if (erase) {
		BMoveToStart();
		BMarkToPoint(mark);
		BMoveToEnd();
		BRegDelete(mark);
		}
	else	BMoveToEnd();
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Open a file for input, searching the path.  NAME should not have a
directory part.  If ACTUALNAME is not NULL, the actual name as opened
will be filled in.  PExpand should be called if you want to remove all
ambuguity.  It searches the following order:

	$FREYJADIR, $HOME, $PATH, current dir
*/

#if defined(MSDOS)
#define SEPCHAR		';'
#define SEPSTR		";"
#endif
#if defined(UNIX)
#define SEPCHAR		':'
#define SEPSTR		":"
#endif

int
FPathOpen(char *name, char *actualname)
	{
	int fd;
	char tname[FNAMEMAX];
	char tpath[2 * FNAMEMAX];
	char *cptr;

#if defined(SYSMGR)
	JGetDir(tpath);
	tpath[strlen(tpath) - 1] = SEPCHAR;
#else
	*tpath = NUL;
#endif
	if ((cptr = getenv("FREYJADIR")) != NULL) {
		strcat(tpath, cptr);
		strcat(tpath, SEPSTR);
		}

	if ((cptr = getenv("HOME")) != NULL) {
		strcat(tpath, cptr);
		strcat(tpath, SEPSTR);
		}

	if ((cptr = getenv("PATH")) != NULL) {
		if (*cptr == SEPCHAR)  strcat(tpath, ".");
		strcat(tpath, cptr);
		}

	for (cptr = tpath; *cptr != NUL; cptr++) {
		if (*cptr == SEPCHAR) *cptr = NUL;
		}
	*(cptr + 1) = NUL;

	for (cptr = tpath; *cptr != NUL; ) {
		xstrcpy(tname, cptr);
		F_FPathOpen_DirSep(tname);
		strcat(tname, name);
#if defined(MSDOS)
#define O_RDONLY	0	/* dummy */
		if ((fd = open(tname, O_RDONLY)) >= 0) {
#endif
#if defined(UNIX)
		if ((fd = open(tname, O_RDONLY, 0)) >= 0) {
#endif
			if (actualname != NULL) xstrcpy(actualname, tname);
			return(fd);
			}
		while (*cptr++ != NUL) ;
		}

#if defined(MSDOS)
	if ((fd = open(name, O_RDONLY)) >= 0) {
#endif
#if defined(UNIX)
	if ((fd = open(name, O_RDONLY, 0)) >= 0) {
#endif
		if (actualname != NULL) xstrcpy(actualname, name);
		return(fd);
		}

	return(-1);
	}


/* ------------------------------------------------------------ */

/* Ask for the printer. */

void
FPrintSel(void)
	{
	if (!isuarg)
		KMenuDo(RES_PRSEL2, 0);
	else	printer = uarg;
	uarg = 0;
	}


/* ------------------------------------------------------------ */

/* Save all modified buffers */

void
FSaveAll(void)
	{
	struct buffer *bptr;
	struct buffer *savebuf = cbuf;

	uarg = 0;
	for (bptr = savebuf + 1; bptr != savebuf; bptr++) {
		if (bptr >= &buffers[NUMBUFFERS]) bptr = buffers;
		if (!BIsFree(bptr) && !IS_SYS(bptr->fname)) {
			BBufGoto(bptr);
			FFileSave();
			}
		}
	BBufGoto(savebuf);
	}


/* ------------------------------------------------------------ */

/* Bring up the DIRED buffer. */

void
FToDIRED(void)
	{
	struct buffer *bptr;

	bptr = BBufFind(Res_String(NULL, RES_MSGS, RES_SYSDIRED));
	if (bptr == NULL) {
		FDIRED();
		}
	else	{
		BBufGoto(bptr);
		uarg = 0;
		}
	}


/* ------------------------------------------------------------ */

void
F_FPathOpen_DirSep(char *name)
	{
#if defined(UNIX)
	if (*(name + strlen(name) - 1) != '/') strcat(name, "/");
#endif
#if defined(MSDOS)
	if (*(name + strlen(name) - 1) != '/' &&
		*(name + strlen(name) - 1) != '\\') strcat(name, "/");
#endif
	}


/* end of FFILE.C -- File-, Buffer-, and System-Oriented Commands */
