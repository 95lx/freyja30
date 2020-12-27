/* MEMUTIL.C -- HP95LX System-Manager Compliant Memory Utility

	Written April 1992 by Craig A. Finseth
	Copyright 1992,3,4 by Craig A. Finseth
*/

#include "memutil.h"
#include "sysmgr.h"

#define FP_OFF(fp)	((unsigned)(fp))
#define FP_SEG(fp)	((unsigned)((unsigned long)(fp) >> 16))

#define DEBUG95_CMD	"C:\\_SYS\\DEBUG"
#define DEBUG100_CMD	"D:\\DOS\\DEBUG"

	/* application states */

#define NUM_AP_STATES	(sizeof(aps) / sizeof(aps[0]))

static struct ap_states {
	char state;
	char *name;
	} aps[] = {
	{ 0, "Closed" },
	{ 1, "Active" },
	{ 2, "Suspended" },
	{ 4, "Exit" },
	{ 8, "Yield" },
	{ 16, "Exit Refused" } };

	/* display status */

enum dstate { DHELP, DAPPS, DAPLONG, DCHAINS, DMEM, DCHARS };

	/* task control structures */

struct mutask {
	enum dstate disp;	/* which type of display to use */
	unsigned height;	/* height of screen in objects */
	unsigned start;		/* first object */
	unsigned num;		/* number of objects */
	unsigned cur;		/* current object, >= 0 and < num */
	};

static struct mutask t[] = {
	{ DHELP, HEIGHT, 0, 0, 0 },
	{ DAPPS, HEIGHT, 0, NUMAPPS, DEFAULTAPP },
	{ DAPLONG, 1, 0, NUMAPPS, DEFAULTAPP },
	{ DCHAINS, HEIGHT, 0, 1, 0 },
	{ DMEM, HEIGHT / 2, 0, 64, 0 },
	{ DCHARS, HEIGHT, 0, 512, 0 } };

static struct mutask *curt = &t[DAPPS];		/* current task */

	/* per-task storage */

static struct acb acbs[NUMAPPS];		/* for DAPPS */
static struct mem_chain links[NUMLINKS];	/* for DCHAINS */
static char chars_buf[BUFFSIZE];		/* for Carat/Label */
static struct task far *sys_tasks;

	/* last six tasks */

#define NUM_LAST	(sizeof(last) / sizeof(last[0]))
static struct mutask *last[6] = { &t[DAPPS], &t[DAPPS], &t[DAPPS],
	&t[DAPPS], &t[DAPPS], &t[DAPPS] };

	/* global flags */

static FLAG isterm = FALSE;	/* are we quitting the program? */
static FLAG isbin = FALSE;	/* text or binary saving? */
static FLAG is100 = FALSE;	/* 95 or 100? */

	/* key labels */
static char *labels[] = {
	/* 0-9 */
"", "", "", "Ctrl-2", "", "", "", "", "", "",
	/* 10-19 */
"", "", "", "", "", "Shift-Tab", "Alt-Q", "Alt-W", "Alt-E", "Alt-R",
	/* 20-29 */
"Alt-T", "Alt-Y", "Alt-U", "Alt-I", "Alt-O", "Alt-P", "", "", "", "",
	/* 30-39 */
"Alt-A", "Alt-S", "Alt-D", "Alt-F", "Alt-G", "Alt-H", "Alt-J",
"Alt-K", "Alt-L", "",
	/* 40-49 */
"", "", "", "", "Alt-Z", "Alt-X", "Alt-C", "Alt-V", "Alt-B", "Alt-N",
	/* 50-59 */
"Alt-M", "", "", "", "", "", "", "", "", "F1",
	/* 60-69 */
"F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "",
	/* 70-79 */
"", "Home", "Up Arrow", "PgUp", "", "Left Arrow", "", "Right Arrow",
"", "End",
	/* 80-89 */
"Down Arrow", "PgDn", "Ins", "Del", "Shift-F1", "Shift-F2", "Shift-F3",
"Shift-F4", "Shift-F5", "Shift-F6",
	/* 90-99 */
"Shift-F7", "Shift-F8", "Shift-F9", "Shift-F10", "Ctrl-F1", "Ctrl-F2",
"Ctrl-F3", "Ctrl-F4", "Ctrl-F5", "Ctrl-F6",
	/* 100-109 */
"Ctrl-F7", "Ctrl-F8", "Ctrl-F9", "Ctrl-F10", "Alt-F1", "Alt-F2", "Alt-F2",
"Alt-F4", "Alt-F5", "Alt-F6",
	/* 110-119 */
"Alt-F7", "Alt-F8", "Alt-F9", "Alt-F10", "Ctrl-PrtSc", "Ctrl-Left Arrow",
"Ctrl-Right Arrow", "Ctrl-End", "Ctrl-PgDn", "Ctrl-Home",
	/* 120-129 */
"Alt-1", "Alt-2", "Alt-3", "Alt-4", "Alt-5", "Alt-6", "Alt-7", "Alt-8",
"Alt-9", "Alt-0",
	/* 130-139 */
"Alt--", "Alt-=", "Ctrl-PgUp", "", "", "", "", "", "", "",
	/* 140-149 */
"", "Ctrl-PgUp", "Ctrl--", "", "Alt-+", "Ctrl-PgDn", "Ctrl-Ins", "", "", "",
	/* 150-159 */
"Ctrl-*", "", "", "", "", "", "", "", "", "",
	/* 160-169 */
"", "", "Ctrl-On", "", "SETUP|&...", "Alt-Tab", "", "Alt-&...", "FILER", "",
	/* 170-179 */
"Ctrl-&...", "Alt-FILER", "COMM|cc:MAIL", "", "Ctrl-FILER", "Alt-COMM|cc:MAIL",
"APPT", "", "Ctrl-COMM|cc:MAIL", "Alt-APPT",
	/* 180-189 */
"PHONE", "", "Ctrl-APPT", "Alt-PHONE", "MEMO", "", "Ctrl-PHONE",
"Alt-MEMO", "123", "",
	/* 190-199 */
"Ctrl-MEMO", "Alt-123", "CALC", "", "Ctrl-123", "Alt-CALC", "", "",
"Ctrl-CALC", "",
	/* 200-209 */
"MENU", "Shift-MENU", "Ctrl-MENU", "Alt-MENU", "", "", "", "", "ZOOM", "DATE",
	/* 210-219 */
"TIME", "", "CUT", "COPY", "PASTE", "", "", "", "", "",
	/* 220-229 */
"", "", "", "", "", "", "", "", "", "",
	/* 230-239 */
"", "", "", "", "", "", "", "", "", "",
	/* 240-249 */
"", "", "", "", "", "", "", "", "", "",
	/* 250-255 */
"", "", "", "", "", "" };

