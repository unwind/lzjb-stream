#
# Makefile for lzjb-stream's test code.
#
# This file is in the public domain.
#

ALL	= test

ALL:	$(ALL)

.PHONY:	clean

# ----------------------------------------------------------------------

CFLAGS	= -std=c99 -I ../include -g -Wall

test:	test.c ../src/lzjb-stream.c

# ----------------------------------------------------------------------

clean:
	rm -f $(ALL)
