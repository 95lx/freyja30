#			 Resource Source File
#		 Freyja Danish / dansk Language Text

# ------------------------------------------------------------
# Messages Table
#
# This table holds all language-dependant messages.

table	MSGS

# system buffer names

SYSBUFFLIST	"%bufferliste%"
SYSCAL		"%kalender%"
SYSDIRED	"%katalog%"
SYSHELP		"%hj^/aelp%"
SYSINCLUDE	"%inds^/aet%"
SYSKILL		"%slet%"
SYSSCRATCH	"%navnl^/oes%"

# freyja.c
CANTINIT	"Fejl under start"
SCRATCH		"Freyja 3.0, tekstbehandling som i Emacs\n\
\n\
Tast ^X ^C for at afslutte\n\
	^^ for menuer\n\
	^X H for hj^/aelp\n\
--------------------------------------\n\
Copyright 1991,2,3,4,6 by Craig A. Finseth\n\
1343 Lafond, St Paul MN 55104, USA\n\
fin@finseth.com\n\
+1 500 448 9440\n\
+1 612 644 4027\n"

FKEYHELP	_u
"F1	hj^/aelp\n\
F2	slet denne linie\n\
F3	reducer ordmellemrummet til et enkelt blanktegn\n\
F4	inds^/aet en tom linie ovenover med samme indrykning\n"
		   _cs
"F1	hj^/aelp\n\
F2	slet denne linie\n\
F3	reducer ordmellemrummet til et enkelt blanktegn\n\
F4	inds^/aet en tom linie ovenover med samme indrykning\n\
F5	g^/aa til n^/aeste buffer		Skift-F5	slet denne buffer\n\
F6	skift til det andet vindue	Skift-F6	skift ml. 1/2 vinduer\n\
F7	s^/oeg forl^/aens\n\
F8	gem bufferen			Skift-F8	find en fil\n\
					   Skift-F9	kalender\n\
F10	vis hovedmenuen			Skift-F10	afslut\n\
		   Ctrl-F2		kopier omr^/aadet til klippebordet\n\
		   Ctrl-F3		flyt omr^/aadet til klippebordet: klip\n\
		   Ctrl-F4		hent fra klippebordet: klistre\n\
		   Ctrl-F5		s^/oeg bagl^/aens\n\
		   Ctrl-F8              gem alle buffere\n\
		   Ctrl-F9		s^/aet m^/aerket her\n\
		   Ctrl-F10	inds^/aet et sideskift\n"
		   _*
"F1	hj^/aelp\n\
F2	slet denne linie\n\
F3	reducer ordmellemrummet til et enkelt blanktegn\n\
F4	inds^/aet en tom linie ovenover med samme indrykning\n\
F5	g^/aa til n^/aeste buffer		Skift-F5	slet denne buffer\n\
F6	skift til det andet vindue	Skift-F6	skift ml. 1/2 vinduer\n\
F7	s^/oeg forl^/aens\n\
F8	gem bufferen			Skift-F8	find en fil\n\
F9	RPN-regnemaskine		Skift-F9	kalender\n\
F10	vis hovedmenuen			Skift-F10	afslut\n\
		   Ctrl-F2		kopier omr^/aadet til slettebufferen\n\
		   Ctrl-F3		flyt omr^/aadet til slettebufferen: klip\n\
		   Ctrl-F4		hent fra slettebufferen: klistre\n\
		   Ctrl-F5		s^/oeg bagl^/aens\n\
		   Ctrl-F8              gem alle buffere\n\
		   Ctrl-F9		s^/aet m^/aerket her\n\
		   Ctrl-F10	inds^/aet et sideskift\n"

FKEYHELP40	_u
"F1	hj^/aelp\n\
F2	slet denne linie\n\
F3	reducer ordmellemrummet til et enkelt blanktegn\n\
F4	inds^/aet en tom linie ovenover med samme indrykning\n"
		   _sc
"\n\
\n\
C:Hj^/aelp  Klip   \x1bS^/oeg    S^/oeg\x1a    M^/aerke\n\
C:  Kopier  Klistr         GemAlt SideSk\n\
\n\
S:              -Buf            Kalen\n\
S:                  1/2Vin  ^/Aaben  Afslut\n\
\n\
Hj^/aelp  OrdMel   Buf\x1a    S^/oeg\x1a\n\
    -Lin    +Lin    Vin\x18\x19   Gem     Menu\n"
		   _*
"\n\
\n\
C:Hj^/aelp  Klip   \x1bS^/oeg    S^/oeg\x1a    M^/aerke\n\
C:  Kopier  Klistr         GemAlt SideSk\n\
\n\
S:              -Buf            Kalen\n\
S:                  1/2Vin  ^/Aaben  Afslut\n\
\n\
Hj^/aelp  OrdMel   Buf\x1a    S^/oeg\x1a    Regn\n\
    -Lin    +Lin    Vin\x18\x19   Gem     Menu\n"

