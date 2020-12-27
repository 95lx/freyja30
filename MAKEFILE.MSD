# Makefile for Freyja2

# Uncomment exactly one of the following two sections.  After changing
# which one is commented out, you will have to recompile everything.
# You can ensure this recompilation by doing "erase *.obj" or "touch *.h"

# Use these for a regular version

FOPTS = -f
FOBJFILES = fcalc.obj fjaguar.obj
FPROG = freyja.exe

# Use these for a system-manager compliant version (must not have
# the calculator, as it uses floating point)

# FOPTS = -f- -DSYSMGR -DNOCALC
# FOBJFILES = smjaguar.obj
# FPROG = freyja.exm

# Use these for a regular version without the calculator:

#FOPTS = -f- -DNOCALC
#FOBJFILES = fjaguar.obj
#FPROG = freyja.exe

# --------------------------------------------------

# In many cases, the tlink program should generate this warning message:
# "Warning: __stklen defined in module NAME.c is duplicated in module STKLEN"

PROGS = makeexm.exe $(FPROG) moon.exe thorabk.exe thorpbk.exe thorres.exe memutil.exm

all:	$(PROGS)

LIB = lib.obj libasm.obj libparse.obj libreade.obj libres.obj xprintf.obj
SPLIB = lib libasm libparse libreade libres xprintf
H = lib.h

CFLAGS = -O

# --------------------------------------------------

# /v for debugging, /m->/s for detailed symbol table
LOPTS = /c/d/m

# -v for debugging
.c.obj:
	tcc $(FOPTS) $*

.asm.obj:
	tasm $* /mx;

# --------------------------------------------------

freyja.ri: thorres.exe freyja.res rpage437.res rlangen.res
	thorres -c 437 -l en -f i freyja.res -h frres.h

# --------------------------------------------------

newri: thorres.exe freyja.res rpage437.res rlangen.res
	thorres -c 437 -l en -f i freyja.res -h frres.h -new

# --------------------------------------------------
# make IBM PC version

inibmpc.ri:	freyja.ri
	copy freyja.ri inibmpc.ri

# --------------------------------------------------
# make Jaguar (non system manager) version

indos95.ri:	thorres.exe freyja.res rpage850.res rlangen.res
	thorres -c 850 -l en -f j freyja.res -h frres.h -o indos95.ri

# --------------------------------------------------
# make System Manager version

insys95.ri:	thorres.exe freyja.res rpage850.res rlangen.res
	thorres -c 850 -l en -f s freyja.res -h frres.h -o insys95.ri

# --------------------------------------------------
# make Cougar DOS version

indos100.ri:	thorres.exe freyja.res rpage850.res rlangen.res
	thorres -c 850 -l en -f d freyja.res -h frres.h -o indos100.ri

# --------------------------------------------------
# make Cougar system manager version

insys100.ri:	thorres.exe freyja.res rpage850.res rlangen.res
	thorres -c 850 -l en -f c freyja.res -h frres.h -o insys100.ri

# --------------------------------------------------

FH = freyja.h frres.h $(H)
FLCORE = lib.obj libres.obj xprintf.obj libasm.obj
FCORE = fbuf.obj fchar.obj fdate.obj fdisplay.obj ffile.obj fkey.obj fmisc.obj fregion.obj ftable.obj fterm.obj fwhite.obj fword.obj $(FOBJFILES)

$(FCORE):	$(FH)

freyja.exe:	freyja.ri freyja.obj $(FCORE) $(FLCORE)
	tlink $(LOPTS) @freyja.lnk

freyja.obj:	freyja.c $(FH)
	tcc -c $(CFLAGS) $(FOPTS) -DMAIN freyja.c

# --------------------------------------------------

freyja.exm:	makeexm.exe smfreyja.exe
	copy smfreyja.exe freyja.exm
	makeexm -p freyja smfreyja

smfreyja.exe:	freyja.ri c0s.obj freyja.obj $(FCORE) $(FLCORE)
	tlink $(LOPTS) @smfreyja.lnk

# --------------------------------------------------

MH = memutil.h $(H)

memutil.exm:	makeexm.exe memutil.exe
	copy memutil.exe memutil.exm
	makeexm -p memutil

memutil.exe:	c0s.obj memutil.obj memapp.obj memchain.obj memhelp.obj lib.obj libasm.obj xprintf.obj
	tlink $(LOPTS) c0s memutil memapp memchain memhelp lib libasm xprintf,memutil,memutil,\lib\cs

memutil.obj:	memutil.c $(MH) sysmgr.h
	tcc -c $(COPTS) -DMAIN memutil.c

memapp.obj: $(MH)

memchain.obj: $(MH)

memhelp.obj: $(MH)

# --------------------------------------------------

makeexm.exe:	makeexm.obj $(LIB)
	tlink $(LOPTS) \lib\c0s makeexm lib libasm xprintf,makeexm,makeexm,\lib\cs

makeexm.obj:	makeexm.c $(H)
	tcc -c $(CFLAGS) -DMAIN makeexm.C

# --------------------------------------------------

moon.exe:	moon.obj $(LIB)
	tlink $(LOPTS) \lib\c0s moon $(SPLIB),moon,moon,\lib\cs+\lib\maths+\lib\emu

moon.obj:	moon.c $(H)
	tcc -c -f $(CFLAGS) -DMAIN moon.C

# --------------------------------------------------

thorabk.exe:	thorabk.obj $(LIB)
	tlink $(LOPTS) \lib\c0s thorabk $(SPLIB),thorabk,thorabk,\lib\cs

