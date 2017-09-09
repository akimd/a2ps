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

# As a new `feature' :), a2ps uses now even more directories
encodingsdir = $(pkgdatadir)/encoding

# I don't want to hurt anybody here, this is under the pressure
# of many users.  Don't pay attention to the names.
major_encodings = ascii.edf iso1.edf
minor_encodings = \
ms-cp1250.edf hp.edf mac.edf ibm-cp437.edf ibm-cp850.edf iso2.edf	\
iso3.edf iso4.edf iso5.edf iso7.edf iso9.edf iso10.edf iso13.edf	\
iso15.edf koi8.edf ms-cp1251.edf

if EXTENSIONS
encodings_DATA = encoding.map $(major_encodings) $(minor_encodings)
else
encodings_DATA = encoding.map $(major_encodings)
endif

EXTRA_DIST = README encoding.map $(major_encodings) $(minor_encodings)
