/* FBUF.C -- Buffer Managment

	Written March 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"

#if defined(UNIX)
#include <sys/types.h>
#include <sys/stat.h>
#endif

struct phys_page {
	char page_data[PAGESIZE];	/* the page */
	struct phys_page *next;		/* LRU chain */
	struct phys_page *prev;
	char page_loc;			/* logical location of page */
	int page_num;
	FLAG is_mod;			/* is the page dirty? */
	struct virt_page *pptr;		/* page description for this page */
	};


	/* cached information on the current page */
static struct virt_page *cur_page;	/* virtual page */
static int cur_pt_offset;		/* point offset */
static int cur_page_len;		/* page length */
static char *cur_cptr;			/* actual character */
static char *cur_page_start;		/* start of the page */
static char *cur_gap_start;		/* position of the gap */
static char *cur_gap_end;
static FLAG cur_is_mod;			/* is modified (dirty) */
static int cur_col = -1;		/* current column position */

static int savecol;
struct mark *getmark;

static struct virt_page vp[SWAPMAX];

static struct mark marks[MAXMARK];
#define MAXSMARK	(sizeof(screenmarks) / sizeof(screenmarks[0]))
static struct mark screenmarks[ROWMAX + 2];

#if defined(MSDOS)
static unsigned swap_base;
#else
static char *swap_base;
#endif

static struct phys_page pages[NUM_PHYS_PAGES];	/* page descriptors */

static struct phys_page *first_phys_page;  /* LRU chain of pages in memory */
static struct phys_page *last_phys_page;

#define SWAPMAPSIZE	((long)SWAPMAX * 1024 / PAGESIZE)
static char swap_map[SWAPMAPSIZE];	/* swap file in use flags */
static int num_swap;			/* number of swap file pages */



void B_FileReadCheck(void);
void B_GetGap(void);
int B_IndexB(char *array, int len, char chr);
int B_IndexF(char *array, int len, char chr);
FLAG B_InsChar(char new);
void B_MarkUpdate(struct virt_page *new_page, int amt, int new_offset);
void B_MarkUpdateNP(struct virt_page *new_page, int new_offset);
void B_MovePointTo(int dist);
struct virt_page * B_PageAllocate(struct buffer *bptr, struct virt_page *prev,
	struct virt_page *next);
struct phys_page *B_PageFindFree(void);
void B_PageFree(struct buffer *bptr, struct virt_page *pptr);
FLAG B_PageSplit(int amount);
void B_PageToMemory(struct virt_page *pptr);
void B_PageToCurrent(struct virt_page *pptr);
void B_RulerLine(void);
void B_SetScreenMarks(FLAG newflag);
void B_SetScreenMarks2(struct window *wptr, FLAG newflag);

/* ------------------------------------------------------------ */

/* Initialize the buffer abstraction.  Return TRUE on success or FALSE
on failure. */

FLAG
BInit(int swap_size)
	{
	struct phys_page *pptr;
	int cnt;

	if (swap_size > SWAPMAX) swap_size = SWAPMAX;

	cbuf = NULL;
	cur_page = NULL;
	for (pptr = pages; pptr < &pages[NUM_PHYS_PAGES]; ++pptr) {
		pptr->page_loc = 'M';
		pptr->is_mod = FALSE;
		pptr->next = pptr + 1;
		pptr->prev = pptr - 1;
		}
	first_phys_page = pages;
	first_phys_page->prev = NULL;
	last_phys_page = pptr - 1;
	last_phys_page->next = NULL;

	getmark = BMarkCreate();

	for (cnt = 0; cnt < MAXMARK; cnt++) marks[cnt].pptr = NULL;
	for (cnt = 0; cnt < MAXSMARK; cnt++) screenmarks[cnt].pptr = NULL;
	for (cnt = 0; cnt < NUMBUFFERS; cnt++) buffers[cnt].num_pages = 0;

	for (cnt = 0; cnt < SWAPMAPSIZE; cnt++) swap_map[cnt] = 0;
	num_swap = ((long)swap_size * 1024) / PAGESIZE;

#if defined(MSDOS)
	/* 64 = 1024 / 16 */
#if defined(SYSMGR)
	swap_base = JAlloc(swap_size * 64);
#else
	swap_base = SwapAlloc(swap_size * 64);
#endif
	return(swap_base != 0);
#endif
#if defined(UNIX)
	swap_base = (char *)malloc(swap_size * 1024);
	return(swap_base != NULL);
#endif
	}


/* ------------------------------------------------------------ */

/* Terminate the buffer abstraction. */

void
BFini(void)
	{
#if defined(MSDOS)
#if defined(SYSMGR)
	JFree(swap_base);
#else
	SwapFree(swap_base);
#endif
#endif
#if defined(UNIX)
	free(swap_base);
#endif
	}


/* ------------------------------------------------------------ */

/* Create a buffer. Returns a pointer to the buffer descriptor or NULL. */

struct buffer *
BBufCreate(char *fname)
	{
	struct buffer *bptr;

	if (cbuf == NULL) {
		for (bptr = buffers; bptr < &buffers[NUMBUFFERS]; bptr++) {
			if (BIsFree(bptr)) break;
			}
		if (bptr >= &buffers[NUMBUFFERS]) {
			DError(RES_NOFREE);
			return(NULL);
			}
		}
	else	{
		for (bptr = cbuf + 1; bptr != cbuf; bptr++) {
			if (bptr > &buffers[NUMBUFFERS]) bptr = buffers;
			if (BIsFree(bptr)) break;
			}
		if (bptr == cbuf) {
			DError(RES_NOFREE);
			return(NULL);
			}
		}

		/* copy parameters from previous buffer */
	bptr->c = (cbuf == NULL) ? c.d : cbuf->c;

	xstrcpy(bptr->fname, fname);
	bptr->num_pages = 0;
	bptr->is_mod = FALSE;
	bptr->point_offset = 0;
	bptr->mptr = NULL;

	if ((bptr->point_page = B_PageAllocate(bptr, NULL, NULL)) == NULL)
		return(NULL);
	B_PageToMemory(bptr->point_page);

	BBufGoto(bptr);
	bptr->mptr = BMarkCreate();
	mark = bptr->mptr;
	return(bptr);
	}


/* ------------------------------------------------------------ */

/* Delete the buffer and all associated pages.  If no non-system
buffers are left, re-create scratch. */

void
BBufDelete(void)
	{
	struct buffer *bptr;

	if (strequ(cbuf->fname, Res_String(NULL, RES_MSGS, RES_SYSKILL))) {
		DError(RES_NODELKILL);
		return;
		}
	while (cbuf->first != NULL) B_PageFree(cbuf, cbuf->first);
	cbuf->num_pages = 0;
	cur_page = NULL;
	savecol = -1;
	BMarkDelete(cbuf->mptr);

/* go to the previous buffer */
	for (bptr = cbuf; 1; ) {
		if (bptr == buffers) bptr = &buffers[NUMBUFFERS];
		bptr--;
		if (bptr == cbuf) break;
		if (BIsFree(bptr)) continue;
		if (c.g.skip_system && !isuarg && IS_SYS(bptr->fname)) continue;
		cbuf = NULL;
		BBufGoto(bptr);
		return;
		}
	cbuf = NULL;
	BBufCreate(Res_String(NULL, RES_MSGS, RES_SYSSCRATCH));
	}