char *ApState(int state);
char *Carat(int chr);
void Command(char *str);
void Display(void);
void Disp_Help(void);
void Disp_Apps(void);
void Disp_ApLong(void);
void Disp_Chains(void);
void Disp_Mem(void);
void Disp_Chars(void);
FLAG GetFile(char *prompt, char *fname, FLAG usestar);
FLAG GetHex(char *prompt, unsigned *val, unsigned mx);
int GetKey(char *prompt);
FLAG GetMenu(void);
FLAG GetStr(char *prompt, char *buf, char *deflt);
char *Label(int chr);
void Push_Task(void);
void Refresh(void);
void ToClip(void);
void ToFile(void);
void To_Help(int fd, int clip);
void To_Apps(int fd, int clip);
void To_ApLong(int fd, int clip);
void To_Chains(int fd, int clip);
void To_Mem(int fd, int clip);
void To_Chars(int fd, int clip);
FLAG To_Write(int fd, int clip, char *buf, int len);

/* ------------------------------------------------------------ */

void
main(void)
	{
	int amt;
	unsigned key;
	EVENT e;

	m_init();

	if (JGetType() == 'C') is100 = TRUE;

	Refresh();

	links[0].seg = 0;
	links[0].size = 1;
	links[0].owner = 0;
	xstrcpy(links[0].name, "[uninitialized]");

	e.kind = E_KEY;
	do	{
		if (curt->cur > curt->num - 1) curt->cur = curt->num - 1;
		if (curt->num == 0) curt->cur = 0;

		/* ensure that the APPS and APLONG selections stay in sync */
		if (curt == &t[DAPPS]) t[DAPLONG].cur = curt->cur;
		if (curt == &t[DAPLONG]) t[DAPPS].cur = curt->cur;

		if (e.kind == E_KEY) Display();

		m_event(&e);
		switch (e.kind) {

		case E_NONE:
			m_posttime();
			break;

		case E_KEY:
			switch (e.data) {

			case CR:
				if (curt == &t[DAPPS]) {
					Push_Task();
					curt = &t[DAPLONG];
					}
				else if (curt == &t[DAPLONG]) {
					Push_Task();
					curt = &t[DMEM];
					curt->start = acbs[t[DAPPS].cur].ds;
					curt->num = APPSEGSIZE;
					curt->cur = 0;
					}
				else if (curt == &t[DCHAINS]) {
					Push_Task();
					curt = &t[DMEM];
					curt->start = links[t[DCHAINS].cur].seg;
					curt->num = links[t[DCHAINS].cur].size;
					curt->cur = 0;
					}
				break;

			case ESC:
				curt = last[NUM_LAST - 1];
				for (amt = NUM_LAST - 1; amt > 0; amt--) {
					last[amt] = last[amt - 1];
					}
				last[0] = &t[DAPPS];
				break;

			case 0x3b00:		/* F1 */
				Push_Task();
				curt = &t[DHELP];
				break;

			case 0x3c00:		/* F2 */
				Push_Task();
				t[DCHAINS].num = GetChain(links, NUMLINKS);
				curt = &t[DCHAINS];
				Refresh();
				break;

			case 0x3d00:		/* F3 */
				Push_Task();
				curt = &t[DAPPS];
				break;

			case 0x3e00:		/* F4 */
				Push_Task();
				curt = &t[DMEM];
				break;

			case 0x3f00:		/* F5 */
				Push_Task();
				curt = &t[DAPLONG];
				break;

			case 0x4000:		/* F6 */
				Push_Task();
				curt = &t[DCHARS];
				break;

			case 0x4100:		/* F7 */
				key = t[DMEM].start;
				if (GetHex("Paragraph to show:", &key,
					 0xFFFF)) {
					Push_Task();
					curt = &t[DMEM];
					curt->start = key;
					curt->cur = 0;
					}
				break;

			case 0x4200:		/* F8 */
				Push_Task();
				curt = &t[DCHARS];
				key = GetKey("Press key to view");
				if (key > 0xff) {
					key >>= 8;
					key &= 0xff;
					key += 256;
					}
				curt->cur = key;
				break;

			case 0x4300:		/* F9 */
				key = t[DMEM].num;
				if (GetHex("Memory block length in pargraphs:",
					 &key, 0xFFFF)) {
					Push_Task();
					curt = &t[DMEM];
					curt->num = key;
					}
				break;

			case 0x4400:		/* F10 */
				key = curt->cur;
				if (GetHex("view starting at offset:", &key,
					 curt->num)) {
					curt->cur = key;
					}
				break;

			case 0x4700:		/* Home */
				curt->cur = 0;
				break;

			case 0x4800:		/* Up */
				if (curt->cur >= 1) curt->cur--;
				break;

			case 0x4900:		/* PgUp */
				if (curt->cur >= curt->height)
					curt->cur -= curt->height;
				else	curt->cur = 0;
				break;

			case 0x4b00:		/* Left */
				amt = curt->num / 10;
				if (amt < 1) amt = 1;
				if (curt->cur >= amt)
					curt->cur -= amt;
				else	curt->cur = 0;
				break;

			case 0x4d00:		/* Right */
				amt = curt->num / 10;
				if (amt < 1) amt = 1;
				curt->cur += amt;
				break;

			case 0x4f00:		/* End */
				curt->cur = curt->num - 1;
				break;

			case 0x5000:		/* Down */
				curt->cur++;
				break;

			case 0x5100:		/* PgDn */
				curt->cur += curt->height;
				break;

			case 0xc800:		/* MENU */
				GetMenu();
				break;

			default:
				m_thud();
				break;
				}
			break;

		case E_ACTIV:
			Refresh();
			Display();
			break;
			}
		} while (!isterm && e.kind != E_TERM);
	m_fini();
	}


/* ------------------------------------------------------------ */

/* Return a pointer to a static buffer that contains a description
of the applcation's state information. */

char *
ApState(int state)
	{
	int cnt;

	for (cnt = 0; cnt < NUM_AP_STATES; cnt++) {
		if (state == aps[cnt].state) return(aps[cnt].name);
		}
	xsprintf(chars_buf, "??st %x", state);
	return(chars_buf);
	}


/* ------------------------------------------------------------ */

/* Return a pointer to a static buffer that contains the carat
notation form of the supplied character. */

char *
Carat(int chr)
	{
	char *buf = chars_buf;

	chr &= 0xff;
	if (chr & 0x80) {
		*buf++ = '~';
		chr &= 0x7f;
		}

	if (chr < SP || chr == DEL) {
		*buf++ = '^';
		chr ^= '@';
		}

	*buf++ = chr;
	*buf = NUL;

	strcat(chars_buf, "  ");
	chars_buf[3] = NUL;
	return(chars_buf);
	}


