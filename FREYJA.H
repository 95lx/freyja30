/* FREYJA.H -- Freyja Header File

	Written 1980 by Mark of the Unicorn, Inc.
	Rewritten 1991 by Craig A. Finseth
	Copyright 1990,1,2,3,4 by Craig A. Finseth

*/

/* -------------------- We Begin -------------------- */

#include "lib.h"
#include "frres.h"

#if defined(MSDOS)
#if defined(MAIN)
#if !defined(SYSMGR)
#define STACKSIZE	((10 * 1024) / 16);
int _stklen = STACKSIZE;
#endif
#endif
#include <time.h>
time_t time();
struct tm *localtime();
#endif

#if defined(SYSMGR)
#undef CALC
#endif

/* -------------------- Constants -------------------- */

typedef	int KEYCODE;

	/* amount of time to delay for delayed echos -- this is a loop count*/
#if defined(MSDOS)
#define DELAYCOUNT	0
#else
#define DELAYCOUNT	1000
#endif

	/* size of the largest terminal to handle */
#if defined(MSDOS)
#define ROWMAX		43
#define COLMAX		80
#else
#define ROWMAX		132
#define COLMAX		240
#endif

	/* universal global declarations */
#define	FORWARD		1
#define	BACKWARD	0

	/* maximum limits */
#define MACROMAX	128		/* max size of a keyboard macro */
#define	STRMAX		60		/* size of a response */
#define LINEBUFFSIZE	255		/* size of an output line buffer */
#define SMALLBUFFSIZE	33

#define BIGBUFFSIZE	(FNAMEMAX)	/* largest of FNAMEMAX and previous
					buffer sizes */
#define NUMBUFFERS	12		/* buffers */

	/* pseudo-key definitions */
#define KEYNONE		(-1)		/* no valid key */
#define KEYABORT	(-2)
#define KEYQUIT		(-3)
#define KEYREGEN	(-4)		/* regenerate display */
#define KEYHELP		(0x100 + 59)
#define KEYF1		(0x100 + 59)
#define KEYF2		(0x100 + 60)
#define KEYF3		(0x100 + 61)
#define KEYF4		(0x100 + 62)
#define KEYHOME		(0x100 + 71)
#define KEYUP		(0x100 + 72)
#define KEYPGUP		(0x100 + 73)
#define KEYLEFT		(0x100 + 75)
#define KEYRIGHT	(0x100 + 77)
#define KEYEND		(0x100 + 79)
#define KEYDOWN		(0x100 + 80)
#define KEYPGDN		(0x100 + 81)
#define KEYCEND		(0x100 + 117)
#define KEYCHOME	(0x100 + 119)

	/* character set masks from the resource file */
#define CSETMASK_LOW	1	/* lower case letter */
#define CSETMASK_UPP	2	/* upper case letter */
#define CSETMASK_SAME	4	/* same case letter (only one case available),
				also covers other characters included in
				tokens (e.g., _ if you like) */
#define CSETMASK_CNUM	8	/* calculator number */
#define CSETMASK_WHI	16	/* whitespace character (space, tab) */
#define CSETMASK_NL	32	/* newline character */
#define CSETMASK_SENT	64	/* sentence end character */
#define CSETMASK_CLOSE	128	/* closing punct. character */

#define CSETMASK_TOK	(CSETMASK_LOW | CSETMASK_UPP | CSETMASK_SAME)

/* -------------------- Configuration Settings -------------------- */

#define RES_FILENAME	"freyja.ri"

struct conf_buffer {
	int left_margin;	/* indent column */
	int right_margin;	/* fill column */
	int tab_spacing;	/* tab stop spacing */
	char fill;		/* filling mode */
	};

