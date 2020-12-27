#		     Freyja Resource Source File
#
# See the file resource.doc for information on the syntax of this file.
#
# Please be careful when making changes to the resource file.  This
# file, in effect, "opens up" the internals of Freyja and it _is_
# possible to create a file that will cause Freyja to crash.  If in
# doubt, fetch a clean copy of the distribution version and start over
# with your changes.
#
# In general, you can change the _values_ for any entry.  Changing an
# entry's _type_ or inserting or deleting entries (except for adding
# new entries to the end of a table) is what will cause big problems.
#
# This file can be compiled for one of four types of systems:
#
#	i	generic IBMPC and compatible
#	c	HP100LX "cougar:" system manager
#	d	HP100LX "DOS:" MS/DOS, but don't assume sys mgr loaded
#	j	HP95LX "jaguar:" MS/DOS under system manager
#	s	HP95LX "system manager:" system manager
#	u	applies to Unix systems
#
# Not all of these values apply to all systems.  Restrictions are
# marked.

# ------------------------------------------------------------
# Character Sets

include "rpagebeg.res"
include "rpage^$.res"

# ------------------------------------------------------------
# Printer Tables
#
# These tables hold the printer page formats.  The format of each
# table is:
#
#	0	Device to print to (as a file name).  Unix systems
#		just send to a file.
#
#	1	Page width in characters.
#
#	2	Left margin, in characters.
#
#	3	Page length in lines.
#
#	4	Top margin, in lines, counting from top.
#
#	5	Bottom margin, in lines, counting from the bottom.
#
#	6	Initial paper offset in lines from the perforation to the
#		print head (for fan-fold printers: so that you can
#		line up the perforation with the tear bar)
#
#	7	Heading type, one of:
#
#			"N"	none
#			"D"	general document (file name, date/time, and
#				page# at top)
#			"L"	letter (page # on bottom for pages 2 & more)

table		PRINTER1
PRDEV	_u	"%print%"
	_*	"PRN"
PAGEWID		85
PRLEFT		10
PAGELEN		66
PRTOP		6
PRBOT		6
PRIPO		0
PRHEAD		"D"

table		PRINTER2
-	_u	"%print%"
	_*	"PRN"
-		85
-		10
-		66
-		6
-		6
-		0
-		"N"

table		PRINTER3
-	_u	"%print%"
	_*	"PRN"
-		85
-		10
-		66
-		6
-		6
-		0
-		"L"

table		PRINTER4
-		"%print%"
-		85
-		10
-		66
-		6
-		6
-		0
-		"N"

# ------------------------------------------------------------
# Messages and Menus

include "rlang^%.res"

# ------------------------------------------------------------
# Configuration Table
#
# This table holds all basic configuration information.

table	CONF

# ------------------------------------------------------------
# Command Line
# ------------------------------------------------------------
# These values can be overridden by command line options.  They
# let you change enough things to get up and running.

# Swap size: this value specifies the size of the swap area.  It is a
# decimal value in Kilobytes from 16 to 512.  It is rounded to the next
# higher multiple of the page size (1K).  The value in this file can
# be overridden on the command line with the -z option.

SWAPSIZE	_u	512
		_i	256
		_*	40


# Keyboard input: this value specifies where keyboard input is taken
# from.  The value in this file can be overridden on the command line
# with the -k option. Values:
#
#	"B"	MS/DOS ROM BIOS
#	"D"	MS/DOS BDOS (may intercept some control characters)
#	"E"	MS/DOS ROM BIOS extended input
#	"J"	MS/DOS: like "E", but do event-driven input
#	"P"	MS/DOS serial port; uses DOS calls to get the data so
#		the default is COM1; can be changed with MODE; does
#		not support IBM function keys; cannot tell whether a
#		character is available
#	"S"	standard input
#	"T"	termcap
#	"V"	UNIX: standard input, interpret VT100 function keys

KEYINP	_cdjs	"J"
	_u	"S"
	_*	"B"


