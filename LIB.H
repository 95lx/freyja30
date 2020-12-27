/* LIB.H -- Main Library Header File

	Written October 1992 by Craig A. Finseth
	Copyright 1985,6,7,8,9,90,1,2,3 by Craig A. Finseth
*/

	/* operating system -- define only one */

/* #define VARARGS		1	/* supports varargs */

/* #define BERK43		1 */
/* #define SUNOS4		1 */
/* #define SUNOS3		1 */
/* #define UNICOS		1 */
/* #define UNIXV		1 */
#define MSDOS		1

#if defined(SUNOS4)
#define SUNOS3		1
#endif

#if defined(SUNOS3)
#define BERK43		1
#define UNIX		1
#endif

#if defined(BERK43)
#define UNIX		1
#endif

#if defined(UNICOS)
#define UNIXV		1
#define UNIX		1
#endif

#if defined(BERK43) || defined(UNIXV)
#define UNIX		1
#endif

/* -------------------- General Includes -------------------- */

#if !defined(MSDOS)
#include <sys/types.h>
#include <sys/file.h>
#include <time.h>
time_t time();
struct tm *localtime();
#if defined(UNIXV)
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#endif
#endif
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#if defined(VARARGS)
#if defined(__STDC__)
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#endif

#if defined(UNIXV)
#define L_SET			0
#endif

#if defined(UNIX)
char *getenv();
#endif

#if !defined(MSDOS)
/* from <netinet/if_ether.h>, but leave out all the other junk */
struct ether_addr {
	u_char	ether_addr_octet[6];
};
#endif
#if defined(isalpha)
#undef isalpha
#endif
#if defined(isdigit)
#undef isdigit
#endif
#if defined(islower)
#undef islower
#endif
#if defined(isupper)
#undef isupper
#endif
#if defined(isxdigit)
#undef isxdigit
#endif

/* -------------------- Configuration Constants -------------------- */

	/* types */

#define FLAG		char

	/* constants */

#ifndef NULL
#define NULL		((void *) 0)
#endif

#define TRUE		1
#define FALSE		0

#if !defined(MSDOS)
#define MAXINT		2147483647
#define MININT		(-2147483648)
#define MAXUNS		4294967295
#endif

#define NUL		'\0'
#define ZCB		'\02'		/* ^B */
#define ZCF		'\06'		/* ^F */
#define BEL		'\07'
#define BS		'\010'
#define TAB		'\011'
#define LF		'\012'
#define NL		'\n'
#define ZCK		'\013'		/* ^K */
#define FF		'\014'
#define CR		'\015'
#define ZCN		'\016'		/* ^N */
#define ZCP		'\020'		/* ^P */
#define ZCQ		'\021'		/* ^Q */
#define ZCU		'\025'		/* ^U */
#define ZCX		'\030'		/* ^X */
#define ZCY		'\031'		/* ^Y */
#define ZCZ		'\032'
#define ESC		'\033'
#define SNL		'\037'		/* "soft" newline, ^_ */
#define SP		' '
#define DEL		'\177'

#define MSTEXTEOF	(ZCZ)

#define NEWLINE		"\n"

#if defined(MSDOS)
#define BUFFSIZE	1024		/* good buffer size to use */
#define FNAMEMAX	81		/* size of file name */
#define TOKENMAX	768		/* size of a general token */
#define INPMAX		256		/* size of user input buffer */
#else
#define BUFFSIZE	(24 * 1024)	/* good buffer size to use */
#define FNAMEMAX	1025		/* size of file name */
#define TOKENMAX	1025		/* size of a general token */
#define INPMAX		256		/* size of user input buffer */
#endif

/* Please don't changee the following or the references to it. */
#define COPY		"Version 2.3.  Copyright 1992,3,4 by Craig A. Finseth.\n\
1343 Lafond, St Paul MN 55104, USA\n\
+1 612 644 4027\n\
fin@unet.umn.edu\n\
Craig.Finseth@mr.net\n"

/* -------------------- Globals -------------------- */

#if defined(MAIN)
unsigned appseg;	/* returned by findstr */
unsigned appoff;

unsigned lolseg;	/* returned by get list of lists routine */
unsigned loloff;
#else
extern unsigned appseg;
extern unsigned appoff;

extern unsigned lolseg;
extern unsigned loloff;
#endif

/* -------------------- Routines -------------------- */

	/* LIB.C */

#if defined(MSDOS)
#define close(fd)		PSystem(0x3E,0,0,fd)
#define creat(name,perm)	PSystem(0x3C,name,0)
#define open(name,mode)		PSystem(0x3D,name,0,0,mode)
#define lseek(fd,dist,mode)	lseeka(fd,dist,mode)
#define read(fd,buff,amt)	PSystem(0x3F,buff,amt,fd)
#define rename(old,new)		PSystem(0x56,old,0,0,0,0,new)
#define tell(fd)		tellc(fd)
#define unlink(name)		PSystem(0x41,name)
#define write(fd,buff,amt)	PSystem(0x40,buff,amt,fd)

#define O_RDONLY		0
#define O_WRONLY		1
#define O_RDWR			2
#endif

#if defined(UNIXV)
#define memmove(d,s,l)	memcpy(d,s,l)	/* ANSI C compatibility */
#endif
#ifdef BERK43
#define memmove(d,s,l)	bcopy(s,d,l)	/* ANSI C compatibility */
#endif
#define xstrncpy(d,s)		{ strncpy((d),(s),sizeof(d)); (d)[sizeof(d)-1]=NUL; }

