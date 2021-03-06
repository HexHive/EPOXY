# Common makefile.am fragments for building a BEEBS benchmark
#
# Copyright (C) 2013 Embecosm Limited and University of Bristol
#
# This file is part of the Bristol/Embecosm Embedded Benchmark Suite.
#
# Contributor: James Pallister <james.pallister@bristol.ac.uk>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

platform_src = $(top_srcdir)/config/$(ARCH)/boards/@BOARD@/boardsupport.c \
               $(top_srcdir)/include/support.h \
               $(top_srcdir)/src/main.c

ifdef CHIPSUPPORT_C
platform_src += $(top_srcdir)/config/@ARCH@/chips/@CHIP@/chipsupport.c
endif

# If there is a benchmark calibration file, this specifies the shifting
# of the repeat factor. If grep doesn't find the program, then we specify
# 0 as the default. This is the default if a calibration file is not found
ifdef CALIBRATION
calib = (egrep "^\b$(bin_PROGRAMS)\b" \
	        $(top_srcdir)/config/@ARCH@/boards/@BOARD@/calibration \
		|| echo "0 0") | cut -d " " -f 2
else
calib = echo 0
endif

AM_CFLAGS=$(WERROR_CFLAGS) $(WARN_CFLAGS) \
		  -I $(top_srcdir)/include \
		  -I $(top_srcdir)/config/@ARCH@/boards/@BOARD@/ \
		  -I $(top_srcdir)/config/@ARCH@/chips/@CHIP@/ \
		  "-DCALIB_SCALE=$(shell $(calib))" \
		  @CONFIG_CFLAGS@

AM_LDFLAGS= @CONFIG_LDFLAGS@
