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
	abbrev.c \
	abs.c \
	add.c \
	atan2.c \
	b2x.c \
	bitand.c \
	bitor.c \
	bitxor.c \
	bool.c \
	c2d.c \
	c2x.c \
	center.c \
	changest.c \
	countstr.c \
	charin.c \
	charout.c \
	chars.c \
	compare.c \
	copies.c \
	d2c.c \
	d2x.c \
	datatype.c \
	date.c \
	dec.c \
	delstr.c \
	delword.c \
	div.c \
	equal.c \
	errortxt.c \
	expose.c \
	format.c \
	hashvalu.c \
	inc.c \
	index.c \
	insert.c \
	intdiv.c \
	justify.c \
	lastpos.c \
	linein.c \
	lineout.c \
	lines.c \
	lmath.c \
	lower.c \
	lstring.c \
	mod.c \
	mult.c \
	neg.c \
	overlay.c \
	pow.c \
	print.c \
	read.c \
	reverse.c \
	right.c \
	sign.c \
	space.c \
	stderr.c \
	strip.c \
	soundex.c \
	sub.c \
	substr.c \
	subword.c \
	time.c \
	translat.c \
	trunc.c \
	upper.c \
	verify.c \
	word.c \
	wordidx.c \
	wordlen.c \
	wordpos.c \
	words.c \
	write.c \
	x2b.c \
	x2c.c \
	x2d.c \
	xrange.c

OBJECTS = $(SOURCES:.c=.o)

############# RULES ###############

.c.o:
	$(CC) -c $(CCOPTIONS) $<


############# TARGETS #############

default:
	@echo "Specify a target configuration"


$(LIBDIR)/$(LSTR_LIB):	$(OBJECTS)
	$(RM) $@
	$(MAKELIB) $@ $^


ifeq ($(STATIC),no)
$(LIBDIR)/$(LSTR_SO):	$(OBJECTS)
ifeq ($(MACOS), yes)
	$(CC) -o $(LSTR_SO) -dynamiclib *.o $(LDEXTRA) $(LIBEXTRA)
else
	$(CC) $(CCOPTIONS) -shared -Wl,-soname,$(LSTR_SO) -o $@ $^ $(LDEXTRA)
endif

targets: $(LIBDIR)/$(LSTR_LIB) $(LIBDIR)/$(LSTR_SO)

else
targets: $(LIBDIR)/$(LSTR_LIB)
endif

install:
	mkdir -p $(INSLIB)
	$(INSTALL) $(LIBDIR)/$(LSTR_LIB) $(INSLIB)
	$(INSTALL) $(LIBDIR)/$(LSTR_SO)  $(INSLIB)

clean:
	$(RM) *.o
	$(RM) *.a
	$(RM) *.so
	$(RM) *.sym