struct conf_global {
	char madefor;		/* made for */
	KEYCODE ESC_swap;	/* swap Esc and this char (27 for no swap) */
	KEYCODE CTX_swap;	/* swap ^X and this char (24 for no swap) */
	char meta_handle;	/* bit 2^8 in a character */
	FLAG vis_gray;		/* True if grayspace is visible, False if not*/
	char vis_nl_char;
	char vis_tab_char;
	char vis_sp_char;
	FLAG skip_system;	/* True if system buffers should be skipped */
	FLAG wordend;		/* True if words end a end */
	char *ctype;		/* character set definitions array */
	char *upper;		/* convert to upper case array */
	char *lower;		/* convert to lower case array */
	};

struct conf {
	struct conf_buffer d;
	struct conf_global g;
	};

#if defined(MAIN)
struct conf c;
int swap_size;
int key_method;
int screen_type;
#else
extern struct conf c;
extern int swap_size;
extern int key_method;
extern int screen_type;
#endif

/* -------------------- Windows -------------------- */

#define NUMWINDOWS	2

struct window {
	FLAG visible;		/* is the window visible/in use? */
	int top;		/* first line of window */
	int bot;		/* last line of window */
	struct mark *sstart;	/* mark at the start of the window */
	struct mark *send;	/* mark at the end of the window */
	FLAG isend;		/* is the end mark valid? */
	struct mark *point;	/* the point */
	int offset;		/* the horizontal offset */
	};

#if defined(MAIN)
struct window windows[NUMWINDOWS];
struct window *cwin;		/* the current window */
int num_windows;		/* number of windows being displayed */
#else
extern struct window windows[NUMWINDOWS];
extern struct window *cwin;
extern int num_windows;
#endif

/* -------------------- Buffers -------------------- */

#define	PAGESIZE	1024		/* page size in bytes */
#define	SWAPMAX		512		/* maximum number of pages */
#define NUM_PHYS_PAGES	16		/* number of pages in memory */

#define	MAXMARK		(2 * NUMBUFFERS + 4 * NUMWINDOWS + 4)

struct virt_page {
	struct virt_page *next;		/* chain of pages in buffer */
	struct virt_page *prev;
	char where;			/* where page is M-memory, S-swap,
					L-locked */
	int page_num;			/* page number in the swap area */
	int gap_start;			/* gap start */
	int page_len;			/* page length */
	};

struct mark {
	struct virt_page *pptr;		/* page in the buffer */
	struct buffer *bptr;		/* buffer the mark is in */
	int mark_offset;		/* offset in the page */
	FLAG is_mod;			/* screen mark modified flags */
	};

struct buffer {
	char fname[FNAMEMAX];		/* the file name */
	struct conf_buffer c;		/* configuration parameters */
	struct mark *mptr;		/* the mark */
	struct virt_page *first;	/* describe the pages */
	struct virt_page *last;
	struct virt_page *point_page;	/* point's page number */
	int point_offset;		/* offset within page */
	int num_pages;			/* number of pages in buffer */
	FLAG is_mod;			/* has the buffer been modified? */
#ifdef UNIX
	long file_time;
#endif
	};

#if defined(MAIN)
struct buffer buffers[NUMBUFFERS];
#else
extern struct buffer buffers[NUMBUFFERS];
#endif

	/* current buffer information */
#if defined(MAIN)
struct buffer *cbuf;			/* the curent buffer */
struct mark *mark;			/* the current mark */
#else
extern struct buffer *cbuf;
extern struct mark *mark;
#endif

	/* system buffer support */
#define IS_SYS(x)	(*(x) == '%')

/* -------------------- Globals -------------------- */

#if defined(MAIN)
KEYCODE lastkey;	/* the last command key given */
int lasttable;		/* the last prefix table used */
KEYCODE key;		/* the current command key */
int table;		/* the current command table */

int uarg;		/* command argument */
FLAG isuarg;		/* was there an argument? */
FLAG isrepeating;	/* are we on the second or later repetition? */
FLAG doabort;		/* do we leave the editor? */