# fbuf.c
ASKOVER		"Overskrive ^/aendret/eksisterende fil? "
BADMARK		"Kludder i m^/aerket"
ERRWRITE	"Fejl under skrivning af fil"
INTPAGEERR	"Intern fejl (paging)"
NODELKILL	"Slettebufferen kan ikke slettes"
NOFREE		"Ingen ledige buffere"
NOMARKS		"Ingen ledige m^/aerker"
NOSWAP		"Ikke plads til mere"
NOTBUFF		"Det er ikke en buffer"
NOTCOPYSAME	"Kopiering skal v^/aere til en anden buffer"
NOTMARK		"Det er ikke et m^/aerke"
NULLPAGE	"Kludder i siden"

# fcalc.c
CALCDIVZERO	_sc ""
		   _* "Division med nul"
CALCFLAG	_sc ""
		   _* "Ugyldigt flag-nr."
CALCINVBIN	_sc ""
		   _* "Ugyldigt bin^/aert tal"
CALCMODZERO	_ss ""
		   _* "Mod med nul"
CALCNEG		_sc ""
		   _* "Negativt"
CALCPCTZERO	_sc ""
		   _* "Procent af nul"
CALCRANGE	_sc ""
		   _* "Uden for gyldighedsomr^/aadet"
CALCUNKCMD	_sc ""
		   _* "Ukendt kommando"
CALCUNKPROG	_sc ""
		   _* "ukendt program"
CALCUNKREG	_sc ""
		   _* "Ukendt register"
UNKINDREG	_sc ""
		   _* "Ukendt indirekte-register"

# fdisplay.c
FREYJAVER	_u "Freyja ###%% *+^ >%d <%d %s %s"
		   _* "Freyja ###%% *+^ \x10%d \x11%d %s %s"
DISPFILL	"H^/aard"
DISPNONE	"Ingen"
DISPWRAP	"Oml^/oeb"

# ffile.c
ASKTHROW	"Skal ^/aendringer smides v^/aek? "
ERRDELETE       "Kan ikke slette"
ERRINS		"Kan ikke oprette inds^/aetningsbuffer"
ERRREAD		"L^/aesefejl"
ERROPEN		"Kan ikke finde filen"
ERRSAVE		"Kan ikke gemme filen"
LISTERR		"Kan ikke lave katalogoversigt"
PROMPTCMD	"Kommandolinie"
PROMPTDELETE    "Slette hvilken fil"
PROMPTDELMOD	"Skal ^/aendret buffer slettes? "
PROMPTDIRNAME	"Fil/katalog-navn"
PROMPTFIND	_cs "File to Find"
		   _*  "Finde hvilken fil"
PROMPTINS	"Inds^/aette hvilken fil"
PROMPTREAD	"L^/aese hvilken fil"
PROMPTWRITE	"Skrive hvilken fil"
RETURN		"[<Retur> for at returnere til Freyja]"
WRITING		"Skriver nu filen..."

# fkey.c
ERRNONNUM	"Det er ikke et tal"
MACROCREATE	"Opretter makro"
MACROFULL	"Tastatur-makro fuld"
MACROUSE	"Anvender makro"
NOSTRING	"\x8\x7fN"
PROMPTARG	"Arg: %d"
YESSTRING	" J"

# fmisc.c
ABOUTSTR	"Freyja tekstbehandling, Version 3.0\n\
\n\
k^/oerer: for %c, tast. %c, sk^/aerm %d %c (%d rk., %d kol.)\n\
bruger %d af %d swap-omr^/aader, ^/a' %d KBytes\n\
er i katalog %s\n\
\n\
Copyright 1991,2,3,4,6 by Craig A. Finseth\n\
1343 Lafond, St Paul MN 55104, USA\n\
fin@finseth.com\n\
+1 500 448 9440\n\
+1 612 644 4027\n\n"

ASKCONFIRM	"Skal ^/aendringer bekr^/aeftes? "
ASKSAVEEXIT	"Skal der gemmes f^/oer der afsluttes? "
ASKSURE		"Er du sikker? "
CHARAT		"  %d decimalt, %x hex, %o oktalt"
DISPLCMDS	_js "J)ja N)nej ,)pr^/oev !)alle .)slut ;)ja&slut"
		   _*  "Kommandoer: J)ja N)nej ,)pr^/oev !)alle .)afslut ;)ja&afslut"
DISPLREPL	_js "Ret '%s' t. '%s'"
		   _*  "Erstat '%s' med '%s'"
