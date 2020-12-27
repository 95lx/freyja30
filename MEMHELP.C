/* MEMHELP.C -- Help Text For MemUtil

	Written April 1992 by Craig A. Finseth
	Copyright 1992,3 by Craig A. Finseth
*/

#include "memutil.h"

static char copies[3][41];

static char *text[] = {

"\xc9\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcb\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbb",
"\xba" "list/lists %04x:%04x \xba" "Copyright \xb8 1994\xba",
"\xba" "applic cbs %04x:%04x \xba" "Craig A. Finseth\xba",
"\xba" "                     \xba" "1343 Lafond     \xba",
"\xba" "MemUtil has six      \xba" "St Paul MN 55104\xba",
"\xba" "different displays:  \xba" "USA             \xba",
"\xba" "  Help               \xba+1 612 644 4027 \xba",
"\xba" "  Applications       \xba" "fin@unet.umn.edu\xba",
"\xba" "  Long Applications  \xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xb9",
"\xba" "  Chains                              \xba",
"\xba" "  Memory                              \xba",
"\xba" "  Characters                          \xba",
"\xba" "                                      \xba",
"\xba" "To move among these displays, simply  \xba",
"\xba" "press the corresponding function key  \xba",
"\xba" "(F1-F6).                              \xba",
"\xcc\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xb9",
"\xba" "GENERAL ITEMS                         \xba",
"\xba" "  Pretty much all input/output is in  \xba",
"\xba" "hexadecimal.                          \xba",
"\xba" "  All memory references are in units  \xba",
"\xba" "of paragraphs (16 bytes).             \xba",
"\xba" "  Whenever the program asks for a     \xba",
"\xba" "numeric value, you can enter a value  \xba",
"\xba" "in the form seg:offset and the program\xba",
"\xba" "will calculate the correct paragraph. \xba",
"\xba" "  The double line across the top shows\xba",
"\xba" "a single, \"slider\" bar.  This bar     \xba",
"\xba" "indicates where the current screen is \xba",
"\xba" "in relation to the overall display.   \xba",
"\xcc\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xb9",
"\xba" "IN DETAIL                             \xba",
"\xba" "                                      \xba",
"\xba" "HELP: This display contains the help  \xba",
"\xba" "text that you are reading.  The first \xba",
"\xba" "two lines show the start of the MS/DOS\xba",
"\xba" "\"List of Lists\" and the System        \xba",
"\xba" "Manager's application control blocks, \xba",
"\xba" "as found by this program.             \xba",
"\xba" "                                      \xba",
"\xba" "APPLICATIONS: This display shows a    \xba",
"\xba" "list of all known applications.  The  \xba",
"\xba" "system applications are listed first, \xba",
"\xba" "then the user applications. However,  \xba",
"\xba" "the display starts off with the first \xba",
"\xba" "user application: you must move       \xba",
"\xba" "backwards (up) to see the system      \xba",
"\xba" "applications.  The \"current item\" for \xba",
"\xba" "this display and the Long Applications\xba",
"\xba" "display are always the same.          \xba",
"\xba" "                                      \xba",
"\xba" "LONG APPLICATIONS: This is essentially\xba",
"\xba" "the same as the Applications display, \xba",
"\xba" "but it devotes the entire screen to   \xba",
"\xba" "full information on just one          \xba",
"\xba" "application.  No, I don't know what   \xba",
"\xba" "all the fields mean: your guess is as \xba",
"\xba" "good as mine.                         \xba",
"\xba" "                                      \xba",
"\xba" "CHAINS: This display shows all MS/DOS \xba",
"\xba" "allocation chains.  It is pretty      \xba",
"\xba" "boring.                               \xba",
"\xba" "                                      \xba",
"\xba" "MEMORY: This display shows a memory   \xba",
"\xba" "block in hex and as characters.  Each \xba",
"\xba" "paragraph is labelled on its first    \xba",
"\xba" "line with its (paragraph) starting    \xba",
"\xba" "address and on its second line with   \xba",
"\xba" "the offset in paragraphs from the     \xba",
"\xba" "start of the block.  If you use the   \xba",
"\xba" "Enter key to move to this display from\xba",
"\xba" "the Applications or Long Applications \xba",
"\xba" "display, it shows the application's   \xba",
"\xba" "data segment (assumed to be 64K long).\xba",
"\xba" "If you move here from the Chains      \xba",
"\xba" "display, it shows the contents of the \xba",
"\xba" "selected chain.                       \xba",
"\xba" "                                      \xba",
"\xba" "CHARACTERS: This display shows each   \xba",
"\xba" "character of the HP95LX's character   \xba",
"\xba" "set in hex, decimal, octal, in \"carat\"\xba",
"\xba" "notation, and as a character.  It also\xba",
"\xba" "shows all scan codes and function /   \xba",
"\xba" "special key names.  It is 512 items   \xba",
"\xba" "long.                                 \xba",
"\xba" "                                      \xba",
"\xba" "Within each display, you move around  \xba",
"\xba" "the same way:                         \xba",
"\xba" "                                      \xba",
"\xba" "Home  moves to the first item         \xba",
"\xba" "End   moves to the last item          \xba",
"\xba" "\x18     moves up one item               \xba",
"\xba" "\x19     moves down one item             \xba",
"\xba" "PgUp  moves up one screen worth       \xba",
"\xba" "PgDn  moves down one screen worth     \xba",
"\xba" "\x1b     moves up by 10% of the items    \xba",
"\xba" "\x1a     moves down by 10% of the items  \xba",
"\xba" "                                      \xba",
"\xba" "You can also press the Offset(F10) key\xba",
"\xba" "and specify the offset (in hex) of the\xba",
"\xba" "item to move to.                      \xba",
"\xcc\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xb9",
"\xba" "SPECIAL KEYS                          \xba",
"\xba" "                                      \xba",
"\xba" "Para(F7): This key prompts for a      \xba",
"\xba" "starting paragraph and drops you into \xba",
"\xba" "the Memory display starting at that   \xba",
"\xba" "paragraph.  The length is unchanged.  \xba",
"\xba" "                                      \xba",
"\xba" "Length(F9)): This key prompts for a   \xba",
"\xba" "length (in paragraphs) and drops you  \xba",
"\xba" "into the Memory display with that     \xba",
"\xba" "length.  The starting address is      \xba",
"\xba" "unchanged.                            \xba",
"\xba" "                                      \xba",
"\xba" "Key(F8): This key prompts for a key   \xba",
"\xba" "and drops you into the Characters     \xba",
"\xba" "display starting at that key.         \xba",
"\xba" "                                      \xba",
"\xba" "Esc: The program keeps track of the   \xba",
"\xba" "last six applications that you        \xba",
"\xba" "visited.  This key \"pops\" you back    \xba",
"\xba" "one application.                      \xba",
"\xba" "                                      \xba",
"\xba" "Enter: This key moves you into \"more  \xba",
"\xba" "information\" on the current item.  It \xba",
"\xba" "moves you:                            \xba",
"\xba" "  from Applications to Long Apps      \xba",
"\xba" "  from Long Applications to Memory    \xba",
"\xba" "  from Chains to Memory               \xba",
"\xcc\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xb9",
"\xba" "MENU ITEMS                            \xba",
"\xba" "                                      \xba",
"\xba" "The program has seven menu options, of\xba",
"\xba" "which four need explanation.  All menu\xba",
"\xba" "items work on all information in the  \xba",
"\xba" "application: not just the part that is\xba",
"\xba" "currently displayed.  In addition, the\xba",
"\xba" "format may be slightly different than \xba",
"\xba" "used for display.                     \xba",
"\xba" "                                      \xba",
"\xba" "C)\x1a" "Clip      copies to the clipboard  \xba",
"\xba" "             (replaces any clipboard  \xba",
"\xba" "             text)                    \xba",
"\xba" "                                      \xba",
"\xba" "Debug        Executes the debugger.   \xba",
"\xba" "                                      \xba",
"\xba" "ExternalCommand   Prompts for and     \xba",
"\xba" "             executes an arbitrary    \xba",
"\xba" "             MS/DOS command.          \xba",
"\xba" "                                      \xba",
"\xba" "F)\x1a" "File      copies to a specified    \xba",
"\xba" "             file (overwrites if it   \xba",
"\xba" "             already exists)          \xba",
"\xba" "                                      \xba",
"\xba" "G)\x1a" "Clip(bin) copies in binary form    \xba",
"\xba" "                                      \xba",
"\xba" "H)\x1a" "File(bin) copies in binary form    \xba",
"\xba" "                                      \xba",
"\xba" "The Help, Chains, and Characters      \xba",
"\xba" "displays are only saved in text form. \xba",
"\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc"
	};

/* ------------------------------------------------------------ */

/* Return a pointer to the specified help line. */

char *
Help_Line(int num)
	{
	if (num < 3)
		return(copies[num]);
	else	return(text[num]);
	}


/* ------------------------------------------------------------ */

/* Return the number of lines of help text. */

int
Help_Num(void)
	{
	return(sizeof(text) / sizeof(text[0]));
	}


/* ------------------------------------------------------------ */

/* Poke the segment information into the help text. */

void
Help_Setup(void)
	{
	xstrcpy(copies[0], text[0]);
	if (lolseg == 0 && loloff == 0) {
		xstrcpy(copies[1], text[1]);
		memmove(copies[1] + 12, "????:????", 9);
		}
	else	xsprintf(copies[1], text[1], lolseg, loloff);

	xsprintf(copies[2], text[2], appseg, appoff);
	}


/* end of MEMHELP.C -- Help Text For MemUtil */