/* ------------------------------------------------------------ */

/* Execute an external command.  If STR is not NULL, prompt. */

void
Command(char *str)
	{
	char buf[BUFFSIZE];

	if (str == NULL) {
		GetStr("Command to execute:", buf, "");
		}
	else	xstrcpy(buf, str);
	strcat(buf, "\r\n");
	
	m_spawn(buf, strlen(buf) - 2, 2, buf);
	}


/* ------------------------------------------------------------ */

void
Display(void)
	{
	char buf[41];	/* we know how big the screen is */
	int start;	/* this stuff is for calculating the slider bar */
	int stop;
	unsigned ustart;
	unsigned ustop;
	unsigned tmp;

	VidCurOff();	/* force the cursor off */

			/* display the top line */
	m_disp(-3, 0, "MemUtil V2.3                            ", 40, 0, 0);

			/* build the slider bar, using 16-bit
			arithmetic */
	memset(buf, '\xCD', 40);

	if (curt->num > 0) {
		if (curt->num > 512) {	/* losing precision isn't so bad... */
			tmp = curt->num / 40;
			ustart = curt->cur / tmp;
			if (ustart > 39) ustart = 39;
			ustop = (curt->cur + curt->height) / tmp;
			if (ustop <= ustart) ustop = ustart + 1;
			if (ustop > 40) ustop = 40;
			memset(&buf[ustart], '\xC4', ustop - ustart);
			}
		else	{	/* don't worry about overflow */
			start = (curt->cur * 40) / curt->num;
			if (start < 0) start = 0;
			if (start > 39) start = 39;
			stop = ((curt->cur + curt->height) * 40 ) / curt->num;
			if (stop <= start) stop = start + 1;
			if (stop > 40) stop = 40;
			memset(&buf[start], '\xC4', stop - start);
			}
		}
	m_disp(-2, 0, buf, 40, 0, 0);

			/* select the per-task display */
	switch (curt->disp) {

	case DHELP:	Disp_Help();	break;
	case DAPPS:	Disp_Apps();	break;
	case DAPLONG:	Disp_ApLong();	break;
	case DCHAINS:	Disp_Chains();	break;
	case DMEM:	Disp_Mem();	break;
	case DCHARS:	Disp_Chars();	break;
		}

			/* put up the function key labels; note the
			"1" in the second-to-last position that
			specifies inverse video */
	m_disp(11, 0, "Help    Apps    ApLong  Para    Length  ", 40, 1, 0);
	m_disp(12, 0, "    Chains  Mem     Chars   Key  Offset ", 40, 1, 0);
	}


/* ------------------------------------------------------------ */

/* Display the help screen. */

void
Disp_Help(void)
	{
	char buf[BUFFSIZE];
	int cnt;

			/* Loop until you get to the end of the screen or
			run out of objects. */
	for (cnt = 0; cnt < HEIGHT && cnt + curt->cur < curt->num; cnt++) {

			/* Put into a buffer, with 40 trailing spaces
			xsprintf is a private version of sprintf that
			has somewhat different performance tradeoffs. */
		xsprintf(buf, "%s                                        ",
			Help_Line(curt->cur + cnt));

			/* Display the first 40 characters of the
			buffer. */
		m_disp(-1 + cnt, 0, buf, 40, 0, 0);
		}

			/* If you ran out of lines and have not run
			out of screen, blank the rest of the screen. */
	if (cnt < HEIGHT) {
		m_clear(-1 + cnt, 0, HEIGHT - (-1 + cnt), 40); 
		}
	}


/* ------------------------------------------------------------ */

/* Display the application activity screen. */

void
Disp_Apps(void)
	{
	struct acb *a;
	struct task k;
	char buf[BUFFSIZE];
	char nbuf[80];
	char sbuf[80];
	char kbuf[80];
	char *name;
	unsigned key;
	int state;
	int cnt;

	for (cnt = 0; cnt < HEIGHT && cnt + curt->cur < curt->num; cnt++) {
		if (is100) {
			k = sys_tasks[curt->cur + cnt];
			name = k.t_extname;
			key = k.t_hotkey;
			state = k.t_state;
			}
		else	{
			a = &acbs[curt->cur + cnt];
			name = a->name;
			key = a->hot_key;
			state = a->state;
			}

		if (*name == NUL)
			xstrcpy(kbuf, "             ");
		else	{
			if (key >= 256) {
				xstrncpy(kbuf, Label((key >> 8) & 0xff));
				}
			else	{
				xstrcpy(kbuf, Carat(key));
				}
			kbuf[13] = NUL;
			strcat(kbuf, "             ");
			kbuf[13] = NUL;
			}

		if (*name == NUL) {
			xstrcpy(nbuf, "[none]         ");
			}
		else	{
			xstrncpy(nbuf, name);
			nbuf[15] = NUL;
			strcat(nbuf, "               ");
			nbuf[15] = NUL;
			}

		if (*name == NUL) {
			xstrcpy(sbuf, "         ");
			}
		else	{
			xstrncpy(sbuf, ApState(state));
			sbuf[9] = NUL;
			strcat(sbuf, "         ");
			sbuf[9] = NUL;
			}

		xsprintf(buf, "%s %s  %s", kbuf, nbuf, sbuf);
		m_disp(-1 + cnt, 0, buf, 40, 0, 0);
		}

	if (cnt < HEIGHT) {
		m_clear(-1 + cnt, 0, HEIGHT - (-1 + cnt), 40); 
		}
	}


/* ------------------------------------------------------------ */

/* Display the application activity screen. */

