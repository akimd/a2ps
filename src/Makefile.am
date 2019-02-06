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

# I don't really understand why I have to put srcdir here, but
# it is needed for yacc and lex files (seems related to #line, but
# I really don't understand why)...
AM_CPPFLAGS = -I.. -I$(srcdir) -I$(top_srcdir)/liba2ps -I$(top_builddir)/intl \
	-I$(top_srcdir)/lib -DLOCALEDIR=\"$(localedir)\"

bin_PROGRAMS = a2ps

a2ps_SOURCES = main.c \
read.c sshread.c ssheet.c select.c generate.c 			\
delegate.c buffer.c versions.c ffaces.c			\
					\
parsessh.y lexssh.l lexps.l sheets-map.l

noinst_HEADERS = main.h \
read.h sshread.h ssheet.h select.h generate.h			\
delegate.h buffer.h versions.h ffaces.h			\
					\
yy2ssh.h lexps.h

BUILT_SOURCES = parsessh.c parsessh.h

YFLAGS = -dtv
LEX = @LEX@

a2ps_LDADD = $(top_builddir)/liba2ps/liba2ps.la $(LIBINTL) ../lib/libgnu.la -lm -L/opt/local/lib -lpaper # FIXME: hard coded.

EXTRA_DIST = $(BUILT_SOURCES)

clean-local:
	rm -rf *.dSYM