char togpgln;		/* are the PgUp/Dn and Up/Down Arrow keys toggled? */

struct buffer *kill_buf;	/* kill buffer */

char stringarg[STRMAX];	/* last string entered */
char filearg[FNAMEMAX];	/* last filename entered */

int float_col;		/* where next floating menu goes */
int float_row;

int printer;		/* current printer */
#else
extern KEYCODE lastkey;
extern int lasttable;
extern KEYCODE key;
extern int table;
extern int uarg;
extern FLAG isuarg;
extern FLAG isrepeating;
extern FLAG doabort;
extern char togpgln;
extern struct buffer *kill_buf;
extern char stringarg[STRMAX];
extern char filearg[FNAMEMAX];
extern int float_col;
extern int float_row;
extern int printer;
#endif

	/* terminal abstraction */
#if defined(MAIN)
int tattrnorm;		/* normal attribute */
int tattrrev;		/* reverse attribute */
int tattrru;		/* reverse,under attribute */
int tattrunder;		/* underscore attribute */
int tcol;		/* current col */
int tcols;		/* maximum col */
int toffset;		/* horizontal offset */
int trow;		/* current row */
int trows;		/* maximum row */
char tprintrep[] = {	/* print representation */
"\2^@ \2^A \2^B \2^C \2^D \2^E \2^F \2^G \
\2^H \2^I \2^J \2^K \2^L \2^M \2^N \2^O \
\2^P \2^Q \2^R \2^S \2^T \2^U \2^V \2^W \
\2^X \2^Y \2^Z \2^[ \2^\\ \2^] \2^^ \2^_ \
\1   \1!  \1\"  \1#  \1$  \1%  \1&  \1'  \
\1(  \1)  \1*  \1+  \1,  \1-  \1.  \1/  \
\1" "0  \1" "1  \1" "2  \1" "3  \1" "4  \1" "5  \1" "6  \1" "7  \
\1" "8  \1" "9  \1:  \1;  \1<  \1=  \1>  \1?  \
\1@  \1A  \1B  \1C  \1D  \1E  \1F  \1G  \
\1H  \1I  \1J  \1K  \1L  \1M  \1N  \1O  \
\1P  \1Q  \1R  \1S  \1T  \1U  \1V  \1W  \
\1X  \1Y  \1Z  \1[  \1\\  \1]  \1^  \1_  \
\1`  \1a  \1b  \1c  \1d  \1e  \1f  \1g  \
\1h  \1i  \1j  \1k  \1l  \1m  \1n  \1o  \
\1p  \1q  \1r  \1s  \1t  \1u  \1v  \1w  \
\1x  \1y  \1z  \1{  \1|  \1}  \1~  \2^? \
\3~^@\3~^A\3~^B\3~^C\3~^D\3~^E\3~^F\3~^G\
\3~^H\3~^I\3~^J\3~^K\3~^L\3~^M\3~^N\3~^O\
\3~^P\3~^Q\3~^R\3~^S\3~^T\3~^U\3~^V\3~^W\
\3~^X\3~^Y\3~^Z\3~^[\3~^\\\3~^]\3~^^\3~^_\
\2~  \2~! \2~\" \2~# \2~$ \2~% \2~& \2~' \
\2~( \2~) \2~* \2~+ \2~, \2~- \2~. \2~/ \
\2~0 \2~1 \2~2 \2~3 \2~4 \2~5 \2~6 \2~7 \
\2~8 \2~9 \2~: \2~; \2~< \2~= \2~> \2~? \
\2~@ \2~A \2~B \2~C \2~D \2~E \2~F \2~G \
\2~H \2~I \2~J \2~K \2~L \2~M \2~N \2~O \
\2~P \2~Q \2~R \2~S \2~T \2~U \2~V \2~W \
\2~X \2~Y \2~Z \2~[ \2~\\ \2~] \2~^ \2~_ \
\2~` \2~a \2~b \2~c \2~d \2~e \2~f \2~g \
\2~h \2~i \2~j \2~k \2~l \2~m \2~n \2~o \
\2~p \2~q \2~r \2~s \2~t \2~u \2~v \2~w \
\2~x \2~y \2~z \2~{ \2~| \2~} \2~~ \3~^?" };
#else
extern int tattrnorm;
extern int tattrrev;
extern int tattrru;
extern int tattrunder;
extern int tcol;
extern int tcols;
extern int toffset;
extern int trow;
extern int trows;
extern char tprintrep[];
#endif

