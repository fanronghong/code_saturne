## Process this file with automake to produce Makefile.in

#-------------------------------------------------------------------------------

# This file is part of Code_Saturne, a general-purpose CFD tool.
#
# Copyright (C) 1998-2020 EDF S.A.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
# Street, Fifth Floor, Boston, MA 02110-1301, USA.

#-------------------------------------------------------------------------------

SUBDIRS = Base Pages studymanager_gui trackcvg

EXTRA_DIST = Base/icons/code_saturne.ico

# Install data

imagesdir = ${pkgdatadir}/images

images_DATA = \
Base/icons/splashscreen.png \
Base/icons/22x22/logo_salome_cfd.png

install-exec-local:
	@case ${host_os} in \
	  *mingw* | *cygwin*) \
	    $(INSTALL_PROGRAM) $(srcdir)/Base/icons/code_saturne.ico $(DESTDIR)$(bindir); \
	    ;; \
	esac

uninstall-local:
	@case ${host_os} in \
	  *mingw* | *cygwin*) \
	    rm $(DESTDIR)$(bindir)/code_saturne.ico; \
	    rmdir $(DESTDIR)$(bindir); \
	    ;; \
	esac

