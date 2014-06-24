#
# Simplistic Makefile for lzjb-stream. Mainly concerned with dist-archiving.
#

.PHONY:	doc help sdist

ALL:	help

VERSION	= $(shell grep LZJBSTREAM_VERSION include/lzjb-stream.h | cut -d'"' -f2)

SRC	= src/lzjb-stream.c
INC	= include/lzjb-stream*.h
DOC	= README.md
LICENSE	= LICENSE
DIST	= $(SRC) $(INC) $(DOC) $(LICENSE)

# ----------------------------------------------------------------------

doc:
	cd doc && make

help:
	@echo "Available make targets:"
	@echo "- doc    Builds the Doxygen HTML documentation, requires Doxygen."
	@echo "- sdist  Builds a trivial source distribution archive.\n"
	@echo "There is no general library target, copy the source files to your project instead!"

sdist:	Makefile $(DIST)
	tar czf lzjb-stream-$(VERSION).tar.gz  $(DIST)