# Screen type: This value specifies the type of screen output used by
# the editor.  The value in this file can be overridden on the command
# line with the -s option.  Values:
#
#	mode	display	status		size		default method
#
#	0	CGA	text		25 x 40		memory mapped
#	1	CGA	text		25 x 40		memory mapped
#	2	CGA	text		25 x 80		memory mapped
#	3	CGA	text		25 x 80		memory mapped
#	4	CGA	graphics	25 x 40		BIOS call
#	5	CGA	graphics,gray	25 x 40		BIOS call
#	6	CGA	graphics	25 x 80		BIOS call
#	7	MDA	text		25 x 80		memory mapped
#	8	PCJr	unsupported
#	9	PCJr	unsupported
#	10	PCJr,EGA unsupported
#	11	?	unsupported
#	12	?	unsupported
#	13	EGA	graphics	25 x 40		BIOS call
#	14	EGA	graphics	25 x 80		BIOS call
#	15	EGA	graphics	43 x 80		BIOS call
#	16	EGA	unsupported
#	24	95LX	text		16 x 40		memory mapped
#	25	95LX	graphics	21 x 60		custom 1
#	26	95LX	graphics	25 x 80		custom 2
#	27	100LX	text		25 x 80		extended BIOS call
#	28	VT100	text		24 x 80		standard output
#	29	termcap	text		LINES x COLS	API
#	80	100LX	text		64 x 18		extended BIOS call
#	81	100LX	text		64 x 18		extended BIOS call
#	82	100LX	text		40 x 25		extended BIOS call
#	83	100LX	text		40 x 25		extended BIOS call
#	84	100LX	text		40 x 16		extended BIOS call
#	85	100LX	text		40 x 16		extended BIOS call
#
# special values:
#
#	100	used to tell the program to auto-determine the mode
#		If one of the unsupported types is found, mode 2 is used.
#	101	used to mark the end of a list
#	102	used to indicate that no mode has been specified yet
#	103	return to normal font

SCRNTYPE	_u	28
		_js	24	# don't change this to 25/26: see in*95.doc
		_cd	2
		_*	100


# ------------------------------------------------------------
# User Configuration
# ------------------------------------------------------------
# Here is where most configuration will be made.

# INITMACRO: this macro is executed after Freyja starts up and just
# before the program starts reading commands.  Note that if you
# start Freyja without specifying any files on the command line (or
# are using the system manager version), and do not have an inital
# file specified with INITFILE, you must press a key before this
# macro is run.

INITMACRO	key ""


# ESC swap: this value allows you to swap the ESC key with any one
# other key on the keyboard.  The other key must be a single key (i.e.,
# not a ^X prefix function.  The value to use to swap the ESC key with
# the ` key is 96.  On MS/DOS machines, you can specify swapping with
# "function" keys (or anything else that returns a key value in the form
# XX00) by using the value 256 + the scan code.

ESCSWAP		27


# CTX swap: this value allows you to swap the ^X key with any one
# other key on the keyboard.  The other key must be a single key (i.e.,
# not a Esc prefix function.  On MS/DOS machines, you can specify
# swapping with "function" keys (or anything else that returns a key
# value in the form XX00) by using the value 256 + the scan code.

CTXSWAP		24


# Word end: this value allows you to control how the commands that
# move forward words work.  Values:
#	"Y"	The commands stop at the end of the word
#	"N"	The commands stop at the START of the NEXT word

WORDEND		"Y"


# Home/End: this value allows you to control how the Home and End keys
# work.  Values:
#	"Y"	Use alternate settings
#	"N"	Use normal settings
#
# The alternate settings are:
#
#	Home		move to start of line
#	End		move to end of line
#	Ctrl-Home	move to the beginning of the buffer
#	Ctrl-End	move to the end of the buffer

HOMEEND		"N"


# Screen moves: this value allows you to control how the next and previous
# screen commands work.  Values:
#	"P"	moves up/down by "panes"
#	"L"	moves up/down the same number of lines (down, then
#		up brings you back)

SCRNMOVE	"P"


# Twiddle Drag: this value allows you to control how the Twiddle character
# (^T) command works.  Values:
#	"P"	interchange the two preceeding characters
#	"D"	interchange the two characters on each side of the point,
#		leave the point after the second one (i.e., drag the character
#		on the left to the right)

