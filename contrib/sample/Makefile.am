# Process this file with automake to create Makefile.in
#
# Copyright 1988-2017 Free Software Foundation, Inc.
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

## Since this package is written in ansi, be ready to un-ansify
AUTOMAKE_OPTIONS = $(top_builddir)/lib/ansi2knr
localedir = $(datadir)/locale

noinst_PROGRAMS = sample

sample_SOURCES = main.c

INCLUDES = -I. -I.. -I$(top_builddir) -I$(top_srcdir)/intl -I$(top_srcdir)/lib
sample_LDADD = $(top_builddir)/lib/liba2ps.la @LIBINTL@ -lm
DEFS = @DEFS@ -DLOCALEDIR=\"$(localedir)\"
