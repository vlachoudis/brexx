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
SOURCES  = $(shell ls *.h)

targets:

install:
	mkdir -p $(INSINC)
	for f in $(SOURCES);do $(INSTALL) -m 644 $$f $(INSINC); done