/* -------------------- Routines -------------------- */

	/* fbuf.c */

FLAG BInit(int swap_size);
void BFini(void);
struct buffer *BBufCreate(char *fname);
void BBufDelete(void);
struct buffer *BBufFind(char *name);
void BBufGoto(struct buffer *bptr);
void BBufUnmod(void);
void BCharChange(char new);
void BCharDelete(int amount);
FLAG BFileRead(void);
FLAG BFileWrite(void);
void BFlush(void);
char BGetChar(void);
char BGetCharAdv(void);
int BGetCol(void);
long BGetLength(struct buffer *bptr);
long BGetLocation(void);
FLAG BInsChar(char new);
void BInsSpaces(int amount);
FLAG BInsString(char *str);
void BInsTabSpaces(int amount);
void BInvalid(void);
FLAG BIsAfterMark(struct mark *mptr);
FLAG BIsAtMark(struct mark *mptr);
FLAG BIsBeforeMark(struct mark *mptr);
FLAG BIsEnd(void);
FLAG BIsFree(struct buffer *bptr);
FLAG BIsMod(struct buffer *bptr);
FLAG BIsStart(void);
void BMakeColB(int col);
void BMakeColF(int col);
struct buffer *BMarkBuf(struct mark *mptr);
struct mark *BMarkCreate(void);
void BMarkDelete(struct mark *mptr);
FLAG BMarkIsMod(struct mark *mptr);
void BMarkSetMod(struct mark *mptr);
struct mark *BMarkScreen(int row);
void BMarkSwap(struct mark *mptr);
void BMarkToPoint(struct mark *mptr);
void BMoveBy(int amount);
void BMoveToEnd(void);
void BMoveToStart(void);
int BPagesUsed(void);
int BPagesMax(void);
int BPagesSize(void);
void BPointToMark(struct mark *mptr);
void BPopState(void);
void BPushState(void);
void BRegCopy(struct mark *mptr, struct buffer *bptr);
void BRegDelete(struct mark *mptr);
FLAG BSearchB(char c1, char c2);
FLAG BSearchF(char c1, char c2);

	/* fcalc.c */

#if !defined(NOCALC)
void UInit(void);
void UCalc(void);
char *UDescr(int operation);
void UEnter(void);
char *UHelp(int operation);
int UNumOps(void);
void UPrintX(void);
#endif

	/* fchar.c */

void CCharB(void);
void CCharBD(void);
void CCharF(void);
void CCharFD(void);
void CCharTran(void);
void CLineA(void);
void CLineAED(void);
void CLineB(void);
void CLineE(void);
void CLineF(void);
void CLineFD(void);
void CScrnB(void);
void CScrnBOW(void);
void CScrnF(void);
void CScrnFOW(void);

	/* fdate.c */

void DCal(void);
void DMove(int amout);
void DNext(void);
void DNow(struct tm *tptr);
int DOW(long day);
void DPrev(void);
void DSetup(int weekstart);
long DToDayN(struct tm *tptr, int cal);

	/* fdisplay.c */

void DInit1(void);
void DInit2(void);
void DFini(void);
void DClear(int start, int stop);
void DEcho(char *msg);
void DEchoNM(char *msg);
void DError(int msgnum);
void DErrorStr(char *msg);
int DFont(int new, FLAG doincr);
void DIncrDisplay(void);
void DLeft(void);
void DLine(char *buf, int line);
void DMenu(int which, int *rows, int *cols);
void DMenuHelp(int menu, int entry);
void DMenuSetup(int *rows, int *cols, char *hot_chars, int menu, int first,
	int number);
