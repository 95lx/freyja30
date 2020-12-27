/* FREYJA.C -- Freyja Text Editor

	Written March 1991 by Craig A. Finseth
	Copyright 1991,2,3,4 by Craig A. Finseth
*/

#include "freyja.h"

void edit(void);
FLAG RInit(char *rfile);

/* ------------------------------------------------------------ */

#if !defined(SYSMGR)
int
main(argc,argv)
	int argc;
	char *argv[];
	{
	int cnt;
	FLAG any = FALSE;
	char *rfile = RES_FILENAME;
	char *ifile;

	swap_size = -1;
	key_method = NUL;
	screen_type = 102;
	xprintf("Freyja, Copyright 1991,2,3 by Craig A. Finseth\n");

	if (argc < 1) {
usage:
		xprintf("usage is: freyja [-options] [<file(s)>]\n\
	-k <type>	key input method\n\
	-r <file>	specify alternate resource file\n\
	-s <type>	screen type\n\
	-z <size>	edit area size in Kbytes\n"
			);
		exit(1);
		}

/* arguments */

	for (cnt = 1; cnt < argc; cnt++) {
		if (*argv[cnt] == '-') {
			if (cnt + 1 >= argc) {
				xprintf("Missing value.\n");
				goto usage;
				}
			}
		if (strequ(argv[cnt], "-k")) {
			key_method = xtoupper(*argv[++cnt]);
			}
		else if (strequ(argv[cnt], "-r")) {
			rfile = argv[++cnt];
			}
		else if (strequ(argv[cnt], "-s")) {
			if (!SToN(argv[++cnt], &screen_type, 10)) {
				xprintf("Screen type must be an integer.\n");
				goto usage;
				}
			}
		else if (strequ(argv[cnt], "-z")) {
			if (!SToN(argv[++cnt], &swap_size, 10)) {
				xprintf("Buffer size must be an integer.\n");
				goto usage;
				}
			}
		else if (*argv[cnt] == '-') {
			xprintf("Unrecognized option '%s'.\n", argv[cnt]);
			goto usage;
			}
		}

/* now can initialize stuff */

	if (!RInit(rfile)) exit(1);
	if (!TInit()) {
		xprintf("Can't init terminal %d\n", screen_type);
		exit(1);
		}
	if (!BInit(swap_size)) {
		DError(RES_CANTINIT);
		TFini();
		exit(1);
		}

	DInit1();
	cbuf = NULL;
	kill_buf = BBufCreate(Res_String(NULL, RES_MSGS, RES_SYSKILL));

/* now load files from cmd line */

	for (cnt = 1; cnt < argc; cnt++) {
		if (*argv[cnt] == '-') {
			++cnt;
			}
		else	{
			if (BBufCreate(argv[cnt]) != NULL) BFileRead();
			any = TRUE;
			}
		}

	if (!any) {
		ifile = (char *)Res_String(NULL, RES_CONF, RES_INITFILE);
		if (*ifile != NUL) {
			if (BBufCreate(ifile) != NULL) BFileRead();
			any = TRUE;
			}
		}
	if (!any) {
		BBufCreate(Res_String(NULL, RES_MSGS, RES_SYSSCRATCH));
		}

	DInit2();
#if !defined(NOCALC)
	UInit();
#endif

#if defined(MSDOS)
	if (c.g.madefor == 'D' || c.g.madefor == 'J') JInit();
#endif

	lastkey = 0;
	lasttable = 0;

	*stringarg = NUL;
	*filearg = NUL;

	DNewDisplay();
	if (!any) {
		DShow('T', Res_String(NULL, RES_MSGS, RES_SCRATCH), FALSE);
		KPush(KGetChar());
		}

	edit();			/* do the actual editing */

#if defined(MSDOS)
	if (c.g.madefor == 'D' || c.g.madefor == 'J') JFini();
#endif
	DFini();
	BFini();
	TSetPoint(TMaxRow() - 1,0);
	TFini();
#if defined(MSDOS)
	_exit(0);
#else
	exit(0);
#endif
	}
#else
unsigned int errno;
unsigned int __brklvl;
unsigned char *environ = "";

void
main(void)
	{
	FLAG any = FALSE;
	char *ifile;

	swap_size = -1;
	key_method = NUL;
	screen_type = 102;

	JInit();
	if (!RInit(RES_FILENAME)) JFini();
	TInit();
	if (!BInit(swap_size)) JFini();

	DInit1();
	cbuf = NULL;
	kill_buf = BBufCreate(Res_String(NULL, RES_MSGS, RES_SYSKILL));

	ifile = (char *)Res_String(NULL, RES_CONF, RES_INITFILE);
	if (*ifile != NUL) {
		if (BBufCreate(ifile) != NULL) BFileRead();
		any = TRUE;
		}
	else	{
		BBufCreate(Res_String(NULL, RES_MSGS, RES_SYSSCRATCH));
		}
	DInit2();

	lastkey = 0;
	lasttable = 0;

	*stringarg = NUL;
	*filearg = NUL;

	DNewDisplay();
	if (!any) {
		DShow('T', Res_String(NULL, RES_MSGS, RES_SCRATCH), FALSE);
		KPush(KGetChar());
		}

	edit();			/* do the actual editing */

	BFini();
	JFini();
	DFini();
	BFini();
	TSetPoint(TMaxRow() - 1,0);
	TFini();
	}