/* ------------------------------------------------------------ */

/* Locate a buffer with a buffer name of exactly NAME (preferred) or
starting with NAME (backup). */

struct buffer *
BBufFind(char *name)
	{
	struct buffer *bptr;

	for (bptr = cbuf + 1; 1; bptr++) {
		if (bptr >= &buffers[NUMBUFFERS]) bptr = buffers;
		if (!BIsFree(bptr) && strequ(name, bptr->fname)) return(bptr);
		if (bptr == cbuf) break;
		}

	for (bptr = cbuf + 1; 1; bptr++) {
		if (bptr >= &buffers[NUMBUFFERS]) bptr = buffers;
		if (!BIsFree(bptr) && strnequ(name, bptr->fname, strlen(name)))
			return(bptr);
		if (bptr == cbuf) break;
		}
	return(NULL);
	}


/* ------------------------------------------------------------ */

/* Switch to the specified buffer. */

void
BBufGoto(struct buffer *bptr)
	{
	if (bptr == NULL || bptr->num_pages == 0) {
		DError(RES_NOTBUFF);
		return;
		}
	if (cbuf != NULL) {
		cbuf->point_page = cur_page;
		cbuf->point_offset = cur_pt_offset;
		}
	B_PageToCurrent(bptr->point_page);
	B_MovePointTo(bptr->point_offset);
	cbuf = bptr;
	cur_col = -1;
	mark = cbuf->mptr;
	}


/* ------------------------------------------------------------ */

/* Clear the current buffer's modified flag. */

void
BBufUnmod(void)
	{
	cbuf->is_mod = FALSE;
	}


/* ------------------------------------------------------------ */

/* Change the current character to TO. */

void
BCharChange(char to)
	{
	if (!BIsEnd()) {
		*cur_cptr = to;
		cbuf->is_mod = TRUE;
		cur_is_mod = TRUE;
		B_SetScreenMarks(FALSE);
		BMoveBy(1);
		}
	else	BInsChar(to);
	}


/* ------------------------------------------------------------ */

/* Delete AMOUNT characters.  If AMOUNT is negative, delete backwards. */

void
BCharDelete(int amount)
	{
	int num = amount;
	int tmp;
	struct mark *mptr;
	struct virt_page *vpage;

	if (amount == 0) return;
	B_GetGap();

	if (cur_pt_offset + amount > cur_page_len)
		num = cur_page_len - cur_pt_offset;
	if (num < 0) num = 0;

	cur_gap_end += num;
	cur_page_len -= num;
	if (cur_page == cbuf->last) amount = num;

	amount -= num;
	cbuf->is_mod = TRUE;
	cur_is_mod = TRUE;
	if (cur_page_len == 0 &&
		 (cur_page->next != NULL || cur_page->prev != NULL)) {
		vpage = cur_page->next;
		tmp = 0;
		if (vpage == NULL) {
			vpage = cur_page->prev;
			tmp = vpage->page_len;
			}
		B_MarkUpdateNP(vpage, tmp);
		B_PageFree(cbuf, cur_page);
		cur_page = NULL;
		}
	else	{
		vpage = cur_page;
		tmp = cur_pt_offset;
		if (tmp >= cur_page_len && cur_page->next) {
			vpage = cur_page->next;
			tmp = 0;
			}
		B_MarkUpdate(vpage, num, tmp);
		}
	B_PageToCurrent(vpage);
	B_MovePointTo(tmp);
	if (amount != 0)
		BCharDelete(amount);
	else	B_SetScreenMarks(TRUE);
	}


/* ------------------------------------------------------------ */

/* Load the buffer with the current file. */