EXPLARG		"Der skal angives et argument."
NOTCMD		"er ikke en kommando"
PROMPTAPROPOS	"Apropos"
PROMPTCTX	"Ctrl-X:"
PROMPTESC	"Meta:"
PROMPTFWDSRCH	"Forl^/aens s^/oegning"
PROMPTKEY	"Tast at beskrive: "
PROMPTQUERY	"S^/oegning med bekr^/aeftelse"
PROMPTQUOTE	"Inds^/aet:"
PROMPTREPL	"S^/oeg og erstat"
PROMPTREVSRCH	"Bagl^/aens s^/oegning"
PROMPTWITH	"med"
SELFINSERT	"inds^/aetter sig selv"
STRNOTFOUND	"Teksten er der ikke"
UNKCMD		"Ukendt kommando"
WORKING		"Vent et ^/oejeblik..."

# fregion.c
PRINTDOC	"%s, %4d-%02d-%02d %2d:%02d:%02d, page %d"
PRINTLET	"- %d -"
PROMPTSREG	"Hvilken fil skal omr^/aadet gemmes i? "

# ftable.c
ABORTHELP	"afbryd, fortryd"
ABORTKEY	"Fortryd"
QUITHELP	"afslut programmet"
QUITKEY		"Afslut"

# fwhite.c
DISPLCOLS	_js	"Margen %d-%d  Tab %d  %s%s%s"
		   _* "Venstre margen %d   H^/oejre margen %d   Tabulator %d   %s%s%s"
DISPLLINE	_js	"Linie %u  Ialt %u  M^/aerke %u "
		   _*	"Er p^/aa linie %u   Linier ialt: %u   M^/aerke p^/aa linie %u "

DISPLPOINT	_js	"Pos %l  Lgd %l  Kol %d  M^/aerke %l"
		   _*	"Position %l   L^/aengde %l   Kolonne %d   M^/aerke %l "
DISPLRULER	"Lineal er indsat"
ERRFILL		"Ukendt ombrydningsm^/aade"

# smjaguar.c
CLIPINIT	_sc "kan ikke initialisere klippebordet"
		   _* ""
CLIPOPEN	_sc "kan ikke ^/aabne klippebordet"
		   _* ""
GETTER		_sc "kan ikke initialisere 'file getter'"
		   _* ""
PASTE		_sc "der er ikke noget at klistre ind"
		   _* ""


# ------------------------------------------------------------
# Months Table
#
# This table holds the names of the months.

table	MONTHS
-	"januar"
-	"februar"
-	"marts"
-	"april"
-	"maj"
-	"juni"
-	"juli"
-	"august"
-	"september"
-	"oktober"
-	"november"
-	"december"


# ------------------------------------------------------------
# Days Table
#
# This table holds the names of the days of the week, first is
# Sunday.  These entries should not be more than four characters long.
# They will ordinarily be three characters and a trailing space.

table	DAYS
-	_jsi "s^/oen "
	   _* "soen"
-	"man "
-	"tir "
-	"ons "
-	"tor "
-	"fre "
-	_jsi "l^/oer "
	   _* "loer"


# ------------------------------------------------------------
# Key Labels Table
#
# This table holds the labels to display for all function keys.
# Note that there must be 255 entries in this table.

table	KEYLABEL

	   # 0
- "^0"		- "^1"		- "^2"		- "Ctrl-2"	- "^4"
- "^5"		- "^6"		- "^7"		- "^8"		- "^9"
	   # 10
- "^10"		- "^11"		- "^12"		- "^13"		- "^14"
- "Skift-Tab"	- "Alt-Q"	- "Alt-W"	- "Alt-E"	- "Alt-R"
	   # 20
- "Alt-T"	- "Alt-Y"	- "Alt-U"	- "Alt-I"	- "Alt-O"
- "Alt-P"	- ""		- "	"	- ""		- ""
	   # 30
- "Alt-A"	- "Alt-S"	- "Alt-D"	- "Alt-F"	- "Alt-G"
- "Alt-H"	- "Alt-J"	- "Alt-K"	- "Alt-L"	- ""
	   # 40
- ""		- ""		- "	"	- ""		- "Alt-Z"
- "Alt-X"	- "Alt-C"	- "Alt-V"	- "Alt-B"	- "Alt-N"
	   # 50
- "Alt-M"	- ""		- ""		- ""		- ""
- ""		- ""		- ""		- ""		- "F1"
	   # 60
- "F2"		- "F3"		- "F4"		- "F5"		- "F6"
- "F7"		- "F8"		- "F9"		- "F10"		- ""
	   # 70
- ""		- "Home"	- "Pil op"	- "PgUp"	- ""
- "Venstrepil"	- ""		- "H^/oejrepil"	- ""		- "End"
	   # 80