TWIDDRAG	"P"


# Meta handle: this value allows you to specify how "meta" keys are
# handled.  Meta keys send the codes 128 through 255.  Values:
#	"B"	They are bad commands.
#	"I"	They are self-insert (used for entering 8-bit characters)
#	"M"	They are mapped to the meta (Esc prefix) commands

METAKEY	_u	"M"
	_*	"I"


# Ruler lines allowed: this value allows you to specify whether files
# should be checked for word wrap ruler lines when read in.  Values:
#	"Y"	Yes, check them.
#	"N"	No, don't check them.

RULERCHECK	"Y"


# Swap PgUp/Dn and Up/Down Arrow: this value allows you to specify
# the initial value of the Up/Down Arrow and PgUp/Dn keys.  When
# swapped, the program is more usable for reading documents on 95's
# and 100's.  The setting can be toggled dynamically with the ^X -
# command.  Values:
#	"Y"	Yes, initial settings toggled.
#	"N"	No, initial settings normal.

SWAPPGLN	"N"


# Visible grayspace: this value allows you to specify whether
# grayspace (spaces, tabs, and hard newlines) are displayed as "visible"
# characters (ordinarily ., >, and <, respectively).  The setting can be
# toggled dynamically with the ^X ` command.  Values:
#	"Y"	Yes, display them as visible characters.
#	"N"	No, don't display them.

VISGRAY		"N"


# Use caret notation: this value allows you to specify how control
# and meta characters (other than tab, soft NL, and hard NL) are
# displayed.  Values:
#	"Y"	Yes, display them with ^ notation.
#	"N	No, do not convert them before display (allows display
#		of 8-bit characters; not recommended on V and T
#		terminal types)

USECARAT _u	"Y"
	_*	"N"


# System buffers: this value allows you to specify whether system
# buffers should be skipped during buffer commands.  Regardless of the
# setting of this value, you can specify that system buffers not be
# skipped by supplying an argument to the buffer command.  Values:
#	"Y"	Yes, skip them
#	"N	No, do not skip them

SKIPSYS		"Y"


# Saving unmodified files: this value allows you to specify whether the
# ^X ^S command always saves the file, or only saves the file if it is
# modified.  Values:
#	"Y"	Yes, save unmodified files
#	"N"	No, do not save unmodified files

SAVEUNMOD	"Y"


# Backup: this value allows you to specify whether to automatically create
# backup (.BAK) versions of a file that is being saved.  Values:
#	"Y"	Yes, make them
#	"N"	No, do not make them

BACKUP		"N"


# Initial file: this value allows you to specify an initial file to
# load.  This value is _not_ used if a files are specified on the
# command line.  A "" string means to load no file.

INITFILE	""


# Default directory: this value allows you to specify an default
# working directory to change to.  A "" string means to leave the
# working directory unchanged.

DEFDIR		""


# Use Getter: this value allows you to specify whether Freyja's file
# completion or the system manager's file getter code is used to
# enter file names.  This value is only used on the system manager
# version.  Values:
#	"F"	use Freyja file completion
#	"G"	use system manager file getter

USEGETTER	_s "G"
		_* "F"


# Default file pattern: this value allows you to specify the file
# pattern to use in the file getter.  While you can specify any star
# pattern (using regular MS/DOS * and ? characters), it is mainly used
# to specify a default extension (e.g., "*.txt" or "*.doc" or "*." (for
# no extension)).  This value is only used on the system manager version
# and with USEGETTER set to "G".

DEFPAT		"*.*"


# Calendar start day: this value allows you to set the first
# (leftmost) day of the week for the calendar.  0-Sunday, 1-Monday, etc.

CALSTART	0


# Radix: this value allows you to set the default radix to use in the
# calculator.  Values are "." and ",".

RADIX		"."


# ------------------------------------------------------------
# Per-Buffer Values
# ------------------------------------------------------------
# These values establish initial values for these parameters.
# However, they are maintained on a per-buffer basis and changing the
# values in any one buffer does not affect any other existing buffers.
# New buffers inherit their values from the buffer that was current just
# before the new buffer was created.


# Left margin: this value specifies the left margin column (zero
# origin).  The value can be changed with the ^X . command.

