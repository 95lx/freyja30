/* MEMUTIL.H -- MemUtil Header File

	Written October 1992 by Craig Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "lib.h"

/* ---------- Application Control Block ---------- */

	/* Also called the "task" control block, defined by the
	Jaguar operating system */

#define NUMAPPS		17
#define DEFAULTAPP	9
#define APPSEGSIZE	4096	/* default size of application data segment
				(in paragraphs) */

struct acb {
	unsigned sp;		/* stack pointer at last switchout */
	unsigned ss;		/* stack segment at last switchout */
	unsigned imagev_off;	/* imagevector offset */
	unsigned imagev_seg;	/* imagevector segment */
	unsigned ds;		/* saved DS */
	unsigned mem_seg;	/* base segment of allocation */
	unsigned hot_key;	/* current hot key */
	char membank[6];	/* memory mapping information */
	char chipsel[6];	/* chip selection array */
	char state;		/* current state */
	char flags;		/* is 123 and other flags */
	unsigned rsrc_seg;	/* ? */
	char nowait;		/* is task just testing for keys? */
	char filler;
	char name[12];		/* task name as presented */
	char filler2[4];
	};

/* -------------------- Memory Chains -------------------- */

#define NUMLINKS	120	/* maximum number of links */

struct mem_chain {
	unsigned seg;	/* starting segment */
	unsigned size;	/* size in paragraphs */
	unsigned owner;	/* owner */
	char name[FNAMEMAX];	/* owner's name */
	};

/* -------------------- Globals -------------------- */

#define HEIGHT	12	/* number of useful lines in display */

/* -------------------- Routines -------------------- */

	/* MEMAPP.C */

int GetApp(struct acb *a);

	/* MEMCHAIN.C */

int GetChain(struct mem_chain *links, int number);

	/* MEMHELP.C */

char *Help_Line(int num);
int Help_Num(void);
void Help_Setup(void);

/* end of MEMUTIL.H -- MemUtil Header File */
