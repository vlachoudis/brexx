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

############# INCLUDE #############

include ../make.cnf

############# MACROS ##############

SOURCES = \
	$(MAIN) \
	rexx.c \
	compile.c \
	expr.c \
	template.c \
	interpre.c \
	nextsymb.c \
	rexxfunc.c \
	builtin.c \
	rxstr.c \
	rxconv.c \
	rxmath.c \
	rxword.c \
	rxfiles.c \
	address.c \
	variable.c \
	trace.c \
	error.c \
	stack.c \
	dqueue.c \
	bintree.c \
	bmem.c \
	$(EXTRA)

OBJECTS = $(SOURCES:.c=.o)

############# RULES ###############

.c.o:
	$(CC) -c $(CCOPTIONS) $<

############# TARGETS #############

default:
	@echo "Specify a target configuration"

ifeq ($(STATIC),yes)
$(REXX_EXE):	$(OBJECTS)
	$(CC) -o $@ $(CCOPTIONS) $^ -L$(LIBDIR) -llstring $(LIBEXTRA)
else
$(REXX_EXE):	$(OBJECTS)
	$(CC) -o $@ $(CCOPTIONS) $^ $(LIBDIR)/$(LSTR_SO) $(LIBEXTRA)
ifeq ($(MACOS),yes)
	mv rexx brexx
	install_name_tool -change liblstring.dylib @loader_path/../lstring/liblstring.dylib brexx
endif
endif

install:
	mkdir -p $(INSBIN)
	$(INSTALL) -m 755 $(REXX_EXE) $(INSBIN)

clean:
	${RM} *.o
	$(RM) *.obj
	$(RM) $(REXX_EXE)

targets: $(REXX_EXE)