LMAR		0


# Right margin: this value specifies the right margin column (zero
# origin).  The value can be changed with the ^X F command.

RMAR		70


# Tab spacing: this value specifies the tab spacing in columns.  The
# value can be changed with the ^X , command.

TABW		8


# Fill mode: this value allows you to specify the fill mode.  Values:
#	"N"	No filling is performed.
#	"F"	"Hard" filling is performed: lines are filled with
#		hard NLs as new characters are typed.  Only the
#		current line is refilled.
#	"W"	The buffer is in "word wrap" mode: the entire current
#		paragraph (up to the next hard NL) is refilled as
#		typing and editing is performed.
# Fill-style newlines use ^J to indicate a line break and two ^Js to
# indicate a paragraph break.  Wrap-style newlines use soft newline (^_)
# internally to indicate a new line and ^J to indicate a paragraph
# break.

FILL		"N"


# ------------------------------------------------------------
# Starting here, you probably don't want to actually change anything...
# ------------------------------------------------------------

# This value tells what version the resource file was complied for.
# It simply provides a way to get this information into the resource
# file.

MADEFOR		"^&"


# Screen method: This value specifies the method used to handle screen
# output.  The value in this file can be overridden on the command
# line with the -m option.  Values:
#
#	"@"	use default as implied by font
#	"B"	force memory mapped -> BIOS calls (MSDOS only)

SCRNMETHOD	"@"


# Type list: this list allows you to specify the screen types that the
# ^X + command cycles among.  Values are as from the SCRTYPE list.

TYPELIST	list8
			?u  28
			?js 24 ?js 25 ?js 26
			?cd  2 ?cd 80 ?cd 84
			?i   2 ?i   0	# MDA displays don't cycle
					# EGA mode 15 must be explicitly
					#  requested
			101 .


# This list contains the attribute values to use for various displays.
# Don't change the order or add/delete any entries.  Each entry is
# four bytes and contains the attribute for:
#
#	normal character
#	reverse video character
#	underscored character
#	reverse video, underscored character

ATTRLIST list8
	15 112 4 112	# 0 CGA text, 1 bit/pixel
	15 112 4 112	# 1 CGA text, 4 bits/pixel
	?cd 7 !cd 15 112 ?cd 8 !cd 4 112	# 2 CGA text, 1 bit/pixel
	15 112 4 112	# 3 CGA text, 4 bits/pixel
	127 127 127 127	# 4 CGA graphics, 2 bits/pixel
	127 127 127 127	# 5 CGA graphics,gray, 2 bits/pixel
	127 127 127 127	# 6 CGA graphics, 1 bit/pixel
	7 112 1 9	# 7 MDA text
	0 0 0 0		# 8 PCJr unsupported, 4 bits/pixel
	0 0 0 0		# 9 PCJr unsupported, 4 bits/pixel
	0 0 0 0		# 10 PCJr,EGA unsupported, 2,6 bits/pixel
	0 0 0 0		# 11 ? unsupported, ?
	0 0 0 0		# 12 ? unsupported, ?
	127 127 127 127	# 13 EGA graphics, 4 bits/pixel
	127 127 127 127	# 14 EGA graphics, 4 bits/pixel
	127 127 127 127	# 15 EGA graphics, 2 bits/pixel
	0 0 0 0		# 16 EGA unsupported, ?
	7 112 1 9	# 24 95LX text
	0 1 2 3		# 25 95LX graphics (please don't change)
	0 1 2 3		# 26 95LX graphics (please don't change)
	15 112 4 112	# 27 100LX text, 1 bit/pixel
	0 1 2 3		# 28 VT100 (please don't change)
	0 1 0 1		# 29 termcap (please don't change)
	?cd 7 !cd 15 112 ?cd 8 !cd 4 112	# 80 100LX text, 1 bit/pixel
	15 112 4 112	# 81 100LX text, 2 bits/pixel
	15 112 4 112	# 82 100LX text, 1 bit/pixel
	15 112 4 112	# 83 100LX text, 2 bits/pixel
	?cd 7 !cd 15 112 ?cd 8 !cd 4 112	# 84 100LX text, 1 bit/pixel
	15 112 4 112	# 85 100LX text, 2 bits/pixel
	.