void DModeFlags(void);
void DModeLine(void);
void DNewDisplay(void);
int DPrefLine(void);
void DRight(void);
void DRowChar(int which);
void DScreenRange(void);
void DShow(char where, char *str, FLAG hilit);
void DTogVisGray(void);
void DView(char *msg);
void DWindGrow(void);
int DWindHeight(void);
void DWindOne(void);
void DWindSwap(void);
void DWindTog(void);
void DWindTwo(void);
void DWindTwoO(void);

	/* ffile.c */

void FBufBeg(void);
void FBufDel(void);
void FBufEnd(void);
void FBufList(void);
void FBufNext(void);
void FBufPrev(void);
void FBufPrint(void);
void FCommand(void);
void FDIRED(void);
FLAG FDIREDDo(char *fname, FLAG longdisp, FLAG iscompl);
void FFileDelete(void);
void FFileFind(void);
void FFileFixName(char *fname);
void FFileGetFile(char *fname, char *fullname);
void FFileGetDir(char *dname, char *fullname);
void FFileIns(void);
void FFilePath(void);
void FFileRead(void);
void FFileSave(void);
void FFileWrite(void);
void FGetDir(char *buf);
FLAG FMakeSys(char *name, FLAG erase);
int FPathOpen(char *name, char *actualname);
void FPrintSel(void);
void FSaveAll(void);
void FToDIRED(void);


	/* fjaguar.c */

#if defined(MSDOS)
void JInit(void);
void JFini(void);
void JDisEnd(void);
void JDisStart(void);
KEYCODE JGetKey(void);
char JIsKey(void);

#if defined(SYSMGR)
unsigned JAlloc(unsigned amt);
void JCopy(void);
void JCut(void);
void JFree(unsigned what);
void JGetDate(int *year, int *mon, int *day, int *hour, int *min, int *sec);
void JGetDir(char *dname);
FLAG JGetFile(int promptnum, char *fname);
void JNoFini(void);
void JPaste(void);
void JSystem(char *cmd_line);
#endif
#endif

	/* fkey.c */

int KAsk(int msgnum);
void KBegMac(void);
FLAG KDelayPrompt(int msgnum);
void KColon(void);
void KEndMac(void);
void KFromBuf(struct buffer *bptr);
void KFromMac(void);
void KFromStr(char *str, int len);
KEYCODE KGetChar(void);
int KGetFile(int msgnum, char *fname, FLAG useDIRED);
int KGetStr(int msgnum, char *str, int len);
int KGetStr2(char *msg, char *str, int len, FLAG isfile);
char KIsKey(void);
void KLoadMac(void);
void KMacDo(int table, int offset);
char KMacIs(void);
KEYCODE *KMacPtr(void);
void KMacRec(KEYCODE key);
void KMenu(int which);
void KMenuDo(int menu, int entry);
void KMenuMac(void);
void KPush(KEYCODE key);
FLAG KUArg(int targ);

	/* fmisc.c */

