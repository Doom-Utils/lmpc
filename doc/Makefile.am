## Process this file with automake to produce Makefile.in


PERL=@PERL@
SED=@SED@
SGML2HTML=@SGML2HTML@
SGML2LATEX=@SGML2LATEX@
SGML2TXT=@SGML2TXT@
WPU=@WPU@

EXTRA_DIST=$(PACKAGE).base COMPILE COMPILE-go32

.PHONY: docu sgml txt doc html dvi ps ps.gz ps.zip

docu: sgml txt doc html ps

sgml: $(PACKAGE).sgml

txt: $(PACKAGE).txt

doc: $(PACKAGE).doc

html: $(PACKAGE).html

dvi: $(PACKAGE).dvi

ps: $(PACKAGE).ps

ps.gz: $(PACKAGE).ps.gz

ps.zip: $(PACKAGE).ps.zip

index.html: $(PACKAGE).sgml
	$(WPU) --set ACTION=packageindex $< $@

indexpart.html: $(PACKAGE).sgml
	$(WPU) --set ACTION=indexpart $< $@

SUFFIXES=.base .sgml .txt .doc .html .dvi .ps

.base.sgml:
	$(WPU) --set ACTION=subst $< $@

.sgml.txt:
	$(SGML2TXT) $<

.txt.doc:
	$(PERL) -p -e "s/.\010//g" < $< > $@ 

.sgml.html:
	$(SGML2HTML) $<

.sgml.tex:
	$(SGML2LATEX) --papersize=a4 --output=tex $<

.sgml.dvi:
	$(SGML2LATEX) --papersize=a4 --output=dvi $<

.sgml.ps:
	$(SGML2LATEX) --papersize=a4 --output=ps $<

$(PACKAGE).ps.gz: $(PACKAGE).ps
	gzip --best --to-stdout $< > $@

$(PACKAGE).ps.zip: $(PACKAGE).ps
	$(ZIP) -9 $@ $<

CLEANFILES=$(PACKAGE).sgml \
$(PACKAGE).txt $(PACKAGE).doc \
$(PACKAGE).html $(PACKAGE)-?.html $(PACKAGE)-??.html \
$(PACKAGE).dvi $(PACKAGE).ps $(PACKAGE).ps.gz $(PACKAGE).ps.zip \
index.html indexpart.html