void
Disp_ApLong(void)
	{
	struct acb *a;
	struct task k;
	char buf[BUFFSIZE];
	char buf2[13];
	char buf3[128];
	char fname[30];		/* exactly legal apname file name size + 1 */
	int cnt;
	unsigned amt;

	if (is100) {
		k = sys_tasks[curt->cur];

		xstrncpy(buf2, k.t_extname);
		xsprintf(buf, "%s                                        ",
			buf2);
		m_disp(-1, 0, buf, 40, 0, 0);

		xsprintf(buf, "     status %s                            ",
			ApState(k.t_state));
		m_disp(0, 0, buf, 40, 0, 0);

		if (k.t_hotkey >= 256) {
			xsprintf(buf, "     hot key %s                          ",
				Label((k.t_hotkey >> 8) & 0xff));
			}
		else	{
			xsprintf(buf, "     hot key %s                          ",
				Carat(k.t_hotkey));
			}
		m_disp(1, 0, buf, 40, 0, 0);

		if (k.t_seg_image == 0) {
			m_disp(2, 0, "                                        ", 40, 0, 0);
			}
		else	{
			amt = k.t_seg_image + (k.t_off_image >> 4);
			BlockGet(buf3, sizeof(buf3), amt);
			amt = k.t_off_image & 0xf;
			xstrncpy(fname, &buf3[amt + 56]);
			xsprintf(buf, "%s                                        ", fname);
			m_disp(2, 0, buf, 40, 0, 0);
			}

		xsprintf(buf, "  stack %x:%x  far size %x  far off %x      ",
			k.t_ss, k.t_sp, k.t_far_size, k.t_far_off);
		m_disp(3, 0, buf, 40, 0, 0);

		xsprintf(buf, "  imagevec %x:%x  far rsvrd %x             ",
			k.t_seg_image, k.t_off_image, k.t_far_rsvrd);
		m_disp(4, 0, buf, 40, 0, 0);

		xsprintf(buf, "  DS %x  page count %x %x                  ",
			k.t_ds, k.t_pagecount[0], k.t_pagecount[1]);
		m_disp(5, 0, buf, 40, 0, 0);

		xsprintf(buf, "  mem seg %x  rsrc seg %x  ver %x          ",
			k.t_memseg, k.t_rsrc_seg, k.t_sys_ver);
		m_disp(6, 0, buf, 40, 0, 0);

		xsprintf(buf, "  flags %x  no wait %x  lock %x            ",
			k.t_is123, k.t_nowait, k.t_lock_state);
		m_disp(7, 0, buf, 40, 0, 0);

		xsprintf(buf, "  phy page %x %x  logic page %x %x          ",
			k.t_phypage[0], k.t_phypage[1],
			k.t_logicalpage[0], k.t_logicalpage[1]);
		m_disp(8, 0, buf, 40, 0, 0);

		xsprintf(buf, "  chip selection %2x %2x  ic u loc %x      ",
			k.t_chipsel[0], k.t_chipsel[1], k.t_ic_u_loc);
		m_disp(9, 0, buf, 40, 0, 0);

		xsprintf(buf, "  sp status %x  sp link %x  ic o loc %x    ",
			k.t_sp_status, k.t_sp_link, k.t_ic_o_loc);
		m_disp(10, 0, buf, 40, 0, 0);
		}
	else	{
		a = &acbs[curt->cur];

		xstrncpy(buf2, a->name);
		xsprintf(buf, "%s                                        ",
			buf2);
		m_disp(-1, 0, buf, 40, 0, 0);

		xsprintf(buf, "     status %s                            ",
			ApState(a->state));
		m_disp(0, 0, buf, 40, 0, 0);

		if (a->hot_key >= 256) {
			xsprintf(buf, "     hot key %s                          ",
				Label((a->hot_key >> 8) & 0xff));
			}
		else	{
			xsprintf(buf, "     hot key %s                          ",
				Carat(a->hot_key));
			}
		m_disp(1, 0, buf, 40, 0, 0);

		if (curt->cur < DEFAULTAPP || a->imagev_seg == 0) {
			m_disp(2, 0, "                                        ", 40, 0, 0);
			}
		else	{
			amt = a->imagev_seg + (a->imagev_off >> 4);
			BlockGet(buf3, sizeof(buf3), amt);
			amt = a->imagev_off & 0xf;
			xstrncpy(fname, &buf3[amt + 56]);
			xsprintf(buf, "%s                                        ", fname);
			m_disp(2, 0, buf, 40, 0, 0);
			}

		xsprintf(buf, "     stack %x:%x                          ",
			a->ss, a->sp);
		m_disp(3, 0, buf, 40, 0, 0);

		xsprintf(buf, "     imagevec %x:%x                       ",
			a->imagev_seg, a->imagev_off);
		m_disp(4, 0, buf, 40, 0, 0);

		xsprintf(buf, "     DS %x                               ", a->ds);
		m_disp(5, 0, buf, 40, 0, 0);

		xsprintf(buf, "     mem seg %x     rsrc seg %x           ",
			a->mem_seg, a->rsrc_seg);
		m_disp(6, 0, buf, 40, 0, 0);

		xsprintf(buf, "     flags %x     no wait %x              ",
			a->flags, a->nowait);
		m_disp(7, 0, buf, 40, 0, 0);

		xsprintf(buf, "     memory mapping %2x %2x %2x %2x %2x %2x   ",
			a->membank[0], a->membank[1], a->membank[2],
			a->membank[3], a->membank[4], a->membank[5]);
		m_disp(8, 0, buf, 40, 0, 0);

		xsprintf(buf, "     chip selection %2x %2x %2x %2x %2x %2x   ",
			a->chipsel[0], a->chipsel[1], a->chipsel[2],
			a->chipsel[3], a->chipsel[4], a->chipsel[5]);
		m_disp(9, 0, buf, 40, 0, 0);

		m_disp(10, 0, "                                        ", 40, 0, 0);
		}
	}


/* ------------------------------------------------------------ */

/* Display the memory chains screen. */

void
Disp_Chains(void)
	{
	char buf[BUFFSIZE];
	int cnt;

	for (cnt = 0; cnt < HEIGHT && cnt + curt->cur < curt->num; cnt++) {
		xsprintf(buf, "at %4x  len %4x  %4x:%s                                        ",
			links[cnt + curt->cur].seg,
			links[cnt + curt->cur].size,
			links[cnt + curt->cur].owner,
			links[cnt + curt->cur].name);
		m_disp(-1 + cnt, 0, buf, 40, 0, 0);
		}

	if (cnt < HEIGHT) {
		xsprintf(buf, "last paragraph %x                        ",
			links[curt->num].seg + links[curt->num].size + 1);
		m_disp(-1 + cnt, 0, buf, 40, 0, 0);
		cnt++;
		}

	if (cnt < HEIGHT) {
		m_clear(-1 + cnt, 0, HEIGHT - (-1 + cnt), 40); 
		}
	}


/* ------------------------------------------------------------ */

/* Display a memory block. */

void
Disp_Mem(void)
	{
	char buf[BUFFSIZE];
	char data[(HEIGHT / 2) * 16];
	int row;
	unsigned para;
	unsigned offset;
	int cnt;
	char c;

	para = curt->start + curt->cur;
	offset = curt->cur;
	BlockGet(data, sizeof(data), para);
	for (row = 0; row < HEIGHT && offset < curt->num; row++, para++, offset++) {
		xsprintf(buf, "%4x    ", para);
		for (cnt = 0; cnt < 8; cnt++) {
			xsprintf(&buf[8 + cnt * 3], "%2x ",
				data[(row / 2) * 16 + cnt]);
			}
		for (cnt = 0; cnt < 8; cnt++) {
			c = data[(row / 2) * 16 + cnt];
			if (c < SP || c > '~') c = '.';
			buf[32 + cnt] = c;
			}
		m_disp(-1 + row, 0, buf, strlen(buf), 0, 0);
		
		row++;

		xsprintf(buf, " (%4x) ", offset);
		for (cnt = 0; cnt < 8; cnt++) {
			xsprintf(&buf[8 + cnt * 3], "%2x ",
				data[(row / 2) * 16 + cnt + 8]);
			}
		for (cnt = 0; cnt < 8; cnt++) {
			c = data[(row / 2) * 16 + cnt + 8];
			if (c < SP || c > '~') c = '.';
			buf[32 + cnt] = c;
			}
		m_disp(-1 + row, 0, buf, strlen(buf), 0, 0);
		}

	if (row < HEIGHT) {
		m_clear(-1 + row, 0, HEIGHT - (-1 + row), 40); 
		}
	}


