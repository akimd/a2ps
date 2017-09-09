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

# As a new `feature' :), a2ps uses now even more directories
psdir = $(pkgdatadir)/ps

prologues = bw.pro bold.pro fixed.pro gray.pro color.pro gray2.pro \
matrix.pro diff.pro diffcolor.pro ul.pro
procsets = base.ps ehandler.ps
headers = a2ps.hdr color.hdr

ps_DATA = $(prologues) $(procsets) $(headers)

EXTRA_DIST = $(ps_DATA)
