#			 Resource Source File
#		     Freyja English Language Text
#

# ------------------------------------------------------------
# Messages Table
#
# This table holds all language-dependant messages.

table	MSGS

# system buffer names

SYSBUFFLIST	"%bufflist%"
SYSCAL		"%cal%"
SYSDIRED	"%dired%"
SYSHELP		"%help%"
SYSINCLUDE	"%include%"
SYSKILL		"%kill%"
SYSSCRATCH	"%scratch%"

# freyja.c
CANTINIT	"Can't init buf"
SCRATCH		"Freyja 3.0, an Emacs-type Text Editor\n\
\n\
Type ^X ^C to exit\n\
     ^^ for menus\n\
     ^X H for help\n\
-------------------------------------\n\
Copyright 1991,2,3,4,6 by Craig A. Finseth\n\
1343 Lafond, St Paul MN 55104, USA\n\
fin@finseth.com\n\
+1 500 448 9440\n\
+1 612 644 4027\n"

FKEYHELP	_u
"F1	help\n\
F2	delete the current line\n\
F3	delete surrounding whitespace and insert space\n\
F4	open a line and indent it the same as current\n"
		_cs
"F1	help\n\
F2	delete the current line\n\
F3	delete surrounding whitespace and insert space\n\
F4	open a line and indent it the same as current\n\
F5	move to the next buffer		Shift-F5	delete a buffer\n\
F6	switch to the other window	Shift-F6	toggle 1/2 windows\n\
F7	forward search\n\
F8	save the buffer			Shift-F8	find a file\n\
					Shift-F9	calendar\n\
F10	bring up the main menus		Shift-F10	exit / quit\n\
		Ctrl-F2		copy region to clipboard\n\
		Ctrl-F3		delete region to clipboard: cut\n\
		Ctrl-F4		yank clipboard: paste\n\
		Ctrl-F5		reverse / backward search\n\
		Ctrl-F8		save all buffers\n\
		Ctrl-F9		set the mark to the point\n\
		Ctrl-F10	insert a page break\n"
		_*
"F1	help\n\
F2	delete the current line\n\
F3	delete surrounding whitespace and insert space\n\
F4	open a line and indent it the same as current\n\
F5	move to the next buffer		Shift-F5	delete a buffer\n\
F6	switch to the other window	Shift-F6	toggle 1/2 windows\n\
F7	forward search\n\
F8	save the buffer			Shift-F8	find a file\n\
F9	RPN calculator			Shift-F9	calendar\n\
F10	bring up the main menus		Shift-F10	exit / quit\n\
		Ctrl-F2		copy region to kill buffer\n\
		Ctrl-F3		delete region to kill buffer: cut\n\
		Ctrl-F4		yank kill buffer: paste\n\
		Ctrl-F5		reverse / backward search\n\
		Ctrl-F8		save all buffers\n\
		Ctrl-F9		set the mark to the point\n\
		Ctrl-F10	insert a page break\n"

FKEYHELP40	_u
"F1	help\n\
F2	delete the current line\n\
F3	delete surrounding whitespace and insert space\n\
F4	open a line and indent it the same as current\n"
		_sc
"\n\
\n\
C:Help  Cut     RFind   Find    Mark\n\
C:  Copy    Paste           SvAll  PgBrk\n\
\n\
S:              DelBuf          Calen\n\
S:                  1/2Win  Open    Quit\n\
\n\
 Help   DelWh   NxBuf   Find\n\
    DelLn   OpnLn   SwWin   Save    Menu\n"
		_*
"\n\
\n\
C:Help  Cut     RFind   Find    Mark\n\
C:  Copy    Paste           SvAll  PgBrk\n\
\n\
S:              DelBuf          Calen\n\
S:                  1/2Win  Open    Quit\n\
\n\
Help   DelWh    NxBuf   Find    Calc\n\
    DelLn   OpnLn   SwWin   Save    Menu\n"

# fbuf.c
ASKOVER		"Overwrite changed/existing file?"
BADMARK		"Bad mark"
ERRWRITE	"Error writing file"
INTPAGEERR	"Internal paging error"
NODELKILL	"Can't delete the kill buffer"
NOFREE		"No free buffers"
NOMARKS		"Out of marks"
NOSWAP		"Out of swap space"
NOTBUFF		"Not a buffer"
NOTCOPYSAME	"Must copy to a different buffer"
NOTMARK		"Not a mark"
NULLPAGE	"Null page"