- "Pil ned"	- "PgDn"	- "Ins"		- "Del"		- "Skift-F1"
- "Skift-F2"	- "Skift-F3"	- "Skift-F4"	- "Skift-F5"	- "Skift-F6"
	   # 90
- "Skift-F7"	- "Skift-F8"	- "Skift-F9"	- "Skift-F10"	- "Ctrl-F1"
- "Ctrl-F2"	- "Ctrl-F3"	- "Ctrl-F4"	- "Ctrl-F5"	- "Ctrl-F6"
	   # 100
- "Ctrl-F7"	- "Ctrl-F8"	- "Ctrl-F9"	- "Ctrl-F10"	- "Alt-F1"
- "Alt-F2"	- "Alt-F2"	- "Alt-F4"	- "Alt-F5"	- "Alt-F6"
	   # 110
- "Alt-F7"	- "Alt-F8"	- "Alt-F9"	- "Alt-F10"	- "Ctrl-PrtSc"
- "Ctrl-Venstrepil"	- "Ctrl-H^/oejrepil"	- "Ctrl-End"	- "Ctrl-PgDn"
- "Ctrl-Home"
	   # 120
- "Alt-1"	- "Alt-2"	- "Alt-3"	- "Alt-4"	- "Alt-5"
- "Alt-6"	- "Alt-7"	- "Alt-8"	- "Alt-9"	- "Alt-0"
	   # 130
- "Alt--"	- "Alt-="	- "Ctrl-PgUp"	- ""		- ""
- ""		- ""		- ""		- ""		- ""
	   # 140
- ""		- "Ctrl-PgUp"	- "Ctrl--"	- ""		- "Alt-+"
- "Ctrl-PgDn"	- "Ctrl-Ins"	- ""		- ""		- ""
	   # 150
- "Ctrl-*"	- ""		- ""		- ""		- ""
- ""		- ""		- ""		- ""		- ""
	   # 160
- ""		- ""		- "Ctrl-On"	- ""		- _cd "&..."
								     _*  "SETUP"
- "Alt-Tab"	- ""		- "Alt-&..."	- "FILER"	- ""
	   # 170
- "Ctrl-&..."	- "Alt-FILER"	- _cd "cc:MAIL"
				     _*  "COMM"	- ""		- "Ctrl-FILER"
- _cd "Alt-cc:MAIL"
  _*  "Alt-COMM" - "APPT"	- ""		- _cd "Ctrl-cc:MAIL"
						     _*  "Ctrl-COMM" - "Alt-APPT"
	   # 180
- "PHONE"	- ""		- "Ctrl-APPT"	- "Alt-PHONE"	- "MEMO"
- ""		- "Ctrl-PHONE"	- "Alt-MEMO"	- "123"	- ""
	   # 190
- "Ctrl-MEMO"	- "Alt-123"	- _cd "HP CALC"
				     _*  "CALC"	- ""		- "Ctrl-123"
- _cd "Alt-HP CALC"
  _*  "Alt-CALC" - ""		- ""		- _cd "Ctrl-HP CALC"
						     _*  "Ctrl-CALC" - ""
	   # 200
- "MENU"	- "Skift-MENU"	- "Ctrl-MENU"	- "Alt-MENU"	- ""
- ""		- ""		- ""		- "ZOOM"	- "DATE"
	   # 210
- "TIME"	- ""		- "CUT"		- "COPY"	- "PASTE"
- ""		- ""		- ""		- ""		- ""
	   # 220
- ""		- ""		- ""		- ""		- ""
- ""		- ""		- ""		- ""		- ""
	   # 230
- ""		- ""		- ""		- ""		- ""
- ""		- ""		- ""		- ""		- ""
	   # 240
- ""		- ""		- ""		- ""		- ""
- ""		- ""		- ""		- ""		- ""
	   # 250
- ""		- ""		- ""		- ""		- ""
- ""


# ------------------------------------------------------------
# Help Strings Table
#

# This table holds the help strings on all commands.  The labels
# defined here are used to as values in the command dispatch tables.
#
#	ALSO: CHANGES IN THIS LIST MUST BE REFLECTED IN FTABLE.C
#
# Except for the first two, the symbols in this table are the
# upper-cased versions of the names of the routines invoked by the
# command.  The entries are in alphabetical order.

table		HELPSTR

