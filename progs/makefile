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
SOURCES  = $(shell ls *.r)

targets:

install:
	mkdir -p $(INSSHA)
	for f in $(SOURCES);do $(INSTALL) $$f $(INSSHA); done