# fcalc.c
CALCDIVZERO	_sc ""
		_* "Divide by Zero"
CALCFLAG	_sc ""
		_* "illegal flag"
CALCINVBIN	_sc ""
		_* "Invalid binary number"
CALCMODZERO	_sc ""
		_* "Mod of Zero"
CALCNEG		_sc ""
		_* "Negative"
CALCPCTZERO	_sc ""
		_* "Percent of Zero"
CALCRANGE	_sc ""
		_* "Out of Range"
CALCUNKCMD	_sc ""
		_* "Unknown Command"
CALCUNKPROG	_sc ""
		_* "unknown program"
CALCUNKREG	_sc ""
		_* "Unknown Register"
UNKINDREG	_sc ""
		_* "Unknown Indirect Register"

# fdisplay.c
FREYJAVER	_u "Freyja ###%% *+^ >%d <%d %s %s"
		_* "Freyja ###%% *+^ \x10%d \x11%d %s %s"
DISPFILL	"Fill"
DISPNONE	"None"
DISPWRAP	"Wrap"

# ffile.c
ASKTHROW	"Throw away changes? "
ERRDELETE	"Can't delete"
ERRINS		"Can't create insertion buffer"
ERRREAD		"Read Error"
ERROPEN		"Can't find file"
ERRSAVE		"Unable to save file"
LISTERR		"Cannot execute list command"
PROMPTCMD	"Command Line"
PROMPTDELETE	"File to Delete"
PROMPTDELMOD	"Delete modified buffer?"
PROMPTDIRNAME	"File/directory name"
PROMPTFIND	"File to Find"
PROMPTINS	"File to Insert"
PROMPTREAD	"File to Read"
PROMPTWRITE	"File to Write"
RETURN		"[Hit <CR> to return to Freyja]"
WRITING		"Writing File..."

# fkey.c
ERRNONNUM	"Not a decimal number"
MACROCREATE	"Creating macro"
MACROFULL	"Keyboard macro full"
MACROUSE	"Using macro"
NOSTRING	"\x8\x7fN"
PROMPTARG	"Arg: %d"
YESSTRING	" Y"

# fmisc.c
ABOUTSTR	"Freyja text editor, Version 3.0\n\
\n\
running: for %c, key %c, screen %d %c (%d rows, %d cols)\n\
using %d of %d swap area pages, each %d KBytes\n\
current directory %s\n\
\n\
Copyright 1991,2,3,4,6 by Craig A. Finseth\n\
1343 Lafond, St Paul MN 55104, USA\n\
fin@finseth.com\n\
+1 500 448 9440\n\
+1 612 644 4027\n\n"
ASKCONFIRM	"Confirm Replace?"
ASKSAVEEXIT	"Save before exiting? "
ASKSURE		"Are you sure? "
CHARAT		"  %d decimal, %x hex, %o octal"
DISPLCMDS	"Commands are: Y)yes N)no ,)try !)all .)exit ;)do&exit"
DISPLREPL	"Replacing '%s' with '%s'"
EXPLARG		"An explicit argument must be supplied."
NOTCMD		"not a command"
PROMPTAPROPOS	"Apropos"
PROMPTCTX	"Control-X:"
PROMPTESC	"Meta:"
PROMPTFWDSRCH	"Forward Search"
PROMPTKEY	"Key to describe:"
PROMPTQUERY	"Query Replace String"
PROMPTQUOTE	"Quote:"
PROMPTREPL	"Replace String"
PROMPTREVSRCH	"Reverse Search"
PROMPTWITH	"with"
SELFINSERT	"self-insert"
STRNOTFOUND	"String not found"
UNKCMD		"Unknown command"
WORKING		"Working..."

# fregion.c
PRINTDOC	"%s, %4d-%02d-%02d %2d:%02d:%02d, page %d"
PRINTLET	"- %d -"
PROMPTSREG	"File to Save Region to"

# ftable.c
ABORTHELP	"abort, cancel"
ABORTKEY	"Abort"
QUITHELP	"exit / quit the editor"
QUITKEY		"Quit"

# fwhite.c
DISPLCOLS	"left %d   right %d   tab spacing %d"
DISPLLINE	"line %u of %u lines, mark on line %u "
DISPLPOINT	"at char %l of %l, column %d, mark %l "
DISPLRULER	"Ruler inserted"
ERRFILL		"Unknown fill mode"

# smjaguar.c
CLIPINIT	_sc "can't init clipboard"
		_* ""
CLIPOPEN	_sc "can't open clipboard"
		_* ""
