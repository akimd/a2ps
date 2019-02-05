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

man_MANS = a2ps.1 card.1 fixps.1 pdiff.1 psmandup.1 psset.1 texi2dvi4a2ps.1
man_aux = $(man_MANS:.1=.x)

irregular_men = a2ps.1 texi2dvi4a2ps.1
a2ps.1 : $(top_srcdir)/src/main.c
texi2dvi4a2ps.1: $(top_srcdir)/contrib/texi2dvi4a2ps

EXTRA_DIST = $(man_MANS) $(HELP2MAN) $(man_aux) \
  Makefile.maint GNUmakefile common.x
HELP2MAN = help2man
MAINTAINERCLEANFILES = $(man_MANS)

# For additional rules usually of interest only to the maintainer,
# see GNUmakefile and Makefile.maint.