FLAG
BFileRead(void)
	{
	int fd;
	int len;
	int chr;
	int amt;
	int col;
	long there;
	struct mark *mptr;
#if defined(UNIX)
	struct stat statbuf;
#endif
#if defined(MSDOS)
	FLAG cr_found = FALSE;
#endif
	FLAG ok = TRUE;

	B_PageToCurrent(cbuf->first);
	while (cur_page->next != NULL) B_PageFree(cbuf, cur_page->next);

	for (mptr = marks; mptr < &marks[MAXMARK]; ++mptr) {
		if (mptr->pptr && mptr->bptr == cbuf) {
			mptr->pptr = cur_page;
			mptr->mark_offset = 0;
			mptr->is_mod = TRUE;
			}
		}
	for (mptr = screenmarks; mptr < &screenmarks[MAXSMARK]; ++mptr) {
		if (mptr->pptr != NULL && mptr->bptr == cbuf) {
			mptr->pptr = cur_page;
			mptr->mark_offset = 0;
			mptr->is_mod = TRUE;
			}
		}

#if defined(MSDOS)
#define O_RDONLY	0	/* dummy */
	if ((fd = open(cbuf->fname, O_RDONLY)) < 0) {
#endif
#if defined(UNIX)
	if ((fd = open(cbuf->fname, O_RDONLY, 0)) < 0) {
#endif
		cur_gap_start = cur_page_start;
		cur_gap_end = cur_page_start + PAGESIZE;
		cur_page_len = 0;
		B_MovePointTo(0);
		cur_is_mod = FALSE;
		cbuf->is_mod = FALSE;
		cur_col = 0;
#if defined(UNIX)
		cbuf->file_time = 0;
#endif
		return(TRUE);
		}

	while ((len = read(fd, cur_page_start, PAGESIZE)) > 0) {
		cur_gap_start = cur_page_start + len;
		cur_gap_end = cur_page_start + PAGESIZE;
		cur_page_len = len;
		B_MovePointTo(0);
		cur_is_mod = TRUE;
#if defined(MSDOS)
		if (!isuarg && cr_found && *cur_cptr == LF) {
			*cur_page_start = NL;
			--(cur_page->prev->page_len);
			--(cur_page->prev->gap_start);
			}
		cr_found = FALSE;
		while (cur_pt_offset < cur_page_len) {
			if (!isuarg && *cur_cptr == CR) {
				B_MovePointTo(cur_pt_offset + 1);
				B_GetGap();
				if (cur_pt_offset >= cur_page_len)
					cr_found = TRUE;
				else if (*cur_cptr == LF) {
					--cur_gap_start;
					--cur_page_len;
					*cur_gap_end = NL;
					}
				B_MovePointTo(cur_pt_offset - 1);
				}
			B_MovePointTo(cur_pt_offset + 1);
			}
#else
		B_MovePointTo(cur_page_len);
#endif
		if (len < PAGESIZE) break;
		if (B_PageAllocate(cbuf, cur_page, NULL) == NULL) {
			ok = FALSE;
			break;
			}
		B_PageToCurrent(cur_page->next);
		B_MovePointTo(0);
		}
	if (len < 0) ok = FALSE;
#if defined(UNIX)
	fstat(fd, &statbuf);
	cbuf->file_time = statbuf.st_mtime;
#endif
	close(fd);
	if (cur_pt_offset == 0 && cur_page != cbuf->first) {
		B_PageFree(cbuf, cur_page);
		cur_page = NULL;
		}
	else	{
		B_GetGap();
		cur_gap_end = cur_page_start + PAGESIZE;
		cur_page_len = cur_pt_offset;
		}
	B_PageToCurrent(cbuf->first);
	B_MovePointTo(0);
	cur_col = 0;
	cbuf->is_mod = FALSE;
	if (cbuf->c.fill == 'W') {
		col = 0;
		while (!BIsEnd()) {
			chr = BGetCharAdv();
			if (chr == NL) {
				there = BGetLocation();
				col = 0;
				}
			else if (chr == TAB) {
				col += TGetTabWidth(col);
				}
			else if (chr == SP) {
				there = BGetLocation();
				col++;
				}
			else if (col >= cbuf->c.right_margin) {
				amt = BGetLocation() - there + 1;
				BMoveBy(-amt);
				BCharChange(SNL);
				BMoveBy(amt);
				col = amt;
				}
			else	{
				col++;
				}
			}
		BMoveToStart();
		}
	if (Res_Char(RES_CONF, RES_RULERCHECK) == 'Y') B_FileReadCheck();
	return(ok);
	}


/* ------------------------------------------------------------ */

/* Write the buffer to its file. */

FLAG
BFileWrite(void)
	{
	struct virt_page *vpage;
	int fd;
	int len;
	FLAG was_err;
	char *cptr;
	char buf[PAGESIZE];
	char bname[FNAMEMAX];
	char chr;
#if defined(UNIX)
	struct stat statbuf;
#endif

	if (Res_Char(RES_CONF, RES_BACKUP) == 'Y') {
		xstrcpy(bname, cbuf->fname);
		PSetExt(bname, "BAK", TRUE);
		unlink(bname);
		rename(cbuf->fname, bname);
		}

#if defined(MSDOS)
	if ((fd = creat(cbuf->fname, 0666)) < 0) return(FALSE);
#endif
#if defined(UNIX)
	if (stat(cbuf->fname, &statbuf) >= 0) {
		if ((cbuf->file_time != 0 &&
			 (cbuf->file_time != statbuf.st_mtime)) &&
			 !KAsk(RES_ASKOVER))
			return(FALSE);
		}
	if ((fd = open(cbuf->fname, O_WRONLY | O_TRUNC | O_CREAT, 0666)) < 0)
		return(FALSE);
#endif

	BMarkToPoint(cwin->point);
	cptr = buf;
	for (vpage = cbuf->first, was_err = FALSE;
		 vpage != NULL && !was_err; ) {
		B_PageToCurrent(vpage);
		B_MovePointTo(0);
		vpage = cur_page->next;
		while (cur_pt_offset < cur_page_len) {
			chr = *cur_cptr;
			if (chr == SNL) {
				*cptr++ = (cbuf->c.fill == 'W') ? SP : SNL;
				}
#if defined(MSDOS)
			else if (!isuarg && chr == NL) {
				*cptr++ = CR;
				if (cptr >= &buf[PAGESIZE]) {
					was_err = write(fd, buf, PAGESIZE)
						!= PAGESIZE;
					if (was_err) break;
					cptr = buf;
					}
				*cptr++ = LF;
				}
#endif
			else	{
				*cptr++ = chr;
				}
			if (cptr >= &buf[PAGESIZE]) {
				was_err = write(fd, buf, PAGESIZE) != PAGESIZE;
				if (was_err) break;
				cptr = buf;
				}
			B_MovePointTo(cur_pt_offset + 1);
			}
		}
	len = cptr - buf;
	if (was_err || len != write(fd, buf, len))
		DError(RES_ERRWRITE);
	else	cbuf->is_mod = FALSE;
#if defined(UNIX)
	fstat(fd,&statbuf);
	cbuf->file_time = statbuf.st_mtime;
#endif
	close(fd);
	BPointToMark(cwin->point);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Write out one modified page. */

void
BFlush(void)
	{
	struct phys_page *ppage;

	for (ppage = last_phys_page; ppage->prev != NULL &&
		(ppage->page_loc != 'S' || !ppage->is_mod);
		ppage = ppage->prev) ;
	if (ppage->prev == NULL) return;
	if (ppage->pptr->page_len != 0) {
#if defined(MSDOS)
#if defined(SYSMGR)
		char huge *tmp = (char huge *) (
			((long)swap_base << 16) +
			((long)_DS << 16) );
#else
		char huge *tmp = (char huge *)((long)swap_base << 16);
#endif
		PagePut(tmp + ((long)ppage->page_num) * PAGESIZE,
			ppage->page_data, PAGESIZE);
#endif
#if defined(UNIX)
		memmove(swap_base + ppage->page_num * PAGESIZE,
			ppage->page_data, PAGESIZE);
#endif
		}
	ppage->is_mod = FALSE;
	}


/* ------------------------------------------------------------ */

/* Return the current character. */

char
BGetChar(void)
	{
	return(*cur_cptr);
	}


/* ------------------------------------------------------------ */

/* Return the current character and move the point by 1 char.  Same as
c = BGetChar(); BMoveBy(1);, but optimized for speed. */

char
BGetCharAdv(void)
	{
	int num;
	char chr = *cur_cptr;

	if (cur_pt_offset >= cur_page_len) return(chr);
	if (chr == NL || chr == SNL)
		cur_col = 0;
	else if (cur_col >= 0) {
		cur_col += (chr == TAB) ? TGetTabWidth(cur_col) :
			TGetWidth(chr);
		}
	if (++cur_pt_offset < cur_page_len) {
		if (++cur_cptr == cur_gap_start) cur_cptr = cur_gap_end;
		return(chr);
		}
	--cur_pt_offset;

	num = cur_pt_offset + 1;
	if (num >= 0 && num < cur_page_len) {
		B_MovePointTo(num);
		return(chr);
		}
	if (num >= cur_page_len && cbuf->last == cur_page) {
		B_MovePointTo(cur_page_len);
		return(chr);
		}
	if (num < 0) {
		if (cur_page == cbuf->first) {
			B_MovePointTo(0);
			return(chr);
			}
		B_PageToCurrent(cur_page->prev);
		B_MovePointTo(cur_page_len);
		}
	else	{
		num -= cur_page_len;	/* must use this cur_page_len */
		B_PageToCurrent(cur_page->next);
		B_MovePointTo(0);
		}
	if (num != 0) BMoveBy(num);
	return(chr);
	}


/* ------------------------------------------------------------ */

/* Return the current column. */

int
BGetCol(void)
	{
	if (cur_col >= 0) return(cur_col);
	if (cur_pt_offset > cur_page_len) {
		cur_col = 0;
		return(0);
		}
	BMarkToPoint(getmark);
	if (BSearchB(NL, SNL)) BMoveBy(1);
	cur_col = 0;
	while (BIsBeforeMark(getmark)) BMoveBy(1);
	return(cur_col);
	}


/* ------------------------------------------------------------ */

/* Returns the buffer's length. */

long
BGetLength(struct buffer *bptr)
	{
	long len;
	struct virt_page *vpage;

	len = 0;
	for (vpage = bptr->first; vpage != NULL; vpage = vpage->next) {
		if (vpage == cur_page)
			len += cur_page_len;
		else	len += vpage->page_len;
		}
	return(len);
	}


/* ------------------------------------------------------------ */

/* Return the current point location. */

long
BGetLocation(void)
	{
	long len;
	struct virt_page *vpage;

	len = 0;
	for (vpage = cbuf->first; vpage != cur_page; vpage = vpage->next)
		len += vpage->page_len;
	return(len + cur_pt_offset);
	}


/* ------------------------------------------------------------ */

/* Insert the specified character into the buffer. Return True on
success or False on buffer full. */

FLAG
BInsChar(char chr)
	{
	FLAG ok;

	ok = B_InsChar(chr);
	B_SetScreenMarks(FALSE);
	return(ok);
	}


/* ------------------------------------------------------------ */

/* Insert AMT spaces. */

void
BInsSpaces(int amt)
	{
	while (amt-- > 0) B_InsChar(SP);
	B_SetScreenMarks(FALSE);
	}


/* ------------------------------------------------------------ */

/* Insert the string into the buffer. Return True on success or False
on buffer full. */

FLAG
BInsStr(char *str)
	{
	FLAG ok = TRUE;

	while (*str != NUL) {
		if (!B_InsChar(*str++)) {
			ok = FALSE;
			break;
			}
		}
	B_SetScreenMarks(FALSE);
	return(ok);
	}


/* ------------------------------------------------------------ */

/* Put in the right number of tabs and spaces */

void
BInsTabSpaces(int amt)
	{
	for ( ; amt >= cbuf->c.tab_spacing; amt -= cbuf->c.tab_spacing) {
		B_InsChar(TAB);
		}
	BInsSpaces(amt);
	}


/* ------------------------------------------------------------ */

/* Put the point in a special, repeatable invalid state off the end of
the buffer. */

void
BInvalid(void)
	{
	B_PageToCurrent(cbuf->last);
	B_MovePointTo(cur_page_len + 1);
	cur_col = -1;
	}


/* ------------------------------------------------------------ */

/* Is the point after the mark? */

FLAG
BIsAfterMark(struct mark *mptr)
	{
	struct virt_page *vpage;

	if (mptr->pptr == NULL || mptr->bptr != cbuf) {
		DError(RES_BADMARK);
		return(FALSE);
		}
	if (mptr->pptr == cur_page) return(cur_pt_offset > mptr->mark_offset);
	for (vpage = cur_page; vpage && vpage != mptr->pptr;
		vpage = vpage->prev);
	return(vpage != NULL);
	}


/* ------------------------------------------------------------ */

/* Is the point at the mark? */

FLAG
BIsAtMark(struct mark *mptr)
	{
	return(mptr->pptr == cur_page && mptr->mark_offset == cur_pt_offset);
	}


/* ------------------------------------------------------------ */

/* Is the point before the mark? */

FLAG
BIsBeforeMark(struct mark *mptr)
	{
	struct virt_page *vpage;

	if (mptr->pptr == NULL || mptr->bptr != cbuf) {
		DError(RES_BADMARK);
		return(FALSE);
		}
	if (mptr->pptr == cur_page) return(cur_pt_offset < mptr->mark_offset);
	for (vpage = cur_page; vpage != NULL && vpage != mptr->pptr;
		vpage = vpage->next);
	return(vpage != NULL);
	}


/* ------------------------------------------------------------ */

/* Is the point at the end of the buffer? */

FLAG
BIsEnd(void)
	{
	return(cur_pt_offset >= cur_page_len && cur_page == cbuf->last);
	}


/* ------------------------------------------------------------ */

/* Return True if the buffer is free, or False if it is in use. */

FLAG
BIsFree(struct buffer *bptr)
	{
	return(bptr->num_pages == 0);
	}


/* ------------------------------------------------------------ */

/* Has the buffer been modified? */

FLAG
BIsMod(struct buffer *bptr)
	{
	return(bptr->is_mod);
	}


/* ------------------------------------------------------------ */

/* Is the point at the beginning of the buffer? */

FLAG
BIsStart(void)
	{
	return(cur_pt_offset == 0 && cur_page == cbuf->first);
	}


/* ------------------------------------------------------------ */

/* Put the cursor in specific column, rounding. */

void
BMakeColB(int col)
	{
	int old_col;

	if (BSearchB(NL, SNL)) BMoveBy(1);
	cur_col = 0;
	while (cur_col < col && *cur_cptr != NL && *cur_cptr != SNL &&
		 !BIsEnd()) {
		old_col = cur_col;
		BMoveBy(1);
		}
	if (cur_col != 0 && cur_col - col > col - old_col) BMoveBy(-1);
	}


/* ------------------------------------------------------------ */

/* Put cursor in specific column, no rounding. */

void
BMakeColF(int col)
	{
	if (BSearchB(NL, SNL)) BMoveBy(1);
	cur_col = 0;
	while (cur_col < col && *cur_cptr != NL && *cur_cptr != SNL &&
		 !BIsEnd()) {
		BMoveBy(1);
		}
	}


/* ------------------------------------------------------------ */

/* Return the mark's buffer. */

struct buffer *
BMarkBuf(struct mark *mptr)
	{
	return(mptr->bptr);
	}


/* ------------------------------------------------------------ */

/* Create a mark at the point. */

struct mark *
BMarkCreate(void)
	{
	struct mark *mptr;

	for (mptr = marks; mptr < &marks[MAXMARK] && mptr->pptr != NULL;
		mptr++) ;
	if (mptr >= &marks[MAXMARK - 1]) {
		DError(RES_NOMARKS);
		mptr = &marks[MAXMARK - 1];
		}
	mptr->bptr = cbuf;
	mptr->pptr = cur_page;
	mptr->mark_offset = cur_pt_offset;
	return(mptr);
	}


/* ------------------------------------------------------------ */

/* Free the supplied mark. */

void
BMarkDelete(struct mark *mptr)
	{
	mptr->pptr = NULL;
	}


/* ------------------------------------------------------------ */

/* Test and clear the modified flag on the specified screen mark. */

FLAG
BMarkIsMod(struct mark *mptr)
	{
	FLAG is_mod;

	is_mod = mptr->is_mod;
	mptr->is_mod = FALSE;
	return(mptr->bptr != cbuf || is_mod);
	}


/* ------------------------------------------------------------ */

/* Set the modified flag on the specified screen mark. */

void
BMarkSetMod(struct mark *mptr)
	{
	mptr->is_mod = TRUE;
	}


/* ------------------------------------------------------------ */

/* Return the mark for the specifed row. */

struct mark *
BMarkScreen(int row)
	{
	return(&screenmarks[row]);
	}


/* ------------------------------------------------------------ */

/* Swap the point and the mark. */

void
BMarkSwap(struct mark *mptr)
	{
	struct mark tmp;

	tmp = *mptr;
	BMarkToPoint(mptr);
	BPointToMark(&tmp);
	}


/* ------------------------------------------------------------ */

/* Put the mark where the point is. */

void
BMarkToPoint(struct mark *mptr)
	{
	mptr->bptr = cbuf;
	mptr->pptr = cur_page;
	mptr->mark_offset = cur_pt_offset;
	}


/* ------------------------------------------------------------ */

/* Move the point relative to its current position by AMT characters.
*/

void
BMoveBy(int amt)
	{
	int num;

	if (amt == 1) {
		if (cur_pt_offset >= cur_page_len) return;
		if (*cur_cptr == NL || *cur_cptr == SNL)
			cur_col = 0;
		else if (cur_col >= 0) {
			cur_col += (*cur_cptr == TAB) ? TGetTabWidth(cur_col) :
				TGetWidth(*cur_cptr);
			}
		if (++cur_pt_offset < cur_page_len) {
			if (++cur_cptr == cur_gap_start)
				cur_cptr = cur_gap_end;
			return;
			}
		--cur_pt_offset;
		}
	else	cur_col = -1;

	num = cur_pt_offset + amt;
	if (num >= 0 && num < cur_page_len) {
		B_MovePointTo(num);
		return;
		}
	if (num >= cur_page_len && cbuf->last == cur_page) {
		B_MovePointTo(cur_page_len);
		return;
		}
	if (num < 0) {
		if (cur_page == cbuf->first) {
			B_MovePointTo(0);
			return;
			}
		B_PageToCurrent(cur_page->prev);
		B_MovePointTo(cur_page_len);
		}
	else	{
		num -= cur_page_len;	/* must use this cur_page_len */
		B_PageToCurrent(cur_page->next);
		B_MovePointTo(0);
		}

	if (num != 0) BMoveBy(num);
	}


/* ------------------------------------------------------------ */

/* Set the point to the end of the buffer. */

void
BMoveToEnd(void)
	{
	B_PageToCurrent(cbuf->last);
	B_MovePointTo(cur_page_len);
	cur_col = -1;
	}


/* ------------------------------------------------------------ */

/* Set the point to the start of the buffer. */

void
BMoveToStart(void)
	{
	B_PageToCurrent(cbuf->first);
	B_MovePointTo(0);
	cur_col = 0;
	}


/* ------------------------------------------------------------ */

/* Return the number of pages in use. */

int
BPagesUsed(void)
	{
	int cnt;
	int num = 0;

	for (cnt = 0; cnt < num_swap; ) {
		if (swap_map[cnt++]) num++;
		}
	return(num);
	}


/* ------------------------------------------------------------ */

/* Return the number of pages in the swap area. */

int
BPagesMax(void)
	{
	return(num_swap);
	}


/* ------------------------------------------------------------ */

/* Return the size of a page, in KBytes. */

int
BPagesSize(void)
	{
	return(PAGESIZE / 1024);
	}


/* ------------------------------------------------------------ */

/* Move the point to the specified mark. */

void
BPointToMark(struct mark *mptr)
	{
	if (mptr->pptr == NULL) {
		DError(RES_NOTMARK);
		return;
		}
	if (mptr->bptr != cbuf) BBufGoto(mptr->bptr);
	B_PageToCurrent(mptr->pptr);
	B_MovePointTo(mptr->mark_offset);
	cur_col = -1;
	}


/* ------------------------------------------------------------ */

/* Restore the current column. */

void
BPopState(void)
	{
	cur_col = savecol;
	}


/* ------------------------------------------------------------ */

/* Save the current column. */

void
BPushState(void)
	{
	savecol = cur_col;
	}


/* ------------------------------------------------------------ */

/* Copy the region from the current point to mark MPTR to the buffer
specified by BPTR. */

void
BRegCopy(struct mark *mptr, struct buffer *bptr)
	{
	struct buffer *save_buf = cbuf;
	struct mark *mptr2;
	struct mark *mptr3;
	FLAG isafter;
	int from;
	int to;
	char *save_point;

	if (bptr == cbuf) {
		DError(RES_NOTCOPYSAME);
		return;
		}
	if (isafter = BIsAfterMark(mptr)) BMarkSwap(mptr);
	mptr2 = BMarkCreate();
	while (BIsBeforeMark(mptr)) {
		if (cur_page == mptr->pptr)
			from = mptr->mark_offset - cur_pt_offset;
		else	from = cur_page_len - cur_pt_offset;
		B_GetGap();
		cur_is_mod = TRUE;
		save_point = cur_gap_end;

		BBufGoto(bptr);
		to = PAGESIZE - cur_page_len;
		if (to == 0)
			if (B_PageSplit(cur_pt_offset +
				 ((cur_pt_offset < PAGESIZE / 2) ? 1 : -1)))
				to = PAGESIZE - cur_page_len;
			else 	{
				BBufGoto(save_buf);
				break;
				}
		B_GetGap();
		if (from < to) to = from;
		memmove(cur_gap_start, save_point, to);
		cur_page_len += to;
		cur_gap_start += to;
		for (mptr3 = marks; mptr3 < &marks[MAXMARK]; ++mptr3) {
			if (mptr3->pptr == cur_page &&
				 mptr3->mark_offset > cur_pt_offset) {
				mptr3->mark_offset += to;
				}
			}
		for (mptr3 = screenmarks; mptr3 < &screenmarks[MAXSMARK];
			 ++mptr3) {
			if (mptr3->pptr == cur_page &&
				 mptr3->mark_offset > cur_pt_offset) {
				mptr3->mark_offset += to;
				}
			}
		B_MovePointTo(cur_pt_offset + to);
		B_SetScreenMarks(FALSE);
		cur_is_mod = TRUE;
		cbuf->is_mod = TRUE;
		BBufGoto(save_buf);
		BMoveBy(to);
		}
	BPointToMark(mptr2);
	BMarkDelete(mptr2);
	if (isafter) BMarkSwap(mptr);
	}


/* ------------------------------------------------------------ */

/* delete from the point to the mark */

void
BRegDelete(struct mark *mptr)
	{
	if (BIsAfterMark(mptr)) BMarkSwap(mptr);
	if (!BIsBeforeMark(mptr)) return;

	if (cur_page == mptr->pptr)
		BCharDelete(mptr->mark_offset - cur_pt_offset);
	else	{
		BCharDelete(cur_page_len - cur_pt_offset);
		BRegDelete(mptr);
		}
	}


/* ------------------------------------------------------------ */

/* Search backwards for both C1 and C2. */

FLAG
BSearchB(char c1, char c2)
	{
	char *cptr;

	do	{
		if (BIsStart()) return(FALSE);
		if (BIsEnd() || cur_cptr == cur_gap_end ||
			 cur_cptr == cur_page_start)
			BMoveBy(-1);
		else	{
			cptr = (cur_cptr >= cur_gap_end) ? cur_gap_end :
				cur_page_start;
			BMoveBy(max(
				B_IndexB(cur_cptr - 1, cur_cptr - cptr, c1),
				B_IndexB(cur_cptr - 1, cur_cptr - cptr, c2))
					- 1);
			}
		} while (*cur_cptr != c1 && *cur_cptr != c2);
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Search forward for both C1 and C2. */

FLAG
BSearchF(char c1, char c2)
	{
	while (!BIsEnd() && *cur_cptr != c1 && *cur_cptr != c2) {
		if (cur_cptr >= cur_gap_start) {
			B_MovePointTo(cur_pt_offset + min(
B_IndexF(cur_cptr, (cur_page_start + PAGESIZE) - cur_cptr, c1),
B_IndexF(cur_cptr, (cur_page_start + PAGESIZE) - cur_cptr, c2)));
			}
		else	{
			B_MovePointTo(cur_pt_offset + min(
B_IndexF(cur_cptr, cur_gap_start - cur_cptr, c1),
B_IndexF(cur_cptr, cur_gap_start - cur_cptr, c2)));
			}
		if (cur_pt_offset >= cur_page_len && cur_page->next) {
			B_PageToCurrent(cur_page->next);
			B_MovePointTo(0);
			}
		}
	cur_col = -1;
	if (!BIsEnd()) {
		BMoveBy(1);
		return(TRUE);
		}
	else	return(FALSE);
	}


/* ------------------------------------------------------------ */

/* The buffer has just been read in.  Check it for a ruler line. */

void
B_FileReadCheck(void)
	{
	if (IsNL()) BMoveBy(1);
	for (;;) {
		if (*cur_cptr == SP || *cur_cptr == TAB) {
			MovePastF(IsWhite);
			if (!IsNL()) break;
			BMoveBy(1);
			}
		else if (*cur_cptr == '.' || *cur_cptr == '@') {
			SearchNLF();
			}
		else if (*cur_cptr == ZCK) {
			B_RulerLine();
			break;
			}
		else break;
		}
	BMoveToStart();
	}


/* ------------------------------------------------------------ */

/* Move the gap to the point. */

void
B_GetGap(void)
	{
	if (cur_cptr == cur_gap_end) return;

	if (cur_cptr < cur_gap_start) {
		cur_gap_end -= cur_gap_start - cur_cptr;
		memmove(cur_gap_end, cur_cptr, cur_gap_start - cur_cptr);
		cur_gap_start = cur_cptr;
		cur_cptr = cur_gap_end;
		}
	else	{
		memmove(cur_gap_start, cur_gap_end, cur_cptr - cur_gap_end);
		cur_gap_start += cur_cptr - cur_gap_end;
		cur_gap_end = cur_cptr;
		}
	}


/* ------------------------------------------------------------ */

/* Return the offset of CHR backwards in BUF, which is LEN characters
long. */

int
B_IndexB(char *buf, int len, char chr)
	{
	char *cptr;

	for (cptr = buf; len-- > 0 && *cptr != chr; --cptr) ;
	return(cptr - buf);
	}


/* ------------------------------------------------------------ */

/* Return the offset of CHR in BUF, which is LEN characters long. */

int
B_IndexF(char *buf, int len, char chr)
	{
	char *cptr;

	for (cptr = buf; len-- > 0 && *cptr != chr; ++cptr) ;
	return(cptr - buf);
	}


/* ------------------------------------------------------------ */

/* Insert the specified character into the buffer. */

FLAG
B_InsChar(char chr)
	{
	struct mark *mptr;

	if (cur_gap_start == cur_gap_end && !B_PageSplit(PAGESIZE / 2))
		return(FALSE);
	B_GetGap();				
	*cur_gap_start++ = chr;
	++cur_page_len;
	B_MovePointTo(cur_pt_offset + 1);

	if (chr == NL || chr == SNL)
		cur_col = 0;
	else if (cur_col >= 0) {
		cur_col += (chr == TAB) ? TGetTabWidth(cur_col) :
			TGetWidth(chr);
		}

	cbuf->is_mod = TRUE;
	cur_is_mod = TRUE;

	for (mptr = marks; mptr < &marks[MAXMARK]; ++mptr) {
		if (mptr->pptr == cur_page &&
			 mptr->mark_offset >= cur_pt_offset) {
			++(mptr->mark_offset);
			}
		}
	for (mptr = screenmarks; mptr < &screenmarks[MAXSMARK]; ++mptr) {
		if (mptr->pptr == cur_page &&
			 mptr->mark_offset >= cur_pt_offset) {
			++(mptr->mark_offset);
			}
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Update the marks to a new page using NEW_PAGE, AMT, NEW_OFFSET. */

void
B_MarkUpdate(struct virt_page *new_page, int amt, int new_offset)
	{
	struct mark *mptr;

	for (mptr = marks; mptr < &marks[MAXMARK]; ++mptr) {
		if (mptr->pptr == cur_page &&
			 mptr->mark_offset >= cur_pt_offset) {
			if (mptr->mark_offset >= cur_pt_offset + amt)
				mptr->mark_offset -= amt;
			else	{
				mptr->pptr = new_page;
				mptr->mark_offset = new_offset;
				}
			}
		}
	for (mptr = screenmarks; mptr < &screenmarks[MAXSMARK]; ++mptr) {
		if (mptr->pptr == cur_page &&
			 mptr->mark_offset >= cur_pt_offset) {
			if (mptr->mark_offset >= cur_pt_offset + amt)
				mptr->mark_offset -= amt;
			else	{
				mptr->pptr = new_page;
				mptr->mark_offset = new_offset;
				}
			}
		}
	}


/* ------------------------------------------------------------ */

/* Update the marks to a new page using NEW_PAGE, NEW_OFFSET. */

void
B_MarkUpdateNP(struct virt_page *new_page, int new_offset)
	{
	struct mark *mptr;

	for (mptr = marks; mptr < &marks[MAXMARK]; ++mptr) {
		if (mptr->pptr == cur_page) {
			mptr->pptr = new_page;
			mptr->mark_offset = new_offset;
			}
		}
	for (mptr = screenmarks; mptr < &screenmarks[MAXSMARK]; ++mptr) {
		if (mptr->pptr == cur_page) {
			mptr->pptr = new_page;
			mptr->mark_offset = new_offset;
			}
		}
	}


/* ------------------------------------------------------------ */

/* Move the point AMT chars into the current page. */

void
B_MovePointTo(int amt)
	{
	cur_pt_offset = amt;
	cur_cptr = cur_page_start + amt;
	if (cur_cptr >= cur_gap_start) cur_cptr += cur_gap_end - cur_gap_start;
	}


/* ------------------------------------------------------------ */

/* Allocate a new page. */

struct virt_page *
B_PageAllocate(struct buffer *bptr, struct virt_page *prev,
	struct virt_page *next)
	{
	int cnt;
	struct virt_page *vpage;
	struct phys_page *ppage;

	for (cnt = 0; cnt < num_swap && swap_map[cnt]; ++cnt) ;
	if (cnt >= num_swap) {
		DError(RES_NOSWAP);
		return(NULL);
		}
	swap_map[cnt] = TRUE;
	vpage = &vp[cnt];

	vpage->next = next;
	vpage->prev = prev;
	if (next != NULL)
		next->prev = vpage;
	else	bptr->last = vpage;
	if (prev != NULL)
		prev->next = vpage;
	else	bptr->first = vpage;

	vpage->gap_start = 0;
	vpage->page_len = 0;
	vpage->where = 'S';
	vpage->page_num = cnt;
	++(bptr->num_pages);
	return(vpage);
	}


/* ------------------------------------------------------------ */

/* Find a free page. */

struct phys_page *
B_PageFindFree(void)
	{
	struct phys_page *ppage;
	int cnt;

	cnt = NUM_PHYS_PAGES / 3 + 1;
	for (ppage = last_phys_page; --cnt > 0 &&
		 (ppage->page_loc == 'L' || ppage->is_mod);
		 ppage = ppage->prev) ;

	if (cnt <= 0) {
		BFlush();
		for (ppage = last_phys_page; ppage != NULL &&
			 (ppage->page_loc == 'L' || ppage->is_mod);
			 ppage = ppage->prev) ;
		if (ppage == NULL) {
			DError(RES_INTPAGEERR);
			return(NULL);
			}
		}

	if (ppage->page_loc == 'S') {
		ppage->pptr->where = 'S';
		ppage->pptr->page_num = ppage->page_num;
		}
	return(ppage);
	}


/* ------------------------------------------------------------ */

/* Free the specifed page. */

void
B_PageFree(struct buffer *bptr, struct virt_page *vpage)
	{
	struct phys_page *ppage;

	if (vpage->next != NULL)
		vpage->next->prev = vpage->prev;
	else	bptr->last = vpage->prev;
	if (vpage->prev != NULL)
		vpage->prev->next = vpage->next;
	else	bptr->first = vpage->next;

	--(bptr->num_pages);

	if (vpage->where == 'M') {
		ppage = &pages[vpage->page_num];
		swap_map[ppage->page_num] = FALSE;
		pages[vpage->page_num].page_loc = 'M';
		pages[vpage->page_num].is_mod = FALSE;
		}
	else	swap_map[vpage->page_num] = FALSE;
	}


/* ------------------------------------------------------------ */

/* Split the curent (presumably full) page.  AMOUNT is where to split
it. */

FLAG
B_PageSplit(int amount)
	{
	struct mark *mptr;
	struct virt_page *vpage;
	struct phys_page *ppage;

	if ((vpage = B_PageAllocate(cbuf, cur_page, cur_page->next)) == NULL)
		return(FALSE);
	B_PageToMemory(vpage);

	ppage = &pages[vpage->page_num];
	memmove(ppage->page_data, cur_page_start + amount, PAGESIZE - amount);

	ppage->is_mod = TRUE;
	cur_is_mod = TRUE;
	cur_gap_start = cur_page_start + amount;
	cur_gap_end = cur_page_start + PAGESIZE;
	cur_page_len = amount;
	vpage->page_len = PAGESIZE - amount;
	vpage->gap_start = PAGESIZE - amount;

	for (mptr = marks; mptr < &marks[MAXMARK]; ++mptr) {
		if (mptr->pptr == cur_page && mptr->mark_offset >= amount) {
			mptr->pptr = vpage;
			mptr->mark_offset -= amount;
			}
		}
	for (mptr = screenmarks; mptr < &screenmarks[MAXSMARK]; ++mptr) {
		if (mptr->pptr == cur_page && mptr->mark_offset >= amount) {
			mptr->pptr = vpage;
			mptr->mark_offset -= amount;
			}
		}

	if (cur_pt_offset >= amount) {
		B_PageToCurrent(vpage);
		B_MovePointTo(cur_pt_offset - amount);
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Make VPAGE the current page. */

void
B_PageToCurrent(struct virt_page *vpage)
	{
	struct phys_page *ppage;

	if (vpage == NULL) {
		DError(RES_NULLPAGE);
		return;
		}

	if (cur_page == vpage) return;

	if (cur_page != NULL) {
		cur_page->page_len = cur_page_len;
		cur_page->gap_start = cur_gap_start - cur_page_start;
		pages[cur_page->page_num].is_mod = cur_is_mod;
		}

	B_PageToMemory(vpage);
	cur_page = vpage;
	ppage = &pages[cur_page->page_num];
	cur_page_start = ppage->page_data;
	cur_is_mod = ppage->is_mod;
	cur_page_len = cur_page->page_len;
	cur_gap_start = cur_page_start + cur_page->gap_start;
	cur_gap_end = cur_gap_start - cur_page_len + PAGESIZE;

	if (ppage == first_phys_page) return;
	if (ppage->next == NULL)
		last_phys_page = ppage->prev;
	else	ppage->next->prev = ppage->prev;
	if (ppage->prev != NULL) ppage->prev->next = ppage->next;
	ppage->prev = NULL;
	ppage->next = first_phys_page;
	first_phys_page->prev = ppage;
	first_phys_page = ppage;
	}


/* ------------------------------------------------------------ */

/* Make sure that the specified page is swapped into memory. */

void
B_PageToMemory(struct virt_page *vpage)
	{
	struct phys_page *ppage;

	if (vpage->where == 'M') return;
	ppage = B_PageFindFree();
	ppage->page_loc = 'S';
	ppage->page_num = vpage->page_num;
	ppage->is_mod = FALSE;
	ppage->pptr = vpage;

	if (vpage->page_len != 0) {
#if defined(MSDOS)
#if defined(SYSMGR)
		char huge *tmp = (char huge *) (
			((long)swap_base << 16) +
			((long)_DS << 16) );
#else
		char huge *tmp = (char huge *)((long)swap_base << 16);
#endif
		PageGet(ppage->page_data,
			tmp + ((long)ppage->page_num) * PAGESIZE, PAGESIZE);
#endif
#if defined(UNIX)
		memmove(ppage->page_data,
			swap_base + ppage->page_num * PAGESIZE, PAGESIZE);
#endif
		}

	vpage->where = 'M';
	vpage->page_num = ppage - pages;
	}


/* ------------------------------------------------------------ */

/* You are at a ruler line.  Process it. */

void
B_RulerLine(void)
	{
	int num;
	int chr;

	BMoveBy(1);
	for (;;) {
		chr = NUL;
		switch (xtoupper(BGetCharAdv())) {

		case 'L':
			BMoveBy(1);
			num = 0;
			while (xisdigit(chr = BGetCharAdv())) {
				num = num * 10 + chr - '0';
				}
			cbuf->c.left_margin = num;
			break;

		case 'R':
			BMoveBy(1);
			num = 0;
			while (xisdigit(chr = BGetCharAdv())) {
				num = num * 10 + chr - '0';
				}
			cbuf->c.right_margin = num;
			break;

		case 'T':
			BMoveBy(1);
			num = 0;
			while (xisdigit(chr = BGetCharAdv())) {
				num = num * 10 + chr - '0';
				}
			cbuf->c.tab_spacing = num;
			break;

		default:
			WFixup(&(cbuf->c));
			return;
			/*break;*/
			}
		if (chr == NL) break;
		}
	WFixup(&(cbuf->c));
	}


/* ------------------------------------------------------------ */

/* Set the modified flag to NEWFLAG. */

void
B_SetScreenMarks(FLAG newflag)
	{
	struct window *wptr;

	for (wptr = windows; wptr < &windows[NUMWINDOWS]; wptr++) {
		if (wptr->visible) B_SetScreenMarks2(wptr, newflag);
		}
	}


/* ------------------------------------------------------------ */

/* Set one windows' modified flags. */

void
B_SetScreenMarks2(struct window *wptr, FLAG newflag)
	{
	struct mark *mptr;
	struct mark *botmptr;
	struct mark *topmptr;

	botmptr = &screenmarks[wptr->bot];
	topmptr = &screenmarks[wptr->top];

	for (mptr = topmptr; mptr <= botmptr &&
		mptr->pptr != cur_page; ++mptr) ;

	if (mptr->bptr != cbuf) {
		mptr->is_mod = TRUE;
		}
	else if (mptr > botmptr) {
		for (mptr = topmptr; mptr <= botmptr &&
			(mptr->bptr != cbuf || BIsAfterMark(mptr));
			++mptr) ;
		if (mptr > topmptr) {
			while ((--mptr)->bptr != cbuf) ;
			mptr->is_mod = TRUE;
			}
		}
	else	{
		while (mptr->pptr == cur_page &&
			 mptr->mark_offset <= cur_pt_offset &&
			 mptr <= botmptr)
			++mptr;

		if (--mptr >= topmptr) mptr->is_mod = TRUE;

		if (newflag) {
			while (mptr > topmptr && mptr->pptr == cur_page &&
				 mptr->mark_offset == cur_pt_offset) {
				(--mptr)->is_mod = TRUE;
				}
			}
		}
	}


/* ------------------------------------------------------------ */

/* Check buffer data structures for consistency. */

#if defined(DEBUGGING_CODE)
void B_XVirtPage(struct virt_page *vptr, char *descr, int cnt);

void
B_XDebug(void)
	{
	struct buffer *bptr;
	struct mark *mptr;
	struct virt_page *vptr;
	char buf[1000];
	int cnt;
	int cnt2;

/* check buffers */
	for (cnt = 0; cnt < NUMBUFFERS; cnt++) {
		bptr = &buffers[cnt];
		if (bptr->num_pages == 0) continue;	/* free */

		if (*bptr->fname == NUL) {
			xsprintf(buf, "buffer %d has no filename", cnt);
			DErrorStr(buf);
			}
		if (strlen(bptr->fname) > FNAMEMAX) {
			xsprintf(buf, "buffer %d has invalid filename", cnt);
			DErrorStr(buf);
			}

		for (cnt2 = 0; cnt2 < MAXMARK; cnt2++) {
			if (bptr->mptr == &marks[cnt2]) break;
			}
		if (cnt2 >= MAXMARK) {
			xsprintf(buf, "buffer %d has invalid mark", cnt);
			DErrorStr(buf);
			}

		B_XVirtPage(bptr->first, "buf first", cnt);
		B_XVirtPage(bptr->last, "buf last", cnt);
		B_XVirtPage(bptr->point_page, "buf point_page", cnt);
		for (vptr = bptr->first, cnt2 = 0; vptr != bptr->last;
			 vptr = vptr->next, cnt2++) {
			B_XVirtPage(vptr, "buf page", cnt2);
			}

		if (bptr->point_offset < 0 || bptr->point_offset >= PAGESIZE) {
			xsprintf(buf, "buffer %d has invalid offset %d", cnt,
				bptr->point_offset);
			DErrorStr(buf);
			}
		if (bptr->point_page->page_len &&
			 bptr->point_offset > bptr->point_page->page_len + 1) {
			xsprintf(buf, "buffer %d has invalid offset %d of %d",
				cnt, bptr->point_offset,
				bptr->point_page->page_len);
			DErrorStr(buf);
			}

		if (bptr->num_pages < 0 || bptr->num_pages >= num_swap) {
			xsprintf(buf, "buffer %d has invalid num of pages %d",
				cnt, bptr->num_pages);
			DErrorStr(buf);
			}
		}

/* check marks */

	for (cnt = 0; cnt < MAXMARK; cnt++) {
		mptr = &marks[cnt];
		if (mptr->pptr == NULL) continue;
		if (BIsFree(mptr->bptr)) continue;

		B_XVirtPage(mptr->pptr, "mark pptr", cnt);

		if (mptr->mark_offset < 0 || mptr->mark_offset >= PAGESIZE) {
			xsprintf(buf, "mark %d has invalid offset %d", cnt,
				mptr->mark_offset);
			DErrorStr(buf);
			}
		if (mptr->pptr->page_len > 0 &&
			 mptr->mark_offset > mptr->pptr->page_len + 1) {
			xsprintf(buf, "mark %d has invalid offset %d of %d",
				cnt, mptr->mark_offset, mptr->pptr->page_len);
			DErrorStr(buf);
			}
		}

/* screen marks */

	for (cnt = 0; cnt < MAXSMARK - 1; cnt++) {
		mptr = &screenmarks[cnt];
		if (mptr->pptr == NULL) continue;
		if (BIsFree(mptr->bptr)) continue;

		B_XVirtPage(mptr->pptr, "scrnmark pptr", cnt);

		if (mptr->mark_offset < 0 || mptr->mark_offset >= PAGESIZE) {
			xsprintf(buf, "smark %d has invalid offset %d", cnt,
				mptr->mark_offset);
			DErrorStr(buf);
			}
		if (mptr->pptr->page_len > 0 &&
			 mptr->mark_offset > mptr->pptr->page_len + 1) {
			xsprintf(buf, "smark %d has invalid offset %d of %d",
				cnt, mptr->mark_offset, mptr->pptr->page_len);
			DErrorStr(buf);
			}
		}
	}

void
B_XVirtPage(struct virt_page *vptr, char *descr, int cnt)
	{
	char buf[1000];
/*
	struct virt_page *next;
	struct virt_page *prev;
*/
	if (vptr->where != 'L' && vptr->where != 'M' && vptr->where != 'S') {
		xsprintf(buf, "virt page %s %d has invalid loc %c",
			descr, cnt, vptr->where);
		DErrorStr(buf);
		}

	if (vptr->page_num < 0 || vptr->page_num > num_swap) {
		xsprintf(buf, "virt page %s %d has invalid num %d",
			descr, cnt, vptr->page_num);
		DErrorStr(buf);
		}

	if (vptr->page_len < 0 || vptr->page_len > PAGESIZE) {
		xsprintf(buf, "virt page %s %d has invalid page size %d",
			descr, cnt, vptr->page_len);
		DErrorStr(buf);
		}

	if (vptr->gap_start < 0 ||
		 (vptr->page_len > 0 && vptr->gap_start > vptr->page_len)) {
		xsprintf(buf, "virt page %s %d has invalid gap start %d",
			descr, cnt, vptr->gap_start);
		DErrorStr(buf);
		}
	}
struct buffer *cbuf;			/* the curent buffer */
struct mark *mark;			/* the current mark */
static struct virt_page *cur_page;	/* virtual page */
static int cur_pt_offset;		/* point offset */
static int cur_page_len;		/* page length */
static char *cur_cptr;			/* actual character */
static char *cur_page_start;		/* start of the page */
static char *cur_gap_start;		/* position of the gap */
static char *cur_gap_end;
static FLAG cur_is_mod;			/* is modified (dirty) */
static int cur_col = -1;		/* current column position */
static int savecol;
struct mark *getmark;
static struct phys_page *first_phys_page;  /* LRU chain of pages in memory */
static struct phys_page *last_phys_page;
#define SWAPMAPSIZE	((long)SWAPMAX * 1024 / PAGESIZE)
static char swap_map[SWAPMAPSIZE];	/* swap file in use flags */
#endif

/* end of FBUF.C -- Buffer Managment */