GETTER		_sc "can't init file getter"
		_* ""
PASTE		_sc "nothing to paste"
		_* ""


# ------------------------------------------------------------
# Months Table
#
# This table holds the names of the months.

table	MONTHS
-	"January"
-	"February"
-	"March"
-	"April"
-	"May"
-	"June"
-	"July"
-	"August"
-	"September"
-	"October"
-	"November"
-	"December"


# ------------------------------------------------------------
# Days Table
#
# This table holds the names of the days of the week, first is
# Sunday.  These entries should not be more than four characters long.
# They will ordinarily be three characters and a trailing space.

table	DAYS
-	"Sun "
-	"Mon "
-	"Tue "
-	"Wed "
-	"Thu "
-	"Fri "
-	"Sat "


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
- "Shift-Tab"	- "Alt-Q"	- "Alt-W"	- "Alt-E"	- "Alt-R"
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
- ""		- "Home"	- "Up Arrow"	- "PgUp"	- ""
- "Left Arrow"	- ""		- "Right Arrow"	- ""		- "End"
	# 80
- "Down Arrow"	- "PgDn"	- "Ins"		- "Del"		- "Shift-F1"
- "Shift-F2"	- "Shift-F3"	- "Shift-F4"	- "Shift-F5"	- "Shift-F6"
	# 90
- "Shift-F7"	- "Shift-F8"	- "Shift-F9"	- "Shift-F10"	- "Ctrl-F1"
- "Ctrl-F2"	- "Ctrl-F3"	- "Ctrl-F4"	- "Ctrl-F5"	- "Ctrl-F6"
	# 100
- "Ctrl-F7"	- "Ctrl-F8"	- "Ctrl-F9"	- "Ctrl-F10"	- "Alt-F1"
- "Alt-F2"	- "Alt-F2"	- "Alt-F4"	- "Alt-F5"	- "Alt-F6"
	# 110