/* ------------------------------------------------------------ */

/* Display the characters screen. */

void
Disp_Chars(void)
	{
	char buf[BUFFSIZE];
	int cnt;
	int which;

	for (cnt = 0, which = curt->cur; cnt < HEIGHT && which < 256;
		 cnt++, which++) {
		xsprintf(buf, "%2x  %3dd  %03oo  %s  %c                         ",
			which, which, which, Carat(which & 0xff), which);
		m_disp(-1 + cnt, 0, buf, 40, 0, 0);
		}

	for ( ; cnt < HEIGHT && which < 512; cnt++, which++) {
		xsprintf(buf, "%2x00  %s                                  ",
			which & 0xFF, Label(which & 0xff));
		m_disp(-1 + cnt, 0, buf, 40, 0, 0);
		}

	if (cnt < HEIGHT) {
		m_clear(-1 + cnt , 0, HEIGHT - (-1 + cnt), 40); 
		}
	}


/* ------------------------------------------------------------ */

/* Get a file name using the file getter.  Return True on success or
False on abort. */

FLAG
GetFile(char *prompt, char *fname, FLAG usestar)
	{
	FILEINFO fi[100];	/* Can display up to 100 files at
				once.  Change this constant to change 
				the maximum number of files that can
				be displayed at once.*/
	FMENU f;		/* file menu extra data structure */
	EDITDATA ed;		/* you've seen these */
	EVENT e;
	FLAG isdone = FALSE;
	FLAG ok = TRUE;
	char dn[FNAMEMAX];	/* working copy of the directory part */
	char fn[FNAMEMAX];	/* working copy of the file part */
	char *cptr;		/* scratch variable */

	if (is100) return(GetStr(prompt, fname, ""));

	xstrcpy(dn, fname);	/* make a copy of the name */

		/* This code splits the full name into the directory
		and file parts.  It starts at the end and searches
		backwards until it finds a /, \, or :.  If there isn't
		one, it sets the directory part to "". If there isn't
		a file part, that is set to "". */
	for (cptr = dn + strlen(dn); cptr > dn; --cptr) {
		if (*cptr == ':' || *cptr == '/' || *cptr == '\\') break;
		}
	if (*cptr == ':' || *cptr == '/' || *cptr == '\\') {
		xstrcpy(fn, cptr + 1);
		*(cptr + 1) = NUL;
		}
	else	{
		xstrcpy(fn, cptr);
		*cptr = NUL;
		}
		/* If there is no file part or usestar is True, force
		the file name to "*.*". */
	if (*fn == NUL || usestar) {
		xstrcpy(fn, "*.*");
		}

		/* Initialize the required parts of the file menu
		structure. */
	f.fm_path = dn;		/* directory name part */
	f.fm_pattern = fn;	/* file name part */
	f.fm_buffer = fi;	/* place to put the working names */
	f.fm_buf_size = sizeof(fi);	/* how big (many) */
	f.fm_startline = -2;	/* top line... *sigh* */
	f.fm_startcol = 0;	/* left edge */
	f.fm_numlines = 14;	/* leave 2 lines for 95buddy */
	f.fm_numcols = 40;	/* and all columns */
	f.fm_filesperline = 3;	/* can fit this many across */

		/* Now, set up the edit buffer part. Just copy these
		values... */
	ed.prompt_window = 1;
	ed.prompt_line_length = 0;
	ed.message_line = prompt;
	ed.message_line_length = strlen(prompt);

		/* clear the screen */
	m_clear(-3, 0, 16, 40);

		/* start things up */
	if (fmenu_init(&f, &ed, "", 0, 0) != RET_OK) {
		GetKey("can't init file getter");
		return(FALSE);
		}

		/* the usual, you've seen all this */
	VidCurOff();
	e.kind = E_KEY;
	while (!isdone) {
		if (e.kind == E_KEY || e.kind == E_ACTIV) fmenu_dis(&f, &ed);
		m_event(&e);
		switch (e.kind) {

		case E_ACTIV:
			VidCurOff();
			Refresh();
			break;

		case E_TERM:
			isterm = TRUE;
			isdone = TRUE;
			break;
		
		case E_BREAK:
			ok = FALSE;
			isdone = TRUE;
			break;

		case E_KEY:
			if (e.data == ESC) {	/* handle Esc */
				ok = FALSE;
				isdone = TRUE;
				}
			else	{	/* this routine has multiple
					return values, which tell you
					what to do */
				switch (fmenu_key(&f, &ed, e.data)) {

				case RET_UNKNOWN:	/* bad key */
				case RET_BAD:
					m_thud();
					break;

				case RET_OK:		/* keep goin' */
					break;

				case RET_REDISPLAY:	/* reshow the
							screen */
					break;

				case RET_ACCEPT:	/* done, ok */
					isdone = TRUE;
					break;

				case RET_ABORT:		/* done, cancel */
					ok = FALSE;
					isdone = TRUE;
					break;
					}
				}
			break;
			}
		}

		/* turn off the display, then put ours back */
	fmenu_off(&f, &ed);
	Display();
		/* copy the file name to the return buffer */
	xstrcpy(fname, ed.edit_buffer);
	return(ok);
	}


/* ------------------------------------------------------------ */

/* Get a hexadecimal value in the range 0 to MX.  Return True on
success or False on abort. The value can be two values separated by a
non-alphanumeric.  They are interpreted as (first * 16 + second) / 16
(i.e., seg:offset). */

