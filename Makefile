#
# Simplistic Makefile for lzjb-stream. Mainly concerned with dist-archiving.
#

.PHONY:	sdist

VERSION	= $(shell grep LZJBSTREAM_VERSION include/lzjb-stream.h | cut -d'"' -f2)

SRC	= src/lzjb-stream.c
INC	= include/lzjb-stream*.h
DOC	= README.md
LICENSE	= LICENSE
DIST	= $(SRC) $(INC) $(DOC) $(LICENSE)

# ----------------------------------------------------------------------

sdist:	Makefile $(DIST)
	tar czf lzjb-stream-$(VERSION).tar.gz  $(DIST)