- "Alt-F7"	- "Alt-F8"	- "Alt-F9"	- "Alt-F10"	- "Ctrl-PrtSc"
- "Ctrl-Left Arrow"	- "Ctrl-Right Arrow"	- "Ctrl-End"	- "Ctrl-PgDn"
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
- "MENU"	- "Shift-MENU"	- "Ctrl-MENU"	- "Alt-MENU"	- ""
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
BBUFUNMOD	"clear the buffer modified flag"
CCHARB		"move backward character"
CCHARBD		"delete the preceeding character"
CCHARF		"move forward character"
CCHARFD		"delete the following character"
CCHARTRAN	"interchange preceeding characters"
CLINEA		"move to the beginning of the line"
CLINEAED	"delete the current line"
CLINEB		"move to the previous line"
CLINEE		"move to the end of the line"
CLINEF		"move to the next line"
CLINEFD		"kill / delete the rest of the line"
CSCRNB		"move to the previous screen"
CSCRNBOW	"move to previous screen in other window"
CSCRNF		"move to the next screen"
CSCRNFOW	"move to next screen in other window"
DCAL		"display a calendar for the current month"
DLEFT		"scroll the window left"
DNEWDISPLAY	"refresh and recenter the screen"
DNEXT		"advance the calendar a month"
DPREV		"rewind the calendar a month"
DRIGHT		"scroll the window right"
DSCREENRANGE	"recenter the screen"
DTOGVISGRAY	"toggle whether grayspace is visible"
DWINDGROW	"grow the current window"
DWINDONE	"set one window"
DWINDSWAP	"move / switch to the other window"
DWINDTOG	"toggle between one and two windows"
DWINDTWO	"set two windows"
DWINDTWOO	"set two windows, leave the point in other window"
FBUFBEG		"move to the beginning of the buffer, with arg, go to that line"
FBUFDEL		"delete a buffer"
FBUFEND		"move to the end of the buffer"
FBUFLIST	"list the buffers"
FBUFNEXT	"move to the next buffer in the ring"
FBUFPREV	"move to the previous buffer in the ring"
FBUFPRINT	"print the buffer"
FCOMMAND	"execute system command"
FDIRED		"DIRED: directory display"
FFILEDELETE	"delete a file"
FFILEFIND	"find a file, creating new buffer"
FFILEINS	"insert a file at the point"
FFILEPATH	"find a file using the path"
FFILEREAD	"read a file into the buffer"
FFILESAVE	"save the buffer using current name"
FFILEWRITE	"ask for filename and write the buffer"
FPRINTSEL	"select a printer"
FSAVEALL	"save all modified buffers"
FTODIRED	"switch to the DIRED buffer"
JCOPY		"copy region to clipboard"
JCUT		"delete region to clipboard: cut"
JPASTE		"yank clipboard: paste"
KBEGMAC		"start collecting keyboard macro"
KCOLON		"do the ^: function"
KENDMAC		"end collecting keyboard macro"
KFROMMAC	"invoke keyboard macro"
KLOADMAC	"load or save the keyboard macro"
MABORT		"abort, cancel"
MABOUT		"show version and other information about this Freyja"
MAPROPOS	"ask for a string and display matching commands"
MATTR		"put up a character attribute test screen"
MCHARAT		"show the value of the character at the point"
MCTRLX		"^X command prefix"
MDATE		"insert the current date"
MDESCBIND	"build command list"
MDESCKEY	"ask for a key and display its meaning"
MDOKEY		"do the function key specified by the argument"
MEXIT		"exit / quit the editor"
MFONT		"cycle the display font (or set it if an argument is supplied)"
MINS8		"insert the argument as a character"
MINSPGBK	"insert a page break"
MMACEVAL	"evaluate the implied keyboard macro directly from the .res file"
MMAKEDELETE	"append the next kill to the kill buffer"
MMENU		"execute the menu specified by the argument"
MMENUH		"help"
MMENUM		"bring up the main menus"
MMETA		"Esc / meta command prefix"
MQUOTE		"quote the next command"
MREPLACE	"global search and replace string"
MREPLACEQ	"query global search and replace string"
MSEARCHB	"reverse / backward search"
MSEARCHF	"forward search"
MTIME		"insert the current time"
MTOGPGLN	"toggle the PgUp/Dn and Up/Down Arrow key assignments"
MUARG		"universal argument"
RDELWHITE	"delete trailing whitespace on lines in the region"
RHARDTOSOFT	"convert hard newlines to soft"
RINDENT		"indent the region"
RLOWER		"lowercase the region"
RMARKSET	"set the mark to the point"
RMARKSWAP	"interchange the point and mark"
RMETA		"clear the meta (2^7) bit in region; with arg, set it"
ROUTDENT	"outdent the region"
RPRINT		"print the region"
RREGCOPY	"copy region to kill buffer"
RREGDELETE	"delete region to kill buffer: cut"
RREGFILL	"fill the region, if arg, fill the buffer"
RSAVE		"save the region to a file"
RSOFTTOHARD	"convert soft newlines to hard"
RTABIFY		"tabify the region"
RUNTABIFY	"untabify the region"
RUPPER		"UPPERCASE the region"
RYANK		"yank kill buffer: paste"
UCALC		"RPN calculator"
UENTER		"enter the current number into the calculator"
UPRINTX		"insert the X register; if arg, does Esc ^A ^W first"
WCASEROTATE	"rotate case of word lower->Capital->UPPER"
WINDDEL		"delete indentation on line"
WINDNEXT	"insert Newline and indent"
WINDTHIS	"open a line and indent it the same as current"
WINSNL		"insert Newline"
WINSNLA		"open line"
WJOINGRAY	"delete surrounding whitespace and insert space"
WLINECENTER	"center line"
WNUMB		"move backward number"
WNUMF		"move forward number"
WNUMMARK	"mark the current number"
WPARAB		"move backward paragraph"
WPARAF		"move forward paragraph"
WPARAFILL	"fill pargraph"
WPARAMARK	"mark the current paragraph"
WPRINTLINE	"display line counts"
WPRINTMAR	"display the margin settings"
WPRINTPOS	"display position"
WSENTB		"move backward sentence"
WSENTBD		"delete the preceding sentence"
WSENTF		"move forward sentence"
WSENTFD		"delete the following sentence"
WSETFILL	"set this buffer's fill mode"
WSETLEFT	"set this buffer's left margin to the argument"
WSETRIGHT	"set this buffer's right margin to the argument"
WSETTABS	"set this buffer's tab spacing to the argument"
WWORDB		"move backward word"
WWORDBD		"delete the preceeding word"
WWORDCAP	"Capitalize word"
WWORDF		"move forward word"
WWORDFD		"delete the following word"
WWORDLOW	"lowercase word"
WWORDTRAN	"interchange words"
WWORDUP		"UPPERCASE word"


# ------------------------------------------------------------

# Menus Tables

menu	BUFFER
- " Buffer "		- _cdiu 0x12
			  _*  0x10
