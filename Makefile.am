 ## Process this file with automake to produce Makefile.in


AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = wpurc README.base
SUBDIRS = bin doc src


######################################################################
# documentation rules

.PHONY: docu sgml txt doc html dvi ps ps.gz ps.zip

docu sgml txt doc html dvi ps ps.gz ps.zip:
	cd doc && $(MAKE) $@


#####################################################################
# WWW page rules

index.html indexpart.html:
	cd doc && $(MAKE) $@
	mv doc/$@ $@


######################################################################
# package compiling rules (binary packages, mostly cross-compile)
# All direcories are hard-coded in here. You _will_ have to change them.

linux: clean
	mv config.status config.status.orig
	-mkdir -p systems/i486-linux
	cd systems/i486-linux && \
		CC=/usr/bin/gcc \
		CXX=/usr/bin/g++ \
		../../configure --host=i486-linux
	cd systems/i486-linux && $(MAKE)
	mv config.status.orig config.status
	cp systems/i486-linux/src/lmpc $(top_srcdir)/bin/lmpc
	cp systems/i486-linux/src/demcut $(top_srcdir)/bin/demcut
	cp systems/i486-linux/src/dbs $(top_srcdir)/bin/dbs

linux-strip: linux
	/usr/i486-linux/bin/strip $(top_srcdir)/bin/lmpc
	/usr/i486-linux/bin/strip $(top_srcdir)/bin/demcut
	/usr/i486-linux/bin/strip $(top_srcdir)/bin/dbs


go32_TARGET=i386-pc-msdosdjgpp
go32_PREFIX=/usr/self/go32

go32: clean
	mv config.status config.status.orig
	-mkdir -p systems/$(go32_TARGET)
	cd systems/$(go32_TARGET) && \
		CC=$(go32_PREFIX)/$(go32_TARGET)/bin/gcc \
		CXX=$(go32_PREFIX)/$(go32_TARGET)/bin/g++ \
		RANLIB=$(go32_PREFIX)/$(go32_TARGET)/bin/ranlib \
		../../configure --host=$(go32_TARGET)
	cd systems/$(go32_TARGET) && $(MAKE)
	mv config.status.orig config.status
	cp systems/$(go32_TARGET)/src/lmpc.exe $(top_srcdir)/bin/lmpc.exe
	cp systems/$(go32_TARGET)/src/demcut.exe $(top_srcdir)/bin/demcut.exe

go32-strip: go32
	$(go32_PREFIX)/$(go32_TARGET)/bin/strip $(top_srcdir)/bin/lmpc.exe
	$(go32_PREFIX)/$(go32_TARGET)/bin/strip $(top_srcdir)/bin/demcut.exe


cygwin_TARGET=i386-pc-cygwin32
cygwin_PREFIX=/usr/self/cygnus

cygwin: clean
	mv config.status config.status.orig
	-mkdir -p systems/$(cygwin_TARGET)
	cd systems/$(cygwin_TARGET) && \
		CC=$(cygwin_PREFIX)/bin/$(cygwin_TARGET)-gcc \
		CXX=$(cygwin_PREFIX)/bin/$(cygwin_TARGET)-g++ \
		RANLIB=$(cygwin_PREFIX)/bin/$(cygwin_TARGET)-ranlib \
		../../configure --host=$(cygwin_TARGET)
	cd systems/$(cygwin_TARGET) && $(MAKE)
	mv config.status.orig config.status
	cp systems/$(cygwin_TARGET)/src/lmpc $(top_srcdir)/bin/lmpc32.exe
	cp systems/$(cygwin_TARGET)/src/demcut $(top_srcdir)/bin/demcut32.exe

cygwin-strip: cygwin
	$(cygwin_PREFIX)/$(cygwin_TARGET)/bin/strip $(top_srcdir)/bin/lmpc32.exe
	$(cygwin_PREFIX)/$(cygwin_TARGET)/bin/strip $(top_srcdir)/bin/demcut32.exe


mingw32_TARGET=i386-pc-mingw32
mingw32_PREFIX=/usr/self/cygnus

mingw32: clean
	mv config.status config.status.orig
	-mkdir -p systems/$(mingw32_TARGET)
	cd systems/$(mingw32_TARGET) && \
		CC=$(cygwin_PREFIX)/bin/$(cygwin_TARGET)-gcc \
		CXX=$(cygwin_PREFIX)/bin/$(cygwin_TARGET)-g++ \
		RANLIB=$(cygwin_PREFIX)/bin/$(cygwin_TARGET)-ranlib \
		CFLAGS=-mno-cygwin \
		LDFLAGS=-mno-cygwin \
		../../configure --host=$(mingw32_TARGET)
	cd systems/$(mingw32_TARGET) && $(MAKE)
	mv config.status.orig config.status
	cp systems/$(mingw32_TARGET)/src/lmpc $(top_srcdir)/bin/lmpcm.exe
	cp systems/$(mingw32_TARGET)/src/demcut $(top_srcdir)/bin/demcutm.exe

mingw32-strip: mingw32
	$(cygwin_PREFIX)/$(cygwin_TARGET)/bin/strip $(top_srcdir)/bin/lmpcm.exe
	$(cygwin_PREFIX)/$(cygwin_TARGET)/bin/strip $(top_srcdir)/bin/demcutm.exe


######################################################################
# distribution rules

dist-hook README:
	$(WPU) --set ACTION=readme --set SRCDIR=$(srcdir) --set PACK=s $(srcdir)/README.base README

.PHONY: pack s l d w m f

pack: dist l d w m f

l d w m f:
	$(WPU) --set ACTION=readme --set PACK=$@ README.base README
	$(WPU) --set ACTION=packaging --set PACK=$@

s: dist

l: linux-strip txt

d: go32-strip doc

w: cygwin-strip doc

m: mingw32-strip doc

f: linux-strip go32-strip cygwin-strip mingw32-strip txt doc ps


######################################################################
# publishing rules

.PHONY: upload

upload: pack html index.html indexpart.html
	$(WPU) --set ACTION=upload


######################################################################
# removing additionals

CLEANFILES=index.html indexpart.html