#endif


/* ------------------------------------------------------------ */

/* This does the actual editing, we assume that the buffer is
initialized. */

void
edit(void)
	{
	struct buffer *lbuf = NULL;

	doabort = FALSE;

	uarg = 1;
	KMacDo(RES_CONF, RES_INITMACRO);

	while (!doabort) {
		if (cbuf->c.fill == 'W') WWrap();
		if (cbuf != lbuf) DModeLine();
		lbuf = cbuf;

		DIncrDisplay();

		table = 0;
		do	{
			key = KGetChar();
			if (key == KEYQUIT) {
				MExit();
				return;
				}
			} while (key < 0);
		uarg = 1;
		isuarg = FALSE;
		isrepeating = FALSE;
		while (uarg > 0) {
			TabDispatch(key, table);
			if (--uarg < 0) uarg = 0;
			isrepeating = TRUE;
			}
		lastkey = key;
		lasttable = table;
		}
	}


/* ------------------------------------------------------------ */

/* Load the specified resource file.  If an error, print a message and
use defaults. */

FLAG
RInit(char *rfile)
	{
	unsigned char *fkeys;
	unsigned char *cptr;
	char fname[FNAMEMAX];
	int fd;

	*fname = NULL;
	if ((fd = FPathOpen(rfile, fname)) < 0) {
#if !defined(SYSMGR)
		xeprintf("Unable to open resource file '%s'.\n", RES_FILENAME);
#endif
		return(FALSE);
		}		
	close(fd);
	if (Res_Load(fname) < 0) {
#if !defined(SYSMGR)
		xeprintf("Unable to load resource file '%s'.\n", fname);
#endif
		return(FALSE);
		}

	if (swap_size == -1) swap_size = Res_Number(RES_CONF, RES_SWAPSIZE);
	if (key_method == NUL) key_method = Res_Char(RES_CONF, RES_KEYINP);
	if (screen_type == 102)
		screen_type = Res_Number(RES_CONF, RES_SCRNTYPE);

	c.g.madefor = Res_Char(RES_CONF, RES_MADEFOR);
	c.g.ESC_swap = Res_Number(RES_CONF, RES_ESCSWAP);
	c.g.CTX_swap = Res_Number(RES_CONF, RES_CTXSWAP);
	c.g.meta_handle = Res_Char(RES_CONF, RES_METAKEY);
	c.g.vis_gray = Res_Char(RES_CONF, RES_VISGRAY) == 'Y';
	c.g.vis_nl_char = Res_Number(RES_CONF, RES_VIS_NL_CHAR);
	c.g.vis_tab_char = Res_Number(RES_CONF, RES_VIS_TAB_CHAR);
	c.g.vis_sp_char = Res_Number(RES_CONF, RES_VIS_SP_CHAR);
	c.g.skip_system = Res_Char(RES_CONF, RES_SKIPSYS) == 'Y';
	c.g.wordend = Res_Char(RES_CONF, RES_WORDEND) == 'Y';
	c.g.ctype = (char *)Res_String(NULL, RES_CSET, RES_CTYPE);
	c.g.lower = (char *)Res_String(NULL, RES_CSET, RES_TOLOW);
	c.g.upper = (char *)Res_String(NULL, RES_CSET, RES_TOUPP);

	c.d.left_margin = Res_Number(RES_CONF, RES_LMAR);
	c.d.right_margin = Res_Number(RES_CONF, RES_RMAR);
	c.d.tab_spacing = Res_Number(RES_CONF, RES_TABW);
	c.d.fill = Res_Char(RES_CONF, RES_FILL);
	DSetup(Res_Number(RES_CONF, RES_CALSTART));
	printer = Res_Number(Res_Number(-1, -1) - 1, 2);

/* do some consistency checking */
	if (swap_size < 16) swap_size = 16;

	if (Res_Char(RES_CONF, RES_HOMEEND) == 'Y') {
		fkeys = Res_String(NULL, RES_FUNC, 0);
		*(fkeys + (KEYHOME & 0xFF)) = RES_CLINEA;
		*(fkeys + (KEYEND & 0xFF)) =  RES_CLINEE;
		*(fkeys + (KEYCHOME & 0xFF)) = RES_FBUFBEG;
		*(fkeys + (KEYCEND & 0xFF)) = RES_FBUFEND;
		}

	if (Res_Char(RES_CONF, RES_SWAPPGLN) == 'Y') {
		togpgln = 2;
		MTogPgLn();
		}
	else	{
		togpgln = 0;
		}

	if (*(cptr = Res_String(NULL, RES_CONF, RES_DEFDIR)) != NUL) {
#if defined(UNIX)
		chdir(cptr);
#endif
#if defined(MSDOS)
		PSystem(0x3B, cptr);
#endif
		}
		
	WFixup(&c.d);
	return(TRUE);
	}


/* end of FREYJA.C -- Freyja Text Editor */