int ConvLower(int chr);
int ConvUpper(int chr);
void GoToGrayB(void);
void GoToGrayF(void);
void GoToNotGrayB(void);
void GoToNotGrayF(void);
FLAG IsGray(void);
FLAG IsNL(void);
FLAG IsWhite(void);
void MAbort(void);
void MAbout(void);
void MApropos(void);
#if defined(MSDOS)
void MAttr(void);
#endif
void MCharAt(void);
void MCtrlX(void);
void MDate(void);
void MDescBind(void);
void MDescKey(void);
void MDoKey(void);
void MExit(void);
void MFont(void);
void MIns8(void);
void MInsChar(void);
void MInsPgBk(void);
void MMacEval(void);
void MMakeDelete(void);
void MNotImpl(void);
void MMenu(void);
void MMenuH(void);
void MMenuM(void);
void MMeta(void);
void MQuote(void);
void MReplace(void);
void MReplaceQ(void);
void MSearchB(void);
void MSearchF(void);
void MTime(void);
void MTogPgLn(void);
void MUArg(void);
void MovePastB(FLAG (*pred)());
void MovePastF(FLAG (*pred)());
void MoveToB(FLAG (*pred)());
void MoveToF(FLAG (*pred)());
FLAG SearchNLB(void);
FLAG SearchNLF(void);

	/* fregion.c */

void RDelWhite(void);
void RHardToSoft(void);
void RIndent(void);
void RKillToMark(struct mark * mptr, FLAG isforward);
void RLower(void);
void RMarkSet(void);
void RMarkSwap(void);
void RMeta(void);
void ROutdent(void);
void RPrint(void);
void RRegCopy(void);
void RRegDelete(void);
void RRegFill(void);
void RSave(void);
void RSoftToHard(void);
void RTabify(void);
void RUntabify(void);
void RUpper(void);
void RYank(void);

	/* ftable.c */

char *TabDescr(KEYCODE key, int table);
void TabDispatch(KEYCODE key, int table);
char *TabHelp(KEYCODE key, int table);
int TabTable(KEYCODE key, int table);
FLAG TabIsDelete(KEYCODE key, int table);
FLAG TabIsVMove(KEYCODE key, int table);
int TabNumFunc(void);

	/* fterm.c */

#define TAttrNorm()	(tattrnorm)
#define TAttrRev()	(tattrrev)
#define TAttrRU()	(tattrru)
#define TAttrUnder()	(tattrunder)
#define TGetCol()	(tcol)
#define TGetOffset()	(toffset)
#define TGetRow()	(trow)
#define TGetWidth(c)	(tprintrep[(c & 0xFF) * 4])
#define TGetTabWidth(col) (cbuf->c.tab_spacing - ((col) % cbuf->c.tab_spacing))
#define TMaxCol()	(tcols)
#define TMaxRow()	(trows)

FLAG TInit(void);
void TFini(void);
void TAttrBlock(char *data, int count, FLAG doclear, int clearattr);
void TBell(void);
void TCLEOL(void);
void TClrScreen(void);
void TCurOff(void);
void TCurOn(void);
void TForce(void);
KEYCODE TGetKey(void);
void THiOff(void);
void THiOn(void);
FLAG TIsKey(void);
char *TPrintChar(char c);
void TPutChar(char c);
char *TPutCharBuf(char c, int col);
void TPutStr(char *str);
void TSetOffset(int col, int offset);
void TSetPoint(int row, int column);

	/* fwhite.c */

void WDelFWhite(void);
void WDelWhite(void);
void WFixup(struct conf_buffer *cptr);
void WIndNext(void);
void WIndDel(void);
void WIndThis(void);
void WInsNL(void);
void WInsNLA(void);
void WJoinGray(void);
void WLineCenter(void);
void WPrintLine(void);
void WPrintMar(void);
void WPrintPos(void);
void WSetFill(void);
void WSetLeft(void);
void WSetRight(void);
void WSetTabs(void);

	/* fword.c */

void WCaseRotate(void);
void WNumB(void);
void WNumF(void);
void WNumMark(void);
void WParaB(void);
void WParaF(void);
void WParaFill(void);
void WParaMark(void);
void WSentB(void);
void WSentBD(void);
void WSentF(void);
void WSentFD(void);
void WWordB(void);
void WWordBD(void);
void WWordCap(void);
void WWordF(void);
void WWordFD(void);
void WWordLow(void);
void WWordTran(void);
void WWordUp(void);
void WWrap(void);

/* end of FREYJA.H -- Freyja Header File */
