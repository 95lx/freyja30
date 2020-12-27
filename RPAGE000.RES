#		  Resource Source File, Version 2.2
#	     Character Set Definitions for Code Page 000 (USASCII only)
#
# See the file resource.doc for information on the syntax of this file.
#
# Basic entry format:
#
#	glyph <###> <code>
#	<line 0>
#	<line 1>
#	<line 2>
#	<line 3>
#	<line 4>
#	<line 5>
#
# <###> is the number of the character glyph.  Range 0 to 255.
#
# <code> is "" if no expanded form is supported, or a unique 2-character
# string.  When the form ^!XX is encountered in a string and XX matches
# this 2-character string, the <###> is substitued.  Otherwise, XX is
# output.
#
# <line 0> to <line 5> is exactly six strings.  They specify the bitmap.
# Each string must be exactly 4 characters long.  Non-space characters
# are set bits and space (32 decimal) characters represent reset bits.

# (see file rpagebeg.res)


# ------------------------------------------------------------
# Character Set Configuration Table
#
# This table holds the information required to recognize and convert
# tokens, uppercase letters, lowercase letters, etc.

table	CSET

# This item defines the attributes of each character.  Each value is a
# bit mask with the following assignments:
#
#	bit 2^0		lower case letter
#	bit 2^1		upper case letter
#	bit 2^2		same case letter (only one case available),
#			also covers other characters included in
#			tokens (e.g., digits and _ if you like)
#	bit 2^3		calculator number (digit, A-F, -#.,)
#	bit 2^4		whitespace character (space, tab)
#	bit 2^5		newline character
#	bit 2^6		sentence end character (.?!:)
#	bit 2^7		closing punctuation char ()]}"')

# Sentence end characters are what can end a sentence.  Closing
# punctuation characters are what can appear between the end of a
# sentence and the whitespace.

# You can't configure the paragraph break characters.  Sorry, ran out of bits.

CTYPE	list8
 0  0  0  0  0  0  0  0  0 16 32  0  0  0  0  0	# ^@^A^B^C^D^E^F^G^H^I^J^K^L^M^N^O
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 32	# ^P^Q^R^S^T^U^V^W^X^Y^Z^[^\^]^^^_
16 64 128 8  0  0  0 128 0 128 0  0  8  8  72 0	#  !"#$%&'()*+,-./
12 12 12 12 12 12 12 12 12 12 64  0  0  0  0 64	# 0123456789:;<=>?
 0 10 10 10 10 10 10  2  2  2  2  2  2  2  2  2	# @ABCDEFGHIJKLMNO
 2  2  2  2  2  2  2  2  2  2  2  0  0 128 0  0	# PQRSTUVWXYZ[\]^_
 0  9  9  9  9  9  9  1  1  1  1  1  1  1  1  1	# `abcdefghijklmno
 1  1  1  1  1  1  1  1  1  1  1  0  0 128 0  0	# pqrstuvwxyz{|}~^?
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 .


# This item lists the values to be _added to_ each character to
# convert it from upper to lower case.  Non-zero values are only present
# for upper case letters.

TOLOW	list8
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# ^@^A^B^C^D^E^F^G^H^I^J^K^L^M^N^O
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# ^P^Q^R^S^T^U^V^W^X^Y^Z^[^\^]^^^_
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	#  !"#$%&'()*+,-./
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# 0123456789:;<=>?
 0 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32	# @ABCDEFGHIJKLMNO
32 32 32 32 32 32 32 32 32 32 32  0  0  0  0  0	# PQRSTUVWXYZ[\]^_
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# `abcdefghijklmno
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# pqrstuvwxyz{|}~^?
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 .


# This item lists the values to be added to each character to convert
# it from lower to upper case.  Non-zero values are only present for
# lower case letters.

TOUPP	list8
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# ^@^A^B^C^D^E^F^G^H^I^J^K^L^M^N^O
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# ^P^Q^R^S^T^U^V^W^X^Y^Z^[^\^]^^^_
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	#  !"#$%&'()*+,-./
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# 0123456789:;<=>?
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# @ABCDEFGHIJKLMNO
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0	# PQRSTUVWXYZ[\]^_
0 224 224 224 224 224 224 224 224 224 224 224 224 224 224 224	# `abcdefghijklmno
224 224 224 224 224 224 224 224 224 224 224 0 0 0 0 0	# pqrstuvwxyz{|}~^?
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 .