UC		"@"	# unknown command
SI		"^"	# self-insert
BBUFUNMOD	"lad som om bufferen ikke er ^/aendret"
CCHARB		"g^/aa et tegn tilbage"
CCHARBD		"slet det foreg^/aaende tegn"
CCHARF		"g^/aa et tegn frem"
CCHARFD		"slet det efterf^/oelgende tegn"
CCHARTRAN	"ombyt de foreg^/aaende to tegn"
CLINEA		"g^/aa til starten af linien"
CLINEAED	"slet denne linie"
CLINEB		"g^/aa en linie tilbage"
CLINEE		"g^/aa til slutningen af linien"
CLINEF		"g^/aa en linie frem"
CLINEFD		"slet resten af linien"
CSCRNB		"g^/aa en sk^/aermside tilbage"
CSCRNBOW	"g^/aa en sk^/aermside tilbage i det andet vindue"
CSCRNF		"g^/aa en sk^/aermside frem"
CSCRNFOW	"g^/aa en sk^/aermside frem i det andet vindue"
DCAL		"vis kalenderen for denne m^/aaned"
DLEFT		"rul vinduet mod venstre"
DNEWDISPLAY	"genopfrisk og centrer sk^/aermbilledet"
DNEXT		"vis kalenderen for den efterf^/oelgende m^/aaned"
DPREV		"vis kalenderen for den foreg^/aaende m^/aaned"
DRIGHT		"rul vinduet mod h^/oejre"
DSCREENRANGE	"centrer sk^/aermbilledet"
DTOGVISGRAY	"vis ikke / vis ordmellemrum"
DWINDGROW	"g^/oer vinduet st^/oerre"
DWINDONE	"vis kun eet vindue"
DWINDSWAP	"g^/aa / skift til det andet vindue"
DWINDTOG	"skift mellem et og to vinduer"
DWINDTWO	"vis to vinduer"
DWINDTWOO	"vis to vinduer og g^/aa til det andet vindue"
FBUFBEG		"g^/aa til starten af bufferen"
FBUFDEL		"slet denne buffer"
FBUFEND		"g^/aa til slutningen af bufferen"
FBUFLIST	"vis buffer-liste"
FBUFNEXT	"g^/aa til n^/aeste buffer i ringen"
FBUFPREV	"g^/aa til foreg^/aaende buffer i ringen"
FBUFPRINT	"udskriv bufferen"
FCOMMAND	"udf^/oer system-kommando"
FDIRED		"DIRED: vis katalog"
FFILEDELETE     "slet en fil"
FFILEFIND	"find en fil og opret en ny buffer"
FFILEINS	"indl^/aes og inds^/aet en fil her"
FFILEPATH	"find en fil v.hj.a. PATH"
FFILEREAD	"indl^/aes en fil til bufferen"
FFILESAVE	"gem bufferen i en fil med samme navn"
FFILEWRITE	"sp^/oerg efter et filnavn og gem bufferen der"
FPRINTSEL	"v^/aelg en printer"
FSAVEALL        "gem alle ^/aendrede buffere"
FTODIRED        "g^/aa til DIRED-bufferen"
JCOPY		"kopier omr^/aadet til klippebordet"
JCUT		"flyt omr^/aadet til klippebordet: klip"
JPASTE		"hent fra klippebordet: klistre"
KBEGMAC		"begynd indspilning af en makro"
KCOLON		"udf^/oer ^:-funktionen"
KENDMAC		"afslut indspilning af en makro"
KFROMMAC	"kald en makro"
KLOADMAC	"hent eller gem en makro"
MABORT		"afbryd, fortryd"
MABOUT		"vis version og andre oplysninger om denne Freyja"
MAPROPOS	"sp^/oerg efter en tekst og vis tilsvarende kommandoer"
MATTR		"vis testbillede for sk^/aermattributter"
MCHARAT		"vis talv^/aerdien af tegnet her"
MCTRLX		"^X kommando-pr^/aefix"
MDATE		"inds^/aet dags dato"
MDESCBIND	"vis listen over alle kommandoer"
MDESCKEY	"sp^/oerg efter en tast og vis dens funktion"
MDOKEY		"udf^/oer den funktionstast, argumentet angiver"
MEXIT		"afslut programmet"
MFONT		"v^/aelg n^/aeste skrifttype (eller s^/aet den hvis der er et argument)"
MINS8		"inds^/aet argumentet som et tegn"
MINSPGBK	"inds^/aet et sideskift"
MMACEVAL        "udf^/oer den angivne makro, som er defineret i .res-filen"
MMAKEDELETE	"tilf^/oej til slettebufferen ved n^/aeste sletning"
MMENU		"vis den menu som argumentet angiver"
MMENUH		"hj^/aelp"
MMENUM		"vis hovedmenuen"
MMETA		"Esc / meta kommando-pr^/aefix"
MQUOTE		"inds^/aet den n^/aeste kommando i stedet for at udf^/oere den"
MREPLACE	"s^/oeg og erstat"
MREPLACEQ	"s^/oeg og erstat med bekr^/aeftelse"
MSEARCHB	"s^/oeg bagl^/aens"
MSEARCHF	"s^/oeg forl^/aens"
MTIME		"inds^/aet klokkesl^/aettet"
MTOGPGLN        "ombyt betydningen af PgUp/PgDn og pil op/ned"
MUARG		"universelt argument"
RDELWHITE	"slet blankt i slutningen af linierne i omr^/aadet"
RHARDTOSOFT	"lav h^/aarde linieskift om til bl^/oede"
RINDENT		"ryk omr^/aadet ind"
RLOWER		"skriv ordene i omr^/aadet med sm^/aa bogstaver"
RMARKSET	"s^/aet m^/aerket her"
RMARKSWAP	"s^/aet m^/aerket her og g^/aa til det gamle m^/aerke"
RMETA		"stryg meta-bitten (2^7) i omr^/aadet; hvis arg, s^/aet den"
ROUTDENT	"ryk omr^/aadet ud"
RPRINT		"udskriv omr^/aadet"
RREGCOPY	"kopier omr^/aadet til slettebufferen"
RREGDELETE	"flyt omr^/aadet til slettebufferen: klip"
RREGFILL	"ombryd omr^/aadet; hvis arg, ombryd hele bufferen"
RSAVE		"gem omr^/aadet i en fil"
RSOFTTOHARD	"lav bl^/oede linieskift om til h^/aarde"
RTABIFY		"brug tabuleringer i omr^/aadet"
RUNTABIFY	"brug ikke tabuleringer i omr^/aadet"
RUPPER		"skriv ordene i omr^/aadet med STORE bogstaver"
RYANK		"hent fra slettebufferen: klistre"
UCALC		"RPN-regnemaskine"
UENTER		"inds^/aet tallet fra teksten i regnemaskinen"
UPRINTX		"inds^/aet X-registeret; hvis arg, udf^/oer Esc ^A ^W f^/oerst"
WCASEROTATE	"skift til n^/aeste skrivem^/aade: sm^/aa->Stort Forbogstav->STORE"
WINDDEL		"slet liniens indrykning"
WINDNEXT	"inds^/aet linieskift og ryk ind"
WINDTHIS	"inds^/aet en tom linie ovenover med samme indrykning"
WINSNL		"inds^/aet linieskift"
WINSNLA		"flyt resten af linien ned p^/aa en ny linie"
WJOINGRAY	"reducer ordmellemrummet til et enkelt blanktegn"
WLINECENTER	"centrer linien"
WNUMB		"g^/aa et tal tilbage"
WNUMF		"g^/aa et tal frem"
WNUMMARK	"afm^/aerk dette tal"
WPARAB		"g^/aa et afsnit tilbage"
WPARAF		"g^/aa et afsnit frem"
WPARAFILL	"ombryd afsnit"
WPARAMARK	"afm^/aerk dette afsnit"
WPRINTLINE	"vis linieantal"
WPRINTMAR	"vis margener"
WPRINTPOS	"vis position"
WSENTB		"g^/aa en s^/aetning tilbage"
WSENTBD		"slet den foreg^/aaende s^/aetning"
WSENTF		"g^/aa en s^/aetning frem"
WSENTFD		"slet den efterf^/oelgende s^/aetning"
WSETFILL	"s^/aet denne buffers ombrydningsm^/aade"
WSETLEFT	"s^/aet denne buffers venstremargen til argumentet"
WSETRIGHT	"s^/aet denne buffers h^/oejremargen til argumentet"
WSETTABS	"s^/aet denne buffers tabulatorafstand til argumentet"
WWORDB		"g^/aa et ord tilbage"
WWORDBD		"slet det foreg^/aaende ord"
WWORDCAP	"skriv ordet med stort forbogstav"
WWORDF		"g^/aa et ord frem"
WWORDFD		"slet det efterf^/oelgende ord"
WWORDLOW	"skriv ordet med sm^/aa bogstaver"
WWORDTRAN	"ombyt ord"
WWORDUP		"skriv ordet med STORE bogstaver"