# Box drawing characters: this value holds the list of characters used
# to draw boxes.  The order is:
#
#	UL corner
#	top
#	UR corner
#	left
#	right
#	LL corner
#	bottom
#	LR corner

BOXCHARS	_u "+-+||+-+"
		_* "\xDA\xC4\xBF\xB3\xB3\xC0\xC4\xD9"


# Window split character: this value allows you to specify the
# character to use (in reverse video) as the window divider line.

WINDSPLITCHAR	"-"


# Menu split character: this value allows you to specify the
# character to use (in reverse video) as the menu divider line.

MENU0SPLITCHAR	_u "="		# type 0 menus
		_* "\xCD"


# Other split character: this value allows you to specify the
# character to use (in reverse video) as the divider line for
# other purposes.

OTHERSPLITCHAR	"*"


# The next three values allow you to specify the characters to use
# when graspace is being made visible.

VIS_SP_CHAR	_u	"."
		_*	"\xFA"
VIS_TAB_CHAR	_u	">"
		_*	"\x10"
VIS_NL_CHAR	_u	"<"
		_*	"\x11"


# Preferred percentage: this value allows you to specify the preferred
# percentage that a "centered" cursor is from the top of the window.

PREFPCT		40


# Horizontal overlap: this value allows you to specify the number of
# columns to overlap for horizontal scrolling.

HOVERLAP	5


# Vertical overlap: this value allows you to specify the number of
# rows to overlap for moving up or down by a screen.

VOVERLAP	2


# The name of the program to use when writing the clipboard.  This value
# is used only by the system manager version.

CBPROG		_sc "Freyja"
		_* ""


# The name of the clipboard tag type to use when reading and writing the
# clipboard.  This value is used only by the system manager version.

CBTAG		_sc "TEXT"
		_* ""


# ------------------------------------------------------------
# Tiny Font Table
#

table	TINYFONT
-	list8 ?js fontmap 0 .


# ------------------------------------------------------------
# Base Dispatch Table
#
# This table is exactly 128 entries long.  It containst the dispatch
# table used for the base command table (no prefix keys).  Entries
# should use the symbols from the Help Strings Table in rlang*.res.

table		BASE
-		list8

# ^@^A^B^C^D^E^F^G
RMARKSET CLINEA CCHARB WCASEROTATE CCHARFD CLINEE CCHARF MABORT

# ^H^I^J^K^L^M^N^O
CCHARBD SI WINDNEXT CLINEFD DSCREENRANGE WINSNL CLINEF WINSNLA

# ^P^Q^R^S^T^U^V^W
CLINEB MQUOTE MSEARCHB MSEARCHF CCHARTRAN MUARG CSCRNF RREGDELETE

# ^X^Y^Z^[^\^]^^^_
MCTRLX RYANK CSCRNB MMETA WINDDEL KFROMMAC MMENUM UC

SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI	#  !"#$%&'()*+,-./
SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI	# 0123456789:;<=>?
SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI	# @ABCDEFGHIJKLMNO
SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI	# PQRSTUVWXYZ[\]^_
SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI	# `abcdefghijklmno
SI SI SI SI SI SI SI SI SI SI SI SI SI SI SI 	# pqrstuvwxyz{|}~

# ^?
?u CCHARBD !u WWORDBD
.


# ------------------------------------------------------------
# Ctrl-X Dispatch Table
#
# This table is exactly 128 entries long.  It containst the dispatch
# table used for the Control-X command table.  Entries should use the
# symbols from the Help Strings Table in rlang*.res.

table		CTX
-		list8

# ^@^A^B^C^D^E^F^G
UC UC FBUFLIST MEXIT FDIRED FCOMMAND FFILEFIND MABORT

# ^H^I^J^K^L^M^N^O
WSENTBD FFILEINS UC FFILEDELETE RLOWER UC UC UC

# ^P^Q^R^S^T^U^V^W
RPRINT UC FFILEREAD FFILESAVE UC RUPPER CSCRNFOW FFILEWRITE

