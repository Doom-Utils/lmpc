## Process this file with automake to produce Makefile.in

GENTOK=$(PERL) $(top_srcdir)/bin/gentok
EXTRACT=$(PERL) $(top_srcdir)/bin/extract


CALC_FROM_SPECS = udm2-te1.c udm2-te2.c udm2-te3.c


bin_PROGRAMS = $(DBS) demcut lmpc
EXTRA_PROGRAMS = dbs
EXTRA_HEADERS = 
noinst_LIBRARIES = libutil.a
noinst_HEADERS = getopt.h alloca.h binblock.h lmpc.h node.h tools.h udem.h \
	udm2.h $(UDM3_H) \
udmo.h ulmp.h uqwd.h record.h
EXTRA_DIST = udeml.l.in udemy.y.in tokenlist.in $(CALC_FROM_SPECS)
# CLEANFILES = udemy.y udemy.c udemy.h udeml.l udeml.c tokenlist tokenlist.tmp
CLEANFILES = udemy.y udemy.c udeml.l udeml.c tokenlist tokenlist.tmp

BUILT_SOURCES = udeml.l udemy.y 

dbs_SOURCES = dbs.c node.c udem.c
dbs_LDADD = $(NETLIBS) libutil.a

demcut_SOURCES = demcut.c udem.c
demcut_LDADD = libutil.a

lmpc_SOURCES = lmpc.c ulmp.c lzw.c unlzw.c udmo.c udem.c udemy.y udeml.l \
uqwd.c udm2.c $(UDM3_C)
lmpc_LDADD = libutil.a $(UDM3_O) @LEXLIB@ @ALLOCA@
lmpc_DEPENDENCIES = libutil.a $(UDM3_O) @ALLOCA@

$(UDM3_O): $(UDM3_C) $(UDM3_H)

libutil_a_SOURCES = tools.c node.c binblock.c record.c
libutil_a_LIBADD = @LIBOBJS@

TOKENBASEFILES = udem.c uqwd.c udm2.c udm2-te3.c $(UDM3_C)

tokenlist: tokenlist.in $(TOKENBASEFILES)
	srcdir=$(srcdir) $(GENTOK) $(TOKENBASEFILES) < $(srcdir)/tokenlist.in > tokenlist.tmp
	cmp -s tokenlist.tmp tokenlist || cp tokenlist.tmp tokenlist

udemy.y: udemy.y.in tokenlist
	srcdir=$(srcdir) $(GENTOK) $(TOKENBASEFILES) < $(srcdir)/udemy.y.in > udemy.y

udeml.l: udeml.l.in tokenlist
	srcdir=$(srcdir) $(GENTOK) $(TOKENBASEFILES) < $(srcdir)/udeml.l.in > udeml.l

udm2.o: $(CALC_FROM_SPECS)

$(CALC_FROM_SPECS):
	echo extract from $(CALC_FROM_SPECS)
	$(EXTRACT)

YFLAGS = -d

dist-hook:
	cd $(distdir) && rm -f udemy.y udemy.c udemy.h udeml.l udeml.c
