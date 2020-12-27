/* MEMCHAIN.C -- Get Memory Chain Routines

	Written April 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "memutil.h"
#undef unlink
#include <dos.h>

	/* define for Program Segment Prefix */
struct psp {
	unsigned int20;
	unsigned top;
	char reserved1[6];
	char terminate[4];
	char ctrlbreak[4];
	char criterror[4];
	char reserved2[22];
	unsigned environ;
	};	

	/* DOS memory block header */
struct block {
	char type;		/* 4D=more, 5A=end */
	unsigned owner;
	unsigned size;
	char filler[11];
	};


/* ------------------------------------------------------------ */

/* Get at most NUMBER links of memory chains.  Return the number of
links gotten. */

int
GetChain(struct mem_chain *links, int number)
	{
	struct psp psp;
	struct block b;
	struct block envb;	/* for getting the environment string */
	int cnt;
	int amt;
	char *cptr;
	char environ[10240];
	char far *fcptr;
	char huge *hcptr;
	unsigned huge *huptr;
	unsigned seg;

/* find list-of-lists */

	ListOfLists();

/* get first entry */

	loloff -= 2;		/* point to first entry, avoide huge math */
	fcptr = MK_FP(lolseg, loloff);
	hcptr = (char huge *)fcptr;
	huptr = (unsigned huge *)hcptr;
	seg = *huptr;

	for (cnt = 0; cnt < number; cnt++, links++) {

	/* get the block */

		BlockGet((char *)&b, sizeof(b), seg);
		links->seg = seg;
		links->size = b.size;
		links->owner = b.owner;

	/* get the possible PSP */

		BlockGet((char *)&psp, sizeof(psp), b.owner);

	/* check out the owner information, and turn into a name */

		if (b.owner == 0) {
			xstrcpy(links->name, "[free]");
			}
		else if (psp.int20 != 0x20cd || psp.environ <= 1) {
			xstrcpy(links->name, "[MS/DOS]");
			}
		else	{

		/* get the environment block */

			BlockGet((char *)&envb, sizeof(envb), psp.environ - 1);

		/* get the environment */

			amt = min(sizeof(environ), envb.size * 16);
			BlockGet(environ, amt, psp.environ);

		/* skip over the environment strings */

			for (cptr = environ;
				*cptr != NUL && cptr < &environ[amt];
				cptr += strlen(cptr) + 1) ;

			if (cptr < &environ[amt] - 4 && *(cptr + 1) == 1 &&
				 *(cptr + 2) == 0) {
				xstrncpy(links->name, cptr + 3);
				}
			else	{
				xstrcpy(links->name, "[program]");
				}
			}

	/* next block */

		if (seg + b.size + 1 < seg || seg + b.size > 0xA000) {
			strcat(links->name, "[corrupt chain]");
			return(cnt);
			}
		seg += b.size + 1;
		if (b.type == 0x5A) break;
		}
	return(cnt);
	}


/* end of MEMCHAIN.C -- Get Memory Chain Routines */