FLAG
GetHex(char *prompt, unsigned *val, unsigned mx)
	{
	char buf[80];	/* response buffer */
	char buf2[80];	/* default buffer */
	char *cptr;
	unsigned v;
	unsigned tmp;

		/* construct the default value from the passed-in
		(numeric) default value */
	xsprintf(buf2, "%x", *val);
		/* call the routine, using the passed-in prompt */
	if (!GetStr(prompt, buf, buf2)) return(FALSE);
	for (cptr = buf; *cptr != NUL && xisalnum(*cptr); cptr++) ;
	if (*cptr == NUL) {
		if (!SToN(buf, val, 16)) {
			xsprintf(buf2, "'%s' not a hex value", buf);
			GetKey(buf2);
			return(FALSE);
			}
		}
	else	{
		*cptr = NUL;
		if (!SToN(buf, &v, 16)) {
			xsprintf(buf2, "'%s' not a hex value", buf);
			GetKey(buf2);
			return(FALSE);
			}
		tmp = v;
		for (cptr++; *cptr != NUL && !xisalnum(*cptr); cptr++) ;
		if (!SToN(cptr, &v, 16)) {
			xsprintf(buf2, "'%s' not a hex value", cptr);
			GetKey(buf2);
			return(FALSE);
			}
		tmp += v >> 4;
		*val = tmp;
		}
	if (*val >= mx) {
		xsprintf(buf2, "%x not less than %x", *val, mx);
		GetKey(buf2);
		return(FALSE);
		}
	return(TRUE);
	}
	

/* ------------------------------------------------------------ */

/* Get a key. */

int
GetKey(char *str)
	{
	EVENT e;

	m_lock();	/* Lock out application switching.  If the
			user presses a "hot key," it won't do anything. */

			/* Display the message.  The call actually
			displays two lines, although we never use the
			second.  Note that the system manager calls
			tend to take (pointer to string, length of
			string) pairs and not use NUL-terminated
			strings. */
	message(str, strlen(str), "", 0);

	do	{	/* Until you get a key press... */
		m_event(&e);
		} while (e.kind != E_KEY);

	msg_off();	/* Turn off the message, which restores what
			was underneath. */

	m_unlock();	/* Turn application switching back on. */
	return(e.data);	/* Return the keystroke. */
	}


/* ------------------------------------------------------------ */

/* Handle the menu key. */