# ------------------------------------------------------------

# Menus Tables

menu	BUFFER
- " Buffer "		- _cdiu 0x12
			     _*  0x10
- "Liste"		- key "^X^B"
- "N^/aeste"		- key "^XB"
- "Foreg^/aaende"	- key "^XV"
- "Slet"		- key "^XK"
- "TegnPosition"	- key "^X="
- "L&iniePosition"	- key "^[="
- "G^/aaTil&DIRED"      - key "^XD"

menu	CALC
- " Regnemaskine "	- _cdiu 0x12
			     _*  0x10
- "Regn"		- key "^[^U"
- "Afm^/aerkTal"	- key "^[^A"
- "Inds^/aetTal"	- key "^[^E"

menu	CLIP
- " Klippebord "	- _cdiu 0x12
			     _*  0x10
- "Kopier(TilKlip)"	- key "^95"
- "Saks/Klip(TilKlip)"	- key "^96"
- "Lim/Klistre(Klippet)" - key "^97"

menu	CALENDAR
- " Kalender "		- _cdiu 0x12
			     _*  0x10
- "Vis"			- key "^[^C"
- "N^/aesteM^/aaned"	- key "^[^N"
- "Foreg^/aaendeM^/aaned" - key "^[^P"

menu	CASE
- " Tegn "		- _cdiu 0x12
			     _*  0x10
