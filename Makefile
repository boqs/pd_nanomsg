# Makefile for 
PDINCLUDEDIR=/usr/local/include/pd

lib.name = nanomsg

class.sources = nanomsg.c
ldflags = -lnanomsg -fno-common -g

#datafiles = myclass1-help.pd myclass2-help.pd README.txt LICENSE.txt

include ../pd-lib-builder/Makefile.pdlibbuilder

tags:
	find . ~/git_checkouts/pure-data -name '*.[ch]' | etags -

test:
	cp *.pd_linux ~/pd-externals; pd nanomsg-help.pd
