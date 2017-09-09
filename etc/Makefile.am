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

ogonkifydir=$(datadir)/ogonkify
libpath = $(pkgdatadir)/sheets:$(pkgdatadir)/ps:$(pkgdatadir)/encoding:$(pkgdatadir)/afm:$(ogonkifydir)/afm:$(pkgdatadir)/ppd:$(pkgdatadir)/fonts:$(ogonkifydir)/fonts:$(pkgdatadir)

pkgdata_DATA = README

dsysconf = $(DESTDIR)$(sysconfdir)

EXTRA_DIST = README.in

all-local: a2ps.cfg

install-data-local: a2ps.cfg
	$(mkinstalldirs) $(dsysconf)
	@if test -r $(dsysconf)/$(PACKAGE).cfg; then \
	  if cmp $(dsysconf)/$(PACKAGE).cfg $(top_builddir)/etc/$(PACKAGE).cfg 2> /dev/null ; then \
	     echo "$(dsysconf)/$(PACKAGE).cfg has not changed"; \
	  else \
	     echo;	\
	     echo "*******************";	\
	     echo "* Warning: $(dsysconf)/$(PACKAGE).cfg exist."; \
	     echo "* It has been moved as $(dsysconf)/$(PACKAGE).cfg.old"; \
	     echo "*******************";	\
	     echo; \
	     mv $(dsysconf)/$(PACKAGE).cfg $(dsysconf)/$(PACKAGE).cfg.old; \
	     $(INSTALL_DATA) $(top_builddir)/etc/$(PACKAGE).cfg $(dsysconf)/$(PACKAGE).cfg ; \
	  fi \
	else \
	  $(INSTALL_DATA) $(top_builddir)/etc/$(PACKAGE).cfg $(dsysconf)/$(PACKAGE).cfg; \
	fi
	@test -r $(dsysconf)/a2ps-site.cfg || \
	  $(INSTALL_DATA) $(top_builddir)/etc/a2ps-site.cfg $(dsysconf)/a2ps-site.cfg ;

uninstall-local:
	rm -f $(dsysconf)/$(PACKAGE).cfg $(dsysconf)/$(PACKAGE)-site.cfg

DISTCLEANFILES = a2ps.cfg README

# Building the correct a2ps.cfg
a2ps.cfg: a2ps_cfg Makefile
	sed "s!@libpath@!$(libpath)!" a2ps_cfg > a2ps.cfg

# Building a time stamp to know the version.
README: README.in Makefile
	sed "s!@date@!`date`!g;s!@version@!$(VERSION)!g"	\
	$(srcdir)/README.in > README
