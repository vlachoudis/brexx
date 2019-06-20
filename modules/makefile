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

########## Sources to Build #######
SOURCES = librxunix.c librxjson.c

# Check if mysql exists
ifeq ($(shell ./checkinclude.sh $(CPP) mysql/mysql.h $(CFLAGS)),0)
	SOURCES += librxmysql.c
endif

ifeq ($(shell ./checkinclude.sh $(CPP) sqlite3.h $(CFLAGS)),0)
	SOURCES += librxsqlite.c
endif

# Conio
#SOURCES += librxconio.c

# Windows message
#SOURCES += librxrsxwin.c

############# MACROS ##############
OBJECTS = $(SOURCES:.c=.o)
ifeq ($(MACOS), yes)
SOS     = $(SOURCES:.c=.dylib)
else
SOS     = $(SOURCES:.c=.so)
endif
############# RULES ###############

.c.o:
	$(CC) -c $(CCOPTIONS) $<

%.so: %.o
	$(CC) $(CCOPTIONS) -shared -Wl,-soname,$@ -o $@ $< -L../lstring -llstring $(LDEXTRA)

%.dylib: %.o
	$(CC) $(CCOPTIONS) -o $@ -dynamiclib -Wl,-U $< -L../lstring -llstring $(LDEXTRA) -rpath


############# TARGETS #############

default:
	@echo "Specify a target configuration"

librxsqlite.so: librxsqlite.o
	$(CC) $(CCOPTIONS) -shared -Wl,-soname,$@ -o $@ $< -lsqlite3 $(LDEXTRA)

librxmysql.so: librxmysql.o
	$(CC) $(CCOPTIONS) -shared -Wl,-soname,$@ -o $@ $< -L$(USRLIB)/mysql -lmysqlclient $(LDEXTRA)

targets: $(SOS)

install:
	mkdir -p $(INSLIB)
	for f in $(SOS);do $(INSTALL) $$f $(INSLIB); done

clean:
	$(RM) *.o
	$(RM) *.a
	$(RM) *.so