FLAG
GetMenu(void)
	{
	MENUDATA u;
	EVENT e;
	int which = 0;
	FLAG isdone = FALSE;

			/* This routine initializes the menu
			structure.  Note the use of embedded \0
			characters to delimit the menu entries
			and ANSI concatenated strings to handle
			the hex constants.  \x1a is a right
			arrow character. */
	menu_setup(&u, "C)\x1a" "Clip\0Debug\0ExternalCommand\0F)\x1a"
		"File\0G)\x1a" "Clip(bin)\0H)\x1a" "File(bin)\0Quit\0",
		7, 1, NULL, 0, NULL);

	VidCurOff();	/* turn the cursor off */

	menu_on(&u);	/* turn on the menus */

	e.kind = E_KEY;
	while (!isdone) {
		if (e.kind == E_KEY || e.kind == E_ACTIV) {
			menu_dis(&u);
			m_disp(-1, 0, "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD", 40, 0, 0);
			}
		m_event(&e);		/* get the next event */
		switch (e.kind) {

			/* You've been suspended and are back.  Do a
			full refresh/display build.  When you come
			around again, the menu will be redisplayed by
			means of the above code. */
		case E_ACTIV:
			Refresh();
			Display();
			break;

			/* Got a terminate, so clean up and exit. */
		case E_TERM:
			isterm = TRUE;
			isdone = TRUE;
			break;

			/* Got a key. */
		case E_KEY:
			if (e.data == ESC) {	/* you have to handle */
				which = -1;	/* ESC yourself */
				isdone = TRUE;
				}
			else	{	/* handle all other key
					presses; which is set to
					0..#entries1 when an item is
					selected */
				menu_key(&u, e.data, &which);
				if (which != -1) isdone = TRUE;
				}
			break;
			}
		}

	menu_off(&u);		/* Turn off the menu display; this
				also restores what's underneath, but
				you're going to regenerate it
				anyway. */
	if (isterm) return(TRUE);	/* ESC pressed, so don't do
					anything. */

	switch (which) {

	case 0:
		isbin = FALSE;
		ToClip();
		break;

	case 1:
		Command(is100 ? DEBUG100_CMD : DEBUG95_CMD);
		break;

	case 2:
		Command(NULL);
		break;

	case 3:
		isbin = FALSE;
		ToFile();
		break;

	case 4:
		isbin = TRUE;
		ToClip();
		break;

	case 5:
		isbin = TRUE;
		ToFile();
		break;

	case 6:
		isterm = TRUE;	/* Quit selected. */
		break;
		}
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Get a string.  BUF must be >= 80 characters long.  DEFLT points to
a default value.  Return True on success or False on abort. */

FLAG
GetStr(char *prompt, char *buf, char *deflt)
	{
	EDITDATA ed;		/* our first exposure to this */
	EVENT e;		/* you've seen this one before */
	FLAG isdone = FALSE;
	FLAG ok = TRUE;
	int result;

		/* Set up the edit data structure. It gets loaded
		with the default value, the maximum input length (16),
		the prompt, and the (unused) second prompt line. */
	edit_top(&ed, deflt, strlen(deflt), 16, prompt, strlen(prompt), "", 0);

		/* You've seen this before.  We use edit_dis to
		update the display. */
	e.kind = E_KEY;
	while (!isdone) {
		if (e.kind == E_KEY || e.kind == E_ACTIV) edit_dis(&ed);
		m_event(&e);
		switch (e.kind) {

		case E_ACTIV:
			VidCurOff();
			Refresh();
			break;

		case E_TERM:		/* handle termination */
			isterm = TRUE;
			ok = FALSE;
			isdone = TRUE;
			break;
		
		case E_BREAK:		/* handle break key */
			ok = FALSE;
			isdone = TRUE;
			break;

		case E_KEY:
			if (e.data == ESC) {	/* handle Esc key */
				ok = FALSE;
				isdone = TRUE;
				}
			else	{	/* handle each keystroke */
				edit_key(&ed, e.data, &result);
				if (result == 1) isdone = TRUE;
				}
			break;
			}
		}

	Display();	/* Put the display back: no routine to do
			this for you. */
	if (!ok) return(FALSE);		/* cancel */
	xstrcpy(buf, ed.edit_buffer);	/* have a valid string, so
					copy it to the return
					buffer */
	return(TRUE);
	}


/* ------------------------------------------------------------ */

/* Return a pointer to a static buffer that contains the key label
for the supplied character. */

char *
Label(int chr)
	{
	xstrcpy(chars_buf, labels[chr]);
	if (*chars_buf == NUL) {
		xsprintf(chars_buf, "??%02x00", chr);
		}
	return(chars_buf);
	}


/* ------------------------------------------------------------ */

/* Push the current task onto the last task stack. */

void
Push_Task(void)
	{
	int cnt;

	for (cnt = 0; cnt < NUM_LAST - 1; cnt++) {
		last[cnt] = last[cnt + 1];
		}
	last[NUM_LAST - 1] = curt;
	}


/* ------------------------------------------------------------ */

/* Refresh our data. */

void
Refresh(void)
	{
	int cnt;

	if (is100) {
		cnt = m_get_TCB_size() / sizeof(struct task);
		t[DAPPS].num = cnt;
		if (t[DAPPS].cur >= cnt) t[DAPPS].cur = cnt - 1;
		t[DAPLONG].num = cnt;
		if (t[DAPLONG].cur >= cnt) t[DAPLONG].cur = cnt - 1;

		sys_tasks = m_get_TCB();
		appoff = FP_OFF(sys_tasks);
		appseg = FP_SEG(sys_tasks);
		}
	else	{
		if (GetApp(acbs) < 0) isterm = TRUE;
		}

	m_dirty_sync();
	m_clear(-3, 0, 16, 40);

	Help_Setup();
	t[DHELP].num = Help_Num();
	t[DHELP].cur = 0;
	}


/* ------------------------------------------------------------ */

/* Copy the current data to the clipboard. */

void
ToClip(void)
	{
		/* Start talking to the clipboard. */
	if (m_open_cb() != 0) {
		GetKey("Can't open clipboard");
		return;
		}

		/* Clear the current data, and sign the data that
		we are about to write. */
	if (m_reset_cb("MemUtil") != 0) {
		GetKey("Can't init clipboard");
		m_close_cb();
		return;
		}

		/* Start a text representation (we'll only generate
		the one). */
	m_new_rep("TEXT");

		/* Put up a message as this may take a while. */
	message("copying to clipboard...", 23, "", 0);

		/* This next call tells the display routines to
		actually display the message.  Ordinarily, this detail
		is handled when you ask for input.  You only need to
		make this call if you want the display actually
		updated, but aren't going to ask for input right
		away. */
	m_dirty_sync();

		/* Dispatch.  The arguments are simply passed through
		to To_Write(). */
	switch(curt->disp) {

	case DHELP:	To_Help(-1, 0);		break;
	case DAPPS:	To_Apps(-1, 0);		break;
	case DAPLONG:	To_ApLong(-1, 0);	break;
	case DCHAINS:	To_Chains(-1, 0);	break;
	case DMEM:	To_Mem(-1, 0);		break;
	case DCHARS:	To_Chars(-1, 0);	break;
		}

		/* Close this representation. */
	m_fini_rep();

		/* If you wanted to write another representation,
		start with another m_new_rep() call. */

		/* All done writing the clipboard. */
	m_close_cb();
	}


/* ------------------------------------------------------------ */

/* Copy the current data to a file. */

void
ToFile(void)
	{
	char fname[FNAMEMAX];
	char buf[BUFFSIZE];
	char *prompt;
	int fd;

	if (isbin)
		prompt = "File to save in binary to:";
	else	prompt = "File to save to:";

	if (!GetFile(prompt, fname, TRUE)) return;

	if ((fd = creat(fname, 0666)) < 0) {
		xsprintf(buf, "Can't create '%s'", fname);
		GetKey(buf);
		return;
		}

	message("saving...", 9, "", 0);
	m_dirty_sync();
	switch(curt->disp) {

	case DHELP:	To_Help(fd, -1);	break;
	case DAPPS:	To_Apps(fd, -1);	break;
	case DAPLONG:	To_ApLong(fd, -1);	break;
	case DCHAINS:	To_Chains(fd, -1);	break;
	case DMEM:	To_Mem(fd, -1);		break;
	case DCHARS:	To_Chars(fd, -1);	break;
		}
	close(fd);
	}


/* ------------------------------------------------------------ */

/* Write the help text. Always use text. */

void
To_Help(int fd, int clip)
	{
	int cnt;
	char *cptr;

	isbin = FALSE;

		/* For each line... */
	for (cnt = 0; cnt < Help_Num(); cnt++) {

			/* ...point to the start...  */
		cptr = Help_Line(cnt);

			/* ...call the generic write routine */
		if (!To_Write(fd, clip, (char *)cptr, strlen(cptr))) return;
		}
	}


/* ------------------------------------------------------------ */

/* Write the applications. */

void
To_Apps(int fd, int clip)
	{
	struct acb *a;
	struct task k;
	char buf[BUFFSIZE];
	char nbuf[80];
	char sbuf[80];
	char kbuf[80];
	char *name;
	unsigned key;
	int state;

	if (isbin) {
		if (is100) {
			k = sys_tasks[curt->cur];
			To_Write(fd, clip, (char *)&k, sizeof(k));
			}
		else	{
			a = &acbs[curt->cur];
			To_Write(fd, clip, (char *)a, sizeof(struct acb));
			}
		return;
		}

	if (is100) {
		k = sys_tasks[curt->cur];
		name = k.t_extname;
		key = k.t_hotkey;
		state = k.t_state;
		}
	else	{
		a = &acbs[curt->cur];
		name = a->name;
		key = a->hot_key;
		state = a->state;
		}

	if (*name == NUL)
		xstrcpy(kbuf, "             ");
	else	{
		if (key >= 256) {
			xstrncpy(kbuf, Label((key >> 8) & 0xff));
			}
		else	{
			xstrcpy(kbuf, Carat(key));
			}
		kbuf[13] = NUL;
		strcat(kbuf, "             ");
		kbuf[13] = NUL;
		}

	if (*name == NUL) {
		xstrcpy(nbuf, "[none]         ");
		}
	else	{
		xstrncpy(nbuf, name);
		nbuf[15] = NUL;
		strcat(nbuf, "               ");
		nbuf[15] = NUL;
		}

	if (*name == NUL) {
		xstrcpy(sbuf, "         ");
		}
	else	{
		xstrncpy(sbuf, ApState(state));
		sbuf[9] = NUL;
		strcat(sbuf, "         ");
		sbuf[9] = NUL;
		}

	xsprintf(buf, "%s %s  %s", kbuf, nbuf, sbuf);
	if (!To_Write(fd, clip, buf, strlen(buf))) return;
	}


/* ------------------------------------------------------------ */

/* Write the long applications text. */

void
To_ApLong(int fd, int clip)
	{
	struct acb *a;
	struct task k;
	char buf[BUFFSIZE];
	char buf3[128];
	char fname[30];		/* exactly legal apname file name size + 1 */
	unsigned amt;
	int cnt;

	if (isbin) {
		if (is100) {
			k = sys_tasks[curt->cur];
			To_Write(fd, clip, (char *)&k, sizeof(k));
			}
		else	{
			a = &acbs[curt->cur];
			To_Write(fd, clip, (char *)a, sizeof(struct acb));
			}
		return;
		}

	if (is100) {
		k = sys_tasks[curt->cur];

		xsprintf(buf, "%s, state %s, hot key %s",
			k.t_extname,
			ApState(k.t_state),
			k.t_hotkey >= 256 ? Label((k.t_hotkey >> 8) & 0xff) :
				Carat(k.t_hotkey));
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		if (k.t_seg_image == 0) {
			xstrcpy(buf, "no file");
			}
		else	{
			amt = k.t_seg_image + (k.t_off_image >> 4);
			BlockGet(buf3, sizeof(buf3), amt);
			amt = k.t_off_image & 0xf;
			xstrncpy(fname, &buf3[amt + 56]);
			xstrcpy(buf, fname);
			}
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "stack %x:%x, far size %x, far off %x",
			k.t_ss, k.t_sp, k.t_far_size, k.t_far_off);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "imagevec %x:%x, far rsvrd %x",
			k.t_seg_image, k.t_off_image, k.t_far_rsvrd);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "DS %x, page count %x %x",
			k.t_ds, k.t_pagecount[0], k.t_pagecount[1]);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "mem seg %x, rsrc seg %x, ver %x",
			k.t_memseg, k.t_rsrc_seg, k.t_sys_ver);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "flags %x, no wait %x, lock %x",
			k.t_is123, k.t_nowait, k.t_lock_state);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "phy page %x %x, logic page %x %x",
			k.t_phypage[0], k.t_phypage[1],
			k.t_logicalpage[0], k.t_logicalpage[1]);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "chip selection %2x %2x, ic u loc %x",
			k.t_chipsel[0], k.t_chipsel[1], k.t_ic_u_loc);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "sp status %x, sp link %x, ic o loc %x",
			k.t_sp_status, k.t_sp_link, k.t_ic_o_loc);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;
		}
	else	{
		a = &acbs[curt->cur];
		xsprintf(buf, "%s, state %s, hot key %s",
			a->name,
			ApState(a->state),
			a->hot_key >= 256 ? Label((a->hot_key >> 8) & 0xff) :
				Carat(a->hot_key));
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		if (a - acbs < DEFAULTAPP || a->imagev_seg == 0) {
			xstrcpy(buf, "no file");
			}
		else	{
			amt = a->imagev_seg + (a->imagev_off >> 4);
			BlockGet(buf3, sizeof(buf3), amt);
			amt = a->imagev_off & 0xf;
			xstrncpy(fname, &buf3[amt + 56]);
			xstrcpy(buf, fname);
			}
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "stack %x:%x, imagevec %x:%x, DS %x",
			a->ss, a->sp,
			a->imagev_seg, a->imagev_off,
			a->ds);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "mem seg %x, rsrc seg %x, flags %x, no wait %x",
			a->mem_seg, a->rsrc_seg, a->flags, a->nowait);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "memory mapping %2x %2x %2x %2x %2x %2x",
			a->membank[0], a->membank[1], a->membank[2],
			a->membank[3], a->membank[4], a->membank[5]);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		xsprintf(buf, "chip selection %2x %2x %2x %2x %2x %2x",
			a->chipsel[0], a->chipsel[1], a->chipsel[2],
			a->chipsel[3], a->chipsel[4], a->chipsel[5]);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;

		if (!To_Write(fd, clip, NULL, 0)) return;
		}
	}