thorabk.obj:	thorabk.c $(H)
	tcc -c $(CFLAGS) -DMAIN thorabk.c

# --------------------------------------------------

thorpbk.exe:	thorpbk.obj $(LIB)
	tlink $(LOPTS) \lib\c0s thorpbk $(SPLIB),thorpbk,thorpbk,\lib\cs

thorpbk.obj:	thorpbk.c $(H)
	tcc -c $(CFLAGS) -DMAIN thorpbk.c

# --------------------------------------------------

thorres.exe:	thorres.obj $(LIB)
	tlink $(LOPTS) \lib\c0s thorres $(SPLIB),thorres,thorres,\lib\cs

thorres.obj:	thorres.c $(H)
	tcc -c $(CFLAGS) -DMAIN thorres.c

# --------------------------------------------------

$(LIB):	$(H)

# --------------------------------------------------

# Build diskettes from MS/DOS system

disk:	inibmpc.ri indos95.ri insys95.ri indos100.ri insys100.ri
	touch *.*
	copy README d:
	copy anatomy.txt d:
	copy ascii.txt d:
	copy c0s.asm d:
	copy copying.doc d:
	copy fbuf.c d:
	copy fcalc.c d:
	copy fcalc.doc d:
	copy fchar.c d:
	copy fcmdlist.doc d:
	copy fcmdxref.doc d:
	copy fdate.c d:
	copy fdisplay.c d:
	copy ffile.c d:
	copy filelist d:
	copy filelist.doc d:
	copy fjaguar.c d:
	copy fkey.c d:
	copy fmenus.doc d:
	copy fmisc.c d:
	copy fregion.c d:
	copy freyja.c d:
	copy freyja.doc d:
	copy freyja.exe d:
	copy freyja.exm d:
	copy freyja.h d:
	copy freyja.icn d:
	copy freyja.lnk d:
	copy freyja.res d:
	copy frres.h d:
	copy fsf.doc d:
	copy ftable.c d:
	copy fterm.c d:
	copy ftutoria.doc d:
	copy fwhite.c d:
	copy fword.c d:
	copy indos100.doc d:
	copy indos100.ri d:
	copy indos95.doc d:
	copy indos95.ri d:
	copy inibmpc.doc d:
	copy inibmpc.ri d:
	copy insys100.doc d:
	copy insys100.ri d:
	copy insys95.doc d:
	copy insys95.ri d:
	copy inunix.doc d:
	copy league.doc d:
	copy lib.c d:
	copy lib.doc d:
	copy lib.h d:
	copy libasm.asm d:
	copy libparse.c d:
	copy libparse.doc d:
	copy libreade.c d:
	copy libres.c d:
	copy makeexm.c d:
	copy makeexm.doc d:
	copy makeexm.exe d:
	copy makefile.msd d:
	copy makefile.unx d:
	copy memapp.c d:
	copy memchain.c d:
	copy memhelp.c d:
	copy memutil.c d:
	copy memutil.doc d:
	copy memutil.exm d:
	copy memutil.h d:
	copy memutil.icn d:
	copy moon.c d:
	copy moon.doc d:
	copy moon.exe d:
	copy resource.doc d:
	copy rlangdk.res d:
	copy rlangen.res d:
	copy rpage000.res d:
	copy rpage437.res d:
	copy rpage850.res d:
	copy rpage852.res d:
	copy rpage865.res d:
	copy rpagebeg.res d:
	copy smfreyja.lnk d:
	copy smjaguar.c d:
	copy sysmgr.h d:
	copy thorabk.c d:
	copy thorabk.doc d:
	copy thorabk.exe d:
	copy thorpbk.c d:
	copy thorpbk.doc d:
	copy thorpbk.exe d:
	copy thorres.c d:
	copy thorres.doc d:
	copy thorres.exe d:
	copy turboc.cfg d:
	copy warranty.doc d:
	copy xprintf.c d:
	copy xprintf.doc d:

# Build diskettes from MS/DOS system

small:
	mkdir small
	copy README small
	copy copying.doc small
	copy fcalc.doc small
	copy fcmdlist.doc small
	copy fcmdxref.doc small
	copy filelist small
	copy filelist.doc small
	copy fmenus.doc small
	copy freyja.doc small
	copy freyja.exe small
	copy freyja.exm small
	copy freyja.icn small
	copy freyja.res small
	copy frres.h small
	copy fsf.doc small
	copy ftutoria.doc small
	copy indos100.doc small
	copy indos100.ri small
	copy indos95.doc small
	copy indos95.ri small
	copy inibmpc.doc small
	copy inibmpc.ri small
	copy insys100.doc small
	copy insys100.ri small
	copy insys95.doc small
	copy insys95.ri small
	copy inunix.doc small
	copy league.doc small
	copy makeexm.doc small
	copy makeexm.exe small
	copy memutil.doc small
	copy memutil.exm small
	copy memutil.icn small
	copy moon.doc small
	copy moon.exe small
	copy resource.doc small
	copy rlangdk.res small
	copy rlangen.res small
	copy rpage000.res small
	copy rpage437.res small
	copy rpage850.res small
	copy rpage852.res small
	copy rpage865.res small
	copy rpagebeg.res small
	copy thorabk.doc small
	copy thorabk.exe small
	copy thorpbk.doc small
	copy thorpbk.exe small
	copy thorres.doc small
	copy thorres.exe small
	copy warranty.doc small

# --------------------------------------------------

cleanup:
	-erase *.obj
	-erase *.map

cleanall:
	-erase *.obj
	-erase *.map
	-erase *.exe
	-erase *.exm
	-erase *.ri
