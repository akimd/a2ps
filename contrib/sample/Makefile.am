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

AM_CPPFLAGS = -I. -I.. -I$(top_builddir) -I$(top_srcdir)/intl -I$(top_srcdir)/liba2ps \
	-DLOCALEDIR=\"$(localedir)\"

noinst_PROGRAMS = sample

sample_SOURCES = main.c

sample_LDADD = $(top_builddir)/liba2ps/liba2ps.la $(LIBINTL) -lm -L/opt/local/lib -lpaper # FIXME: hard coded.

clean-local:
	rm -rf *.dSYM