# ^X^Y^Z^[^\^]^^^_
RMARKSWAP UC CSCRNBOW UC RDELWHITE UC UC UC

#  !"#$%&'
UC UC UC WPRINTMAR UC UC UC UC

# ()*+,-./
KBEGMAC KENDMAC UC MFONT WSETTABS MTOGPGLN WSETLEFT WSETFILL

# 01234567
UC DWINDONE DWINDTWO DWINDTWOO UC UC UC RMETA

# 89:;<=>?
MINS8 MDOKEY UC UC DLEFT WPRINTPOS DRIGHT UC

# @ABCDEFG
MCHARAT UC FBUFNEXT MEXIT FTODIRED KFROMMAC WSETRIGHT UC

# HIJKLMNO
MMENUH UC UC FBUFDEL UC UC UC DWINDSWAP

# PQRSTUVW
FBUFPRINT MQUOTE UC MSEARCHF RTABIFY RUNTABIFY FBUFPREV UC

# XYZ[\]^_
UC UC UC ROUTDENT UC RINDENT DWINDGROW UC

# `abcdefg
DTOGVISGRAY UC FBUFNEXT MEXIT FTODIRED KFROMMAC WSETRIGHT UC

# hijklmno
MMENUH UC UC FBUFDEL UC UC UC DWINDSWAP

# pqrstuvw
FBUFPRINT MQUOTE UC MSEARCHF RTABIFY RUNTABIFY FBUFPREV UC

# xyz{|}~^?
UC UC UC RHARDTOSOFT UC RSOFTTOHARD UC WSENTBD
.


# ------------------------------------------------------------
# Meta Dispatch Table
#
# This table is exactly 128 entries long.  It containst the dispatch
# table used for the Control-[ (Meta prefix) command table.  Entries
# should use the symbols from the Help Strings Table in rlang*.res.

table		ESC
-		list8

# ^@^A^B^C^D^E^F^G
UC WNUMMARK WNUMB DCAL UC UENTER WNUMF MABORT

# ^H^I^J^K^L^M^N^O
WWORDBD UC UC CLINEAED DNEWDISPLAY UPRINTX DNEXT UC

# ^P^Q^R^S^T^U^V^W
DPREV UC MREPLACEQ UC UC ?cs UC !cs UCALC UC MMAKEDELETE

# ^X^Y^Z^[^\^]^^^_
UC UC UC UC UC KLOADMAC UC UC

#  !"#$%&'
RMARKSET UC UC UC UC UC UC UC

# ()*+,-./
UC UC UC UC UC UC UC UC

# 01234567
MMACEVAL MMACEVAL MMACEVAL MMACEVAL MMACEVAL MMACEVAL MMACEVAL MMACEVAL 

# 89:;<=>?
MMACEVAL MMACEVAL UC UC FBUFBEG WPRINTLINE FBUFEND UC

# @ABCDEFG
UC WSENTB WWORDB WWORDCAP WWORDFD WSENTF WWORDF RREGFILL

# HIJKLMNO (keep ESC O open for VT100 function key support)
WPARAMARK UC UC WSENTFD WWORDLOW UC UC UC

# PQRSTUVW
UC WPARAFILL MREPLACE WLINECENTER WWORDTRAN WWORDUP CSCRNB RREGCOPY

# XYZ[\]^_
UC UC UC WPARAB WJOINGRAY WPARAF UC UC

# `abcdefg
UC WSENTB WWORDB WWORDCAP WWORDFD WSENTF WWORDF RREGFILL

# hijklmno
WPARAMARK UC UC WSENTFD WWORDLOW UC UC UC

# pqrstuvw
UC WPARAFILL MREPLACE WLINECENTER WWORDTRAN WWORDUP CSCRNB RREGCOPY

# xyz{|}~^?
UC UC UC UC UC UC BBUFUNMOD WWORDBD
.


# ------------------------------------------------------------
# Function Key Dispatch Table
#
# This table is a little over 128 entries long.  It containst the
# dispatch table used for function keys.  Entries should use the
# symbols from the Help Strings Table in rlang*.res.

table		FUNC
-		list8