- "Forbogstav"		- key "^[C"
- "lilleOrd"		- key "^[L"
- "l&illeOmr^/aade"	- key "^X^L"
- "STORT-ord"		- key "^[U"
- "S&TORT-omr^/aade"	- key "^X^U"

menu	EDIT
- " Rediger "		- _cdiu 0x12
			     _*  0x10
- "AnbringM^/aerketHer"	- key "^@"
- "Marker&Alt"		- key "^U^[H"
- "Kopier|^[W"		- key "^[W"
- "Saks/Klip|^W"	- key "^W"
- "Lim/Klistre|^Y"	- key "^Y"
- "G^/aaTilM^/aerket"	- key "^X^X"

menu	FILE
- " Filer "		- _cdiu 0x12
			     _*  0x10
- "Om&Freyja"		- key "^8"
- "^/Aaben...|^X^F"	- key "^X^F"
- "Ny"			- key "^X^Fudennavn^M"
- "Gem|^X^S"		- key "^X^S"
- "Gem&Som..."		- key "^X^W"
- "Gem&Omr^/aade..."	- key "^7"
- "GemAlle&Buffere"     - key "^101"
- "L^/aesIgen..."	- key "^X^R"
- "Inds^/aet..."	- key "^X^I"
- "Katalog..."		- key "^U^X^D"
- "G^/aaTil&DIRED"      - key "^XD"
- "Sl&etFil"            - key "^X^K"
- "LadSom&U^/aendret"	- key "^[~"
- "Afslut"		- key "^X^C"

menu	FILL1
- " Ombrydning "	- _cdiu 0x01
			     _*  0x00
- "Ingen"		- key "^U0^X/"
- "H^/aard"		- key "^U1^X/"
- "Oml^/oebende"	- key "^U2^X/"

table	FILL2		# this allows us to invoke the fill menu
- key "^U^!FILL1`^0"

menu	HELP
- " Hj^/aelp "		- _cdiu 0x01
			     _*  0x00
- "Apropos..."		- key "^4"
- "Beskriv..."		- key "^6"
- "Taster"		- key "^5"
- "Dok"			- key "^2freyja.doc^M"
- "FSF"			- key "^2fsf.doc^M"
- "Garanti"		- key "^2warranty.doc^M"
- "Kopiering"		- key "^2copying.doc^M"
- "Liga"		- key "^2league.doc^M"
- "L&iste"		- key "^2fcmdlist.doc^M"
- "Menuer"		- key "^2fmenus.doc^M"
- "Regnemaskine"	- key "^2fcalc.doc^M"
- "Undervis"		- key "^2ftutoria.doc^M"
- "XKrydsref"		- key "^2fcmdxref.doc^M"

menu	KEYMACRO
- " Makro "		- _cdiu 0x12
			     _*  0x10
- "Indspil|^X("		- key "^X("
- "AfslutIndspilning|^X)" - key "^U^X)"
- "Udf^/oer|^]"		- key "^]"
- "Hent..."		- key "^:Indtast makro-nummer`^[^]"
- "MakroTilBuffer"	- key "^[^]"
- "BufferTilMakro"	- key "^U^[^]"

menu	LAYOUT
- " Opstilling "	- _cdiu 0x12
			     _*  0x10
- "CentrerLinie|^[S"	- key "^[S"
- "Ombryd&Afsnit|^[Q"	- key "^[Q"
- "OmbrydOmr^/aadet"	- key "^[G"
- "Margener?"		- key "^X#"
- "VenstreMargen..."	- key "^:Indtast venstremargen`^X."
- "H^/oejreMargen..."	- key "^:Indtast h^/oejremargen`^XF"
- "Tabulator..."	- key "^:Indtast tabulatorafstand`^X,"
- "Ombr&ydningsm^/aade..." - key "^X/"
- "H^/aa&rdeLinieSkift" - key "^X}"
- "Bl^/oedeLinieSkift"  - key "^X{"

menu	MOVE
- " G^/aa "		- _cdiu 0x12
			     _*  0x10