int max();			/* int a, int b */
int min();			/* int a, int b */
FLAG PSetExt();			/* char *name, char *ext, FLAG force */
char *sfindin();		/* char *str, char *any */
char *sfindnotin();		/* char *str, char *any */
char *sindex();			/* char *str, char chr */
FLAG SToAny();			/* char *str, int *n */
FLAG SToN();			/* char *str, int *n, int base */
FLAG strequ();			/* char *a, char *b */
FLAG strnequ();			/* char *a, char *b, int len */
char *TrimNL();			/* char *str */
FLAG xisalnum();		/* int c */
FLAG xisalpha();		/* int c */
FLAG xisdigit();		/* int c */
FLAG xisgray();			/* char c */
FLAG xislower();		/* int c */
FLAG xisupper();		/* int c */
FLAG xiswhite();		/* char c */
FLAG xisxdigit();		/* int c */
void xstrcpy();			/* char *dest, char *src */
int xtolower();			/* int c */
int xtoupper();			/* int c */

	/* LIBASM.ASM */

#if defined(MSDOS)
int BGetKey(void);
int BGetKeyE(void);
int BIsKey(void);
int BIsKeyE(void);
void BlockGet(char *dest, int len, unsigned fromseg);
int c_service(unsigned int,...);
void far *c_service_far_ptr(unsigned int,...);
void DOSGetDir(int drive, char *buf);
FLAG find_str(char *str, unsigned seg, unsigned off);
unsigned GetSysDS(void);
void JFiniA(void);
void JInitA(void);
int JGetType(void);
void JLightOff(void);
void JLightOn(void);
void ListOfLists(void);
long lseeka(int fd, long dist, int mode);
void PageGet(char *to, char huge *from, int len);
void PagePut(char huge *to, char *from, int len);
int PSystem(int AH, ...); /* int DX, int CX, int BX, int AL, int SI, int DI */
unsigned SwapAlloc(unsigned para);
void SwapFree(unsigned para);
int VidInit(int mode, int zoom, int curcnt);
void VidFini(int mode, int zoom);
void VidBell(void);
void VidChar(char chr, int attrib);
void VidClear(int count, int attrib);
void VidCursor(int row, int col);
void VidCurOff(void);
void VidCurOn(void);
#endif

	/* LIBPARSE.C */

#define PARSELENGTH	1025	/* recommended parse buffer size */
#define PARSETOKENS	32	/* recommended number of tokens */

struct parse_data {
	char *p_buffer;		/* pointer to user-supplied buffer */
	int  p_length;		/* length of user-supplied buffer, */
				/* 1>length of the longest expected line */
	char p_separator;	/* separator character */
	char p_comment;		/* line comment start (ends at newline) */
	FLAG p_trim;		/* trim surrounding whitespace */
	/* internal use only -- don't set or look at these */
	char p_fname[FNAMEMAX];	/* file name */
	FILE *p_fptr;		/* file pointer */
	int  p_line;		/* current line number */
	};

int Parse_Open();		/* struct parse_data *pptr, char *fname */
void Parse_Close();		/* struct parse_data *pptr */
int Parse_A_Line();		/* struct parse_data *pptr, char *tokens[],
				int num_tokens */
int Parse_A_String();		/* struct parse_data *pptr, char *tokens[],
				int num_tokens, char *str */
char *Parse_File();		/* struct parse_data *pptr */
int Parse_Get_Line();		/* struct parse_data *pptr, char *buf,
				int bufsize */
int Parse_Line();		/* struct parse_data *pptr */

	/* LIBREADE.C */

enum ReadType {
	READNONE,		/* nothing or read error */
	READEOF,		/* eof */
	READSTR,		/* quoted string */
	READTOK,		/* token */
	};

FLAG Read_Init();		/* const char *file */
void Read_Fini();		/* void */
void Read_Comment();		/* char c */
char *Read_File();		/* void */
int Read_Line();		/* void */
void Read_Pipe();		/* FILE *fptr */
FLAG Read_Push();		/* char *file */
enum ReadType Read_Token();	/* char *buf */
void Read_Untok();		/* char *buf */

	/* LIBRES.C */

#define RESTABLES	48	/* number of tables */
#define RESMAXENTRIES	256	/* maximum number of entries */
#define RESMAXSIZE	16384	/* maximum size in bytes */
#define RESSIG		0xFA
#define RESVERSION	1
#define RESTABSTART	2	/* offset of num_tables */

struct resint {
	unsigned char low;
	unsigned char high;
	};

struct res_header {
	unsigned char significator;	/* it is a Freyja resource file */
	unsigned char version;		/* version number */
	struct resint num_tables;
	struct resint tables[RESTABLES + 1];
	};

int Res_Load();			/* char *fname */
unsigned char *Res_Buf();	/* void */
unsigned char Res_Char();	/* int table, int offset */
int Res_Get();			/* int *sizeptr, int table, int offset */
int Res_KeySeq();		/* int *buf, int buflen, int table, int off */
int Res_Number();		/* int table, int offset */
unsigned char *Res_String();	/* int *length, int table, int offset */

	/* XPRINTF.C */

void xprintf_set_text();	/* FLAG istext */
FLAG xprintf_get_text();	/* void */
#if defined(VARARGS)
#if defined(__STDC__)
void xprintf(char *fmt, ...);
void xdprintf(int fd, char *fmt, ...);
void xeprintf(char *fmt, ...);
void xsprintf(char *string, char *fmt, ...);
#else
void xprintf();			/* char *fmt, ... */
void xdprintf();		/* int fd, char *fmt, ... */
void xeprintf();		/* char *fmt, ... */
void xsprintf();		/* char *string, char *fmt, ... */
#endif
#endif

/* end of LIB.H -- Main Library Header File */
