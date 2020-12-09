# Top-level makefile for BREXX V2
#
# BREXX interpreter
# Version:  2.1
# Copyright (C) 1991-2011  Vasilis Vlachoudis (Vasilis.Vlachoudis@cern.ch)
#
# This interpreter is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#
# To add a new configuration for your system add it to the list below
# then update the make.cnf file.

RXVERSION = brexx-2.1.10
RELEASE   = 0
SPEC      = brexx.spec
BUILDIR   = ${HOME}/rpmbuild/
RPMSDIR   = ${BUILDIR}/RPMS
SRCDIR    = ${BUILDIR}/SOURCES
SPECDIR   = ${BUILDIR}/SPECS
BUILDDIR  = ${BUILDIR}/BUILD
TGZ       = ${RXVERSION}.tgz
RPMSRC    = $(RXVERSION)-$(RELEASE).src.rpm
RPM32     = $(RXVERSION)-$(RELEASE).i386.rpm
RPM64     = $(RXVERSION)-$(RELEASE).x86_64.rpm

#RXVERSION = brexx-`grep '^#define.VERSION' inc/rexx.h | head -1 | cut -f 2 -d'"'`

TARGETS =	aix aix_debug \
		alpha alpha_debug \
		android android_debug \
		amiga custom \
		cygwin cygwin_debug \
		gcc gcc_debug \
		hpux hpux_debug \
		hpux-gcc hpux-gcc_debug \
		irix irix_debug \
		termux \
		linux linux_debug \
		linux32 linux32_debug \
		linux64 linux64_debug \
		linux_intel linux_intel_debug \
		linux_noalign linux_noalign_debug \
		cegcc cegcc_debug \
		test test_debug macos \
		install

.PHONY: help
help: default

.PHONY: default
default:
	@echo "BREXX V2 makefile"
	@echo "Make file for $(RXVERSION)"
	@echo "Type one of the following:"
	@echo "  make aix                  for IBM RS/6000 with AIX"
	@echo "  make alpha                for Digital's Alpha"
	@echo "  make amiga                for an AmigaOS application"
	@echo "  make android              for an Android OS application"
	@echo "  make bc                   for a Borland C++, DOS application"
	@echo "  make bc86                 for a Borland C++, 8086 code"
	@echo "  make cegcc                for Windows CE gcc"
	@echo "  make custom               for user customizable built"
	@echo "  make cygwin               for cygwin"
	@echo "  make dos32                for a 32-bit DOS application"
	@echo "  make gcc                  for a generic system with GCC"
	@echo "  make hpux                 for HP systems with HPUX *"
	@echo "  make hpux-gcc             for HP systems with HPUX using GCC"
	@echo "  make irix                 for SGI systems with IRIX"
	@echo "  make linux                for Linux systems with GCC *"
	@echo "  make linux32              for Linux 32bit systems with GCC *"
	@echo "  make linux64              for Linux 64bit systems with GCC *"
	@echo "  make linux_intel          for Linux systems with intel compiler,"
	@echo "  make linux_noalign        for Linux systems with GCC,"
	@echo "  make macintosh            for a Mac (pre-Mac OSX) application"
	@echo "  make macos                for a macos application"
	@echo "  make mswindows            for Microsoft Windows"
	@echo "  make termux               for android termux"
	@echo "  make test                 for testing/experimenting"
	@echo "  make tar                  to prepare a .tar.gz file"
	@echo "  make tags                 to generate the ctags file"
	@echo "  make install              to install rexx"
	@echo "  make clean"
	@echo "(*) USE THIS ONE!"
	@echo "For a DEBUG version type one of the above with suffix _debug"
	@echo "i.e.:  make linux_debug"

$(TARGETS):
	cd lib;      $(MAKE) $@
	cd progs;    $(MAKE) $@
	cd doc;      $(MAKE) $@
	cd inc;      $(MAKE) $@
	cd lstring ; $(MAKE) $@
	cd src;      $(MAKE) $@
	cd modules;  $(MAKE) $@

dos32:
	cd lstring ; $(MAKE) $@
	cd ..
	cd modules ; $(MAKE) $@
	cd ..
	cd src ;     $(MAKE) $@

bc:
	cd lstring
	make -f makefile.bc
	cd ../src
	make -f makefile.bc
	cd ..

bc_debug:
	cd lstring
	make -f makefile.bc -D__DEBUG__
	cd ../src
	make -f makefile.bc -D__DEBUG__
	cd ..

bc86:
	cd lstring
	make -f makefile.bc -D__8086__
	cd ../src
	make -f makefile.bc -D__8086__
	cd ..

macintosh:
	@echo "See the README file for Macintosh intallation information"

mswindows:
	@echo "See the windows/README.WIN for installation information"

# Remove .o files, instr.cnt and profile out files
clean:
	rm -f lib/*.a
	cd lstring ; $(MAKE) $@
	cd modules ; $(MAKE) $@
	cd src;      $(MAKE) $@
	rm -Rf $(RXVERSION)

#rm -f $(TGZ)

tags:
	ctags inc/*.h src/*.c lstring/*.c bsub/*.c


${TGZ}: clean
	mkdir $(RXVERSION)
	cp AUTHORS COPYING ChangeLog INSTALL README $(RXVERSION)
	cp makefile make.cnf brexx.csh *.sh $(RXVERSION)
	for d in inc src lstring modules doc lib progs win cms mvs; do \
		mkdir $(RXVERSION)/$$d; \
		cp -Rdpvu $$d/* $(RXVERSION)/$$d; \
	done
	find $(RXVERSION) -maxdepth 2 -name "RCS" -type d -exec rm -rf {} \;
	tar czvf $@ $(RXVERSION)
	rm -Rf $(RXVERSION)

tar: ${TGZ}

dist: tar

${RPMSRC}: ${TGZ}
	cp $^ ${SRCDIR}
	cp ${SPEC} ${SPECDIR}
	rpmbuild -bs ${SPEC}

${RPM32}: ${RPMSRC}
	rpmbuild -bb --target=i386 ${SPEC}
	cp ${RPMSDIR}/i386/$@ .

${RPM64}: ${RPMSRC}
	rpmbuild -bb --target=x86_64 ${SPEC}
	cp ${RPMSDIR}/x86_64/$@ .

rpm32: ${RPM32}

rpm64: ${RPM64}

rpm: rpm32 rpm64