# 0-9
MMENU KCOLON FFILEPATH RMARKSET MAPROPOS MDESCBIND MDESCKEY RSAVE MABOUT
FPRINTSEL

# 10-19
?u UC !u MATTR UC UC UC UC UC WPARAFILL RREGCOPY WSENTF MREPLACE

# 20-29
WWORDTRAN UC WWORDUP UC UC UC UC UC UC UC

# 30-39
WSENTB WLINECENTER WWORDFD WWORDF UC WPARAMARK UC WSENTFD WWORDLOW UC

# 40-49
UC UC UC UC UC UC WWORDCAP CSCRNB WWORDB UC

# 50-59
UC UC UC UC UC UC UC UC UC MMENUH

# 60-69
CLINEAED WJOINGRAY WINDTHIS FBUFNEXT DWINDSWAP MSEARCHF FFILESAVE ?cs UC !cs
UCALC MMENUM UC

# 70-79
UC FBUFBEG CLINEB CSCRNB UC CCHARB UC CCHARF UC FBUFEND

# 80-89
CLINEF CSCRNF WINSNLA CCHARFD UC UC UC UC FBUFDEL DWINDTOG

# 90-99
UC FFILEFIND DCAL MEXIT MMENUH ?cs JCOPY !cs RREGCOPY ?cs JCUT !cs RREGDELETE
?cs JPASTE !cs RYANK MSEARCHB UC

# 100-109
MSEARCHF FSAVEALL RMARKSET MINSPGBK UC UC UC UC UC UC

# 110-119
UC UC UC UC UC WWORDB WWORDF UC UC UC

# 120-129
MMACEVAL MMACEVAL MMACEVAL MMACEVAL MMACEVAL MMACEVAL MMACEVAL
MMACEVAL MMACEVAL MMACEVAL

# 130-139
UC WPRINTLINE UC UC UC UC UC UC UC UC

# 140-149
UC UC UC UC UC UC UC UC UC UC

# 150-159
UC UC UC UC UC UC UC UC UC UC

# 160-169
UC UC UC UC UC UC UC UC UC UC

# 170-179
UC UC UC UC UC UC UC UC UC UC

# 180-189
UC UC UC UC UC UC UC UC UC UC

# 190-199
UC UC UC UC UC UC UC UC UC UC

# 200-209
MMENUM UC UC UC UC UC UC UC MFONT MDATE

# 210-219
MTIME UC ?cs JCUT !cs RREGDELETE ?cs JCOPY !cs RREGCOPY ?cs JPASTE !cs
RYANK UC UC UC UC UC

# 220-229
UC UC UC UC UC UC UC UC UC UC

# 230-239
UC UC UC UC UC UC UC UC UC UC

# 240-249
UC UC UC UC UC UC UC UC UC UC

# 250-255
UC UC UC UC UC UC
.


# ------------------------------------------------------------
# Keyboard Macros Table
#
# This table holds the pre-defined keyboard macros.  Note that the
# fifth entry (value 4) can _only_ be used by Esc 4 or Alt-4, it cannot
# be loaded with Esc ^]...
#
# When using these with Esc # (or Alt-#), you can _not_ include any
# menu sequences.  The code that executes these and the menu code
# share a buffer.  As all menu operations are available via regular
# key sequences, this should not be a problem.
#
# You can include menu sequences in macros loaded with Esc ^].

# These had all _better_ be key sequences.

table	KEYM

-	key	""	# 0
-	key	""	# 1
-	key	""	# 2
-	key	""	# 3
-	key	""	# 4 restricted use
-	key	""	# 5
-	key	""	# 6
-	key	""	# 7
-	key	""	# 8
-	key	""	# 9


# ------------------------------------------------------------
# This is where you put more printers or menus.

# The general form of an addition printer is:
#
#table		<name>
#-		<value 0>
#-		<value 1>
#-		<value 2>
#-		<value 3>
#-		<value 4>
#-		<value 5>
#-		<value 6>


# ------------------------------------------------------------
# Summary table.
# 

# This _must_ be the last table and is used by Freyja to locate the
# main and help menus, and the default printer.

table		-
-	MAIN
-	HELP
-	PRINTER1