- "List"		- key "^X^B"
- "Next"		- key "^XB"
- "Previous"		- key "^XV"
- "Delete"		- key "^XK"
- "CharsCount"		- key "^X="
- "L&inesCount"		- key "^[="
- "ToDIRED"		- key "^XD"

menu	CALC
- " Calculator "	- _cdiu 0x12
			  _*  0x10
- "Invoke"		- key "^[^U"
- "MarkNumber"		- key "^[^A"
- "EnterNumber"		- key "^[^E"

menu	CLIP
- " Clipboard "		- _cdiu 0x12
			  _*  0x10
- "CopyToClip"		- key "^95"
- "Cu&tToClip"		- key "^96"
- "PasteClip"		- key "^97"

menu	CALENDAR
- " Calendar "		- _cdiu 0x12
			  _*  0x10
- "Display"		- key "^[^C"
- "NextMonth"		- key "^[^N"
- "PrevMonth"		- key "^[^P"

menu	CASE
- " Case "		- _cdiu 0x12
			  _*  0x10
- "CapitalizeWord"	- key "^[C"
- "lowerWord"		- key "^[L"
- "l&owerRegion"	- key "^X^L"
- "UPPERWord"		- key "^[U"
- "U&PPERRegion"	- key "^X^U"

menu	EDIT
- " Edit "		- _cdiu 0x12
			  _*  0x10
- "Mark"		- key "^@"
- "Mark&All"		- key "^U^[H"
- "Copy|^[W"		- key "^[W"
- "Cu&t|^W"		- key "^W"
- "Paste|^Y"		- key "^Y"
- "OtherEnd"		- key "^X^X"

menu	FILE
- " File "		- _cdiu 0x12
			  _*  0x10
- "A&bout"		- key "^8"
- "Open...|^X^F"	- key "^X^F"
- "New"			- key "^X^Funtitled^M"
- "Save|^X^S"		- key "^X^S"
- "Save&As..."		- key "^X^W"
- "SaveRe&gion..."	- key "^7"
- "Sa&veAll"		- key "^101"
- "ReRead..."		- key "^X^R"
- "Insert..."		- key "^X^I"
- "Directory..."	- key "^U^X^D"
- "ToDIRED"		- key "^XD"
- "De&lete..."		- key "^X^K"
- "ClearModified"	- key "^[~"
- "E&xit"		- key "^X^C"

menu	FILL1
- " Fill "		- _cdiu 0x01
			  _*  0x00
- "None"		- key "^U0^X/"
- "Fill"		- key "^U1^X/"
- "Wrap"		- key "^U2^X/"

table	FILL2		# this allows us to invoke the fill menu
- key "^U^!FILL1`^0"

menu	HELP
- " Help "		- _cdiu 0x01
			  _*  0x00
- "Apropos..."		- key "^4"
- "Bindings"		- key "^5"
- "Calc"		- key "^2fcalc.doc^M"
- "C&opying"		- key "^2copying.doc^M"
- "Doc"			- key "^2freyja.doc^M"
- "FSF"			- key "^2fsf.doc^M"
- "Key..."		- key "^6"
- "League"		- key "^2league.doc^M"
- "L&ist"		- key "^2fcmdlist.doc^M"
- "Menus"		- key "^2fmenus.doc^M"
- "Tutorial"		- key "^2ftutoria.doc^M"
- "Warranty"		- key "^2warranty.doc^M"
- "XRef"		- key "^2fcmdxref.doc^M"

menu	KEYMACRO
- " KeyMacro "		- _cdiu 0x12
			  _*  0x10
- "Record|^X("		- key "^X("
- "FinishRecording|^X)"	- key "^U^X)"
- "Execute|^]"		- key "^]"
- "Fe&tch..."		- key "^:Enter macro number`^[^]"
- "Insert"		- key "^[^]"
- "BufferTo"		- key "^U^[^]"

menu	LAYOUT
- " Layout "		- _cdiu 0x12
			  _*  0x10
- "CenterLine|^[S"	- key "^[S"
- "FillPara|^[Q"	- key "^[Q"
- "FillRe&gion"		- key "^[G"
- "ShowMargins"		- key "^X#"
- "LeftMargin..."	- key "^:Enter left margin`^X."
- "RightMargin..."	- key "^:Enter right margin`^XF"
- "TabSpacing..."	- key "^:Enter tab spacing`^X,"
- "F&illType..."	- key "^X/"
- "Conv&WrapNLstoFill"	- key "^X}"
- "ConvFillNLstoWra&p"	- key "^X{"

