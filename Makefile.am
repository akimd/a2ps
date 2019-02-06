# Process this file with automake to create Makefile.in
#
# Copyright 1995-2017 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

## Make sure to drop doc at the end: it causes may problems, so
## let's do the maximum before it
SUBDIRS = build-aux etc ps ogonkify lib liba2ps src po \
	tests sheets encoding afm fonts ppd contrib doc man

EXTRA_DIST = TODO ANNOUNCE FAQ arch m4/gnulib-cache.m4

MAKE_AUTHORS = make-authors.pl

## I shouldn't need this, it looks like a bug in Automake
AMTAR = @AMTAR@
AMTARFLAGS = @AMTARFLAGS@
PERL = @PERL@
all-local: FAQ AUTHORS

prcs-diff:
	thisver=`echo "Version-$(VERSION)" | sed 's/\./-/g'`; \
	IFS="${IFS}:,;" ; \
	for prev in $(PREV); \
	do \
	  prevno=`echo "$$prev" | sed 's/^\./0./'`; \
	  prevver=Version-`echo $$prevno | sed 's/\./-/g'`; \
	  diff=$(PACKAGE)-$$prevno-$(VERSION).diff; \
	  prcs diff -r $$prevver -r $$thisver -N $(PACKAGE) -- -u > $$diff; \
	  gzip -c $$diff > $$diff.gz && \
	  bzip2 $$diff ;\
	done

FAQ: $(top_srcdir)/doc/a2ps.texi
	LC_ALL=C $(MAKEINFO) --no-header -I $(top_builddir)/doc $(top_srcdir)/doc/a2ps.texi | \
	sed -n -f $(top_srcdir)/build-aux/make-faq.sed > FAQ-tmp
	mv FAQ-tmp FAQ

AUTHORS: $(top_srcdir)/doc/translators.txt $(top_srcdir)/doc/$(MAKE_AUTHORS) $(top_srcdir)/sheets/sheets.map
	cd doc \
	&& srcdir=$(abs_top_srcdir)/doc $(PERL) $(abs_top_srcdir)/doc/$(MAKE_AUTHORS) > $@-t \
	&& chmod 644 $@-t
	mv doc/$@-t $@