/* ------------------------------------------------------------ */

/* Write the chains.  Always use text. */

void
To_Chains(int fd, int clip)
	{
	char buf[BUFFSIZE];
	int cnt;

	isbin = FALSE;
	for (cnt = 0; cnt < curt->num; cnt++) {
		xsprintf(buf, "at %4x  len %4x  %4x:%s",
			links[cnt].seg,
			links[cnt].size,
			links[cnt].owner,
			links[cnt].name);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;
		}
	}


/* ------------------------------------------------------------ */

/* Write the memory block. */

void
To_Mem(int fd, int clip)
	{
	char buf[BUFFSIZE];
	char buf2[80];
	unsigned seg;
	int amt;
	int cnt;
	char c;

	if (isbin) {
		for (seg = curt->start; seg < curt->start + curt->num;
			 seg += amt ) {
			amt = min(sizeof(buf) / 16,
				curt->start + curt->num + 1 - seg);
			BlockGet(buf, amt * 16, seg);
			if (!To_Write(fd, clip, buf, amt * 16)) return;
			}
		}
	else	{
		for (seg = curt->start; seg < curt->start + curt->num; seg++ ) {
			BlockGet(buf, sizeof(buf), seg);
			xsprintf(buf2, "%4x  ", seg);

			for (cnt = 0; cnt < 16; cnt++) {
				xsprintf(&buf2[6 + cnt * 3], "%2x ", buf[cnt]);
				}
			for (cnt = 0; cnt < 16; cnt++) {
				c = buf[cnt];
				if (c < SP || c > '~') c = '.';
				buf2[54 + cnt] = c;
				}
			buf2[70] = NUL;
			if (!To_Write(fd, clip, buf2, strlen(buf2))) return;
			}
		}
	}


/* ------------------------------------------------------------ */

/* Write the characters. Always use text. */

void
To_Chars(int fd, int clip)
	{
	char buf[BUFFSIZE];
	int which;

	isbin = FALSE;
	for (which = 0; which < 256; which++) {
		xsprintf(buf, "%2x  %3dd  %03oo  %s  %c",
			which, which, which, Carat(which & 0xff), which);
		if (!To_Write(fd, clip, buf, strlen(buf))) return;
		}

	for ( ; which < 512; which++) {
		xsprintf(buf, "%2x00  %s", which & 0xff,
			Label(which & 0xff));
		if (!To_Write(fd, clip, buf, strlen(buf))) return;
		}
	}


/* ------------------------------------------------------------ */

/* Write the data, checking for errors.  If ISBIN is not true, assume
that you are writing a complete line and append the newline sequence. 
LEN can be at most 1024. */

FLAG
To_Write(int fd, int clip, char *buf, int len)
	{
		/* Handle file writes. */
	if (fd >= 0) {

			/* Write the data. */
		if (len > 0 && write(fd, buf, len) != len) {
			GetKey("Write Error");
			return(FALSE);
			}

			/* If not binary, write a newline */
		if (!isbin) {
			if (write(fd, "\r\n", 2) != 2) {
				GetKey("Write Error");
				return(FALSE);
				}
			}

			/* Done. */
		return(TRUE);
		}

		/* Write to the clipboard. */
	if (clip >= 0) {

			/* Write the data. */
		if (len > 0 && m_cb_write(buf, len) != 0) {
			GetKey("Write Error");
			return(FALSE);
			}

			/* If not binary, write a newline */
		if (!isbin) {
			if (m_cb_write("\r", 1) != 0) {
				GetKey("Write Error");
				return(FALSE);
				}
			}

			/* Done. */
		return(TRUE);
		}
	GetKey("Broken Writing...");
	return(FALSE);
	}


/* end of MEMUTIL.C -- HP95LX System-Manager Compliant Memory Utility */