- "Begyndelsen"		- key "^[<"
- "Slutningen"		- key "^[>"
- "Linie..."		- key "^:G^/aa til linie`^[<"

menu	OTHER
- " Diverse "		- _cdiu 0x12
			     _*  0x10
- "Dato"		- key "^209"
- "Klokken"		- key "^210"
- "OrdmellemrumSes"	- key "^X`"
- "MSDOS-kommando..."	- key "^X^E"
- "TastInd..."		- key "^Q"
- "V^/aerdiInd..."	- key "^:Indtast ASCII-v^/aerdi (decimalt)`^X8"
- "Funktionstast..."	- key "^:Indtast scan-code (decimalt)`^X9"
- "S^/aetMeta"		- key "^X7"
- "S&trygMeta"		- key "^U^X7"
- "GemAlt+Afslut"	- key "^U^X^C"
- "Om&bytPgUp/DnOgPile" - key "^X-"

menu	PRINT
- " Udskrift "		- _cdiu 0x12
			     _*  0x10
- "Bufferen"		- key "^XP"
- "Omr^/aadet"		- key "^X^P"
- "V^/aelg..."		- key "^9"
- "Ubehandlet"		- key "^U^XP"

menu	PRSEL1
- " Printer "		- _cdiu 0x01
			     _*  0x00
- "Generel"		- key "^U^!PRINTER1`^9"
- "UdenOverskrift"	- key "^U^!PRINTER2`^9"
- "Brev"		- key "^U^!PRINTER3`^9"
- "Fil"			- key "^U^!PRINTER4`^9"

table	PRSEL2		# this allows us to invoke the printer select menu
- key "^U^!PRSEL1`^0"

menu	SEARCH
- " S^/oeg "		- _cdiu 0x12
			     _*  0x10
- "Forl^/aens...|^S"	- key "^S"
- "Bagl^/aens...|^R"	- key "^R"
- "S^/oeg+Erstat..."	- key "^[R"
- "S^/oeg+&Erstat+Bekr^/aeft..." - key "^[^R"

menu	WHITESPACE
- " Luft "		- _cdiu 0x12
			     _*  0x10
- "RykLinieUd"		- key "^\\"
- "OrdmellemrumKort"	- key "^[\\"
- "TabIOmr^/aadet"	- key "^XT"
- "EjTabIOmr^/aadet"	- key "^XU"
- "IndrykOmr^/aadet"	- key "^X]"
- "UdrykOmr^/aadet"	- key "^X["
- "Slankomr^/aadet"	- key "^X^\\"

menu	FONT
- " Font "		- _cdiu 0x12
			     _*  0x10
- "Normal"		- key "^U103^X+"
- "Skift|^X+"	- key "^X+"
- _js "21x&60"
  _cd "18x&64"
  _i "25x&40"
  _* "ubrugt"		- _js key "^U25^X+J"
			     _cd key "^U80^X+J"
			     _i key "^U0^X+J"
			     _* key ""
- _js "25x&80"
  _cd "16x&40"
  _* "ubrugt"		- _js key "^U26^X+J"
			     _cd key "^U84^X+J"
			     _* key ""

menu	WINDOW
- " Vindue "		- _cdiu 0x12
			     _*  0x10
- "Et"			- key "^X1"
- "To"			- key "^X2"
- "DetAndet"		- key "^XO"
- "Venstre|^X<"		- key "^X<"
- "H^/oejre|^X>"	- key "^X>"
- "Font..."		- list16 256 FONT .
- "NedAndet"		- key "^X^V"
- "OpAndet"		- key "^X^Z"
- "St^/oerre..."	- key "^:Hvor mange linier st^/oerre`^X^="
- "Centrer"		- key "^L"
- "Gendan"		- key "^[^L"

menu	MAIN
- " Menu "		- _cdiu 0x01
			     _*  0x00
- "Filer"		- list16 256 FILE .
- "Rediger"		- list16 256 EDIT .
- "Buffer"		- list16 256 BUFFER .
- "G^/aa"		- list16 256 MOVE .
- "S^/oeg"		- list16 256 SEARCH .
- "Vindue"		- list16 256 WINDOW .
- "Makro"		- list16 256 KEYMACRO .
- "Tegn"		- list16 256 CASE .
- "Opstilling"		- list16 256 LAYOUT .
- "Luft"		- list16 256 WHITESPACE .
- "Udskrift"		- list16 256 PRINT .
- _sc "Klippebord"
  _*  "Reg&nemaskine"	- list16 256 ?sc CLIP !sc CALC .
- "Kal&ender"		- list16 256 CALENDAR .
- "Diverse"		- list16 256 OTHER .
- "Hj^/aelp"		- list16 256 1 .
- "Afslut"		- key "^X^C"