menu	MOVE
- " Move "		- _cdiu 0x12
			  _*  0x10
- "Beginning"		- key "^[<"
- "End"			- key "^[>"
- "Line..."		- key "^:Goto line`^[<"

menu	OTHER
- " Other "		- _cdiu 0x12
			  _*  0x10
- "Date"		- key "^209"
- "Time"		- key "^210"
- "WhitespaceVisible"	- key "^X`"
- "ExecuteDOSCmd..."	- key "^X^E"
- "KeyInsert..."	- key "^Q"
- "ValInsert..."	- key "^:Enter ASCII value in decimal`^X8"
- "Do&FuncKey..."	- key "^:Enter scan code in decimal`^X9"
- "ClearMeta"		- key "^X7"
- "SetMeta"		- key "^U^X7"
- "SaveAll&Quit"	- key "^U^X^C"
- "To&gglePgUp/DnandArrow" - key "^X-"

menu	PRINT
- " Print "		- _cdiu 0x12
			  _*  0x10
- "Buffer"		- key "^XP"
- "Region"		- key "^X^P"
- "Select..."		- key "^9"
- "SendRa&w"		- key "^U^XP"

menu	PRSEL1
- " Printer "		- _cdiu 0x01
			  _*  0x00
- "General"		- key "^U^!PRINTER1`^9"
- "Noheading"		- key "^U^!PRINTER2`^9"
- "Letter"		- key "^U^!PRINTER3`^9"
- "File"		- key "^U^!PRINTER4`^9"

table	PRSEL2		# this allows us to invoke the printer select menu
- key "^U^!PRSEL1`^0"

menu	SEARCH
- " Search "		- _cdiu 0x12
			  _*  0x10
- "Forward...|^S"	- key "^S"
- "Backward...|^R"	- key "^R"
- "GlobalReplace..."	- key "^[R"
- "QueryReplace..."	- key "^[^R"

menu	WHITESPACE
- " Whitespace "	- _cdiu 0x12
			  _*  0x10
- "ZapIndentation"	- key "^\\"
- "RemoveSurrounding"	- key "^[\\"
- "TabifyRegion"	- key "^XT"
- "UntabifyRegion"	- key "^XU"
- "IndentRegion"	- key "^X]"
- "OutdentRegion"	- key "^X["
- "Tri&mTrailingRegion"	- key "^X^\\"

menu	FONT
- " Font "		- _cdiu 0x12
			  _*  0x10
- "Normal"		- key "^U103^X+"
- "Cycle|^X+"		- key "^X+"
- _js "21x&60"
  _cd "18x&64"
  _i "25x&40"
  _* "unused"		- _js key "^U25^X+Y"
			  _cd key "^U80^X+Y"
			  _i key "^U0^X+Y"
  			  _* key ""
- _js "25x&80"
  _cd "16x&40"
  _* "unused"		- _js key "^U26^X+Y"
			  _cd key "^U84^X+Y"
  			  _* key ""

menu	WINDOW
- " Window "		- _cdiu 0x12
			  _*  0x10
- "One"			- key "^X1"
- "Two"			- key "^X2"
- "Switch"		- key "^XO"
- "Left|^X<"		- key "^X<"
- "Right|^X>"		- key "^X>"
- "Font..."		- list16 256 FONT .
- "NextOther"		- key "^X^V"
- "PrevOther"		- key "^X^Z"
- "Grow..."		- key "^:Lines to grow by`^X^="
- "Center"		- key "^L"
- "R&efresh"		- key "^[^L"

menu	MAIN
- " Main "		- _cdiu 0x01
			  _*  0x00
- "File"		- list16 256 FILE .
- "Edit"		- list16 256 EDIT .
- "Buffer"		- list16 256 BUFFER .
- "Move"		- list16 256 MOVE .
- "Search"		- list16 256 SEARCH .
- "Window"		- list16 256 WINDOW .
- "KeyMacro"		- list16 256 KEYMACRO .
- "C&ase"		- list16 256 CASE .
- "Layout"		- list16 256 LAYOUT .
- "Wh&itespace"		- list16 256 WHITESPACE .
- "Print"		- list16 256 PRINT .
- _sc "Clipboard"
  _*  "Calculator"	- list16 256 ?sc CLIP !sc CALC .
- "Calenda&r"		- list16 256 CALENDAR .
- "Other"		- list16 256 OTHER .
- "Help"		- list16 256 1 .
- "Quit"		- key "^X^C"

