#/* *********************************************************************** *
#*    makefile for libaspipo v1.02                                          *
#*                                                                          *
#*    Copyright (C) 2017 by Ruben Carlo Benante                             *
#*                                                                          *
#*    This program is free software; you can redistribute it and/or modify  *
#*    it under the terms of the GNU General Public License as published by  *
#*    the Free Software Foundation; either version 2 of the License, or     *
#*    (at your option) any later version.                                   *
#*                                                                          *
#*    This program is distributed in the hope that it will be useful,       *
#*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*    GNU General Public License for more details.                          *
#*                                                                          *
#*    You should have received a copy of the GNU General Public License     *
#*    along with this program; if not, write to the                         *
#*    Free Software Foundation, Inc.,                                       *
#*    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
#*                                                                          *
#*    To contact the author, please write to:                               *
#*    Ruben Carlo Benante <rcb@beco.cc>                                     *
#*    Webpage: http://www.beco.cc                                           *
#*    Phone: +55 (81) 3184-7555                                             *
#* ************************************************************************ */

# libaspipo
#
#
#########################
# Gerar biblioteca
#------------------------
# no Linux, para Linux
# gcc -Wall -O0 -g -c libaspipo.c -o libaspipo-ux64.o -fPIC
#
#------------------------
# no Linux, para Windows
# i586-mingw32msvc-gcc -Wall -O0 -g -c libaspipo.c -o libaspipo-w32.o
#
#########################
# Gerar agente
#------------------------
# no Linux, para Linux
# gcc -Wall -O0 -g -c agente.c libaspipo-ux64.o -o agente.x
# ou tambem
# -march=i586 (sem MMX) ou -march=pentium-mmx
#
#------------------------
# no Linux, para Windows
# i586-mingw32msvc-gcc -Wall -O0 -g -c agente.c libaspipo-w32.o -o agente.exe
# ou também
# i686-w64-mingw32-gcc 
#
#########################

.PHONY : libaspipo asp clean copy .aspux64 .aspux32 .aspw64 .aspw32 .clean-ux .clean-w
# disable builtin rules with MAKEFLAGS and .SUFFIXES
MAKEFLAGS += --no-builtin-rules
#.SUFFIXES:
.PRECIOUS: %.o 
SHELL=/bin/bash -o pipefail

#--------------------- libraries' dir
# source dir: ROOT/libaspipo/src/lib/
SRCDIR := ./
# release binaries: ROOT/libaspipo/bin/
BINDIR := ../../bin/
# VERSION dir: ROOT/libaspipo/
VERDIR := ../../
# TEMPLATES dir: 
TPTDIR := ../templates/
#--------------------- CONFIG VARIABLES 
FORTIFY ?= 1
DEBUG ?= 1
MAJOR ?= 1
MINOR ?= 2
OBJ ?=
SRC ?=
CCCOLOR ?= always
D ?= D_
BUILD = $(shell date +"%g%m%d.%H%M%S")
DEFSYM = $(subst .,_,$(BUILD))
VERSION = "\"$(MAJOR).$(MINOR).$(BUILD)\""
CC = gcc
CCW64 = x86_64-w64-mingw32-gcc
CCW32 = i586-mingw32msvc-gcc
#--------------------- CFLAGS
#CFLAGS = -Wall -g -O0 -fdiagnostics-color=$(CCCOLOR) -Wextra -std=gnu99 -pg -fprofile-arcs -ansi -Wpedantic -Werror -pedantic-errors
CFLAGS = -Wall -Wextra -std=gnu99 -fdiagnostics-color=$(CCCOLOR)
ifeq "$(DEBUG)" "0" 
CFLAGS += -Ofast
else ifeq "$(DEBUG)" "1"
CFLAGS += -g -O0
else
CFLAGS += -g -O0 -pg -fprofile-arcs -ansi -Wpedantic
endif
#--------------------- CPPFLAGS
#CPPFLAGS = -DVERSION=$(VERSION) -DBUILD="\"$(BUILD)\"" -DDEBUG=$(DEBUG) -D$(D) -D_FORTIFY_SOURCE=$(FORTIFY) -D_XOPEN_SOURCE=700
CPPFLAGS = -DVERSION=$(VERSION) -DBUILD="\"$(BUILD)\"" -DDEBUG=$(DEBUG) -D$(D) -D_FORTIFY_SOURCE=$(FORTIFY)
ifeq "$(DEBUG)" "2"
CPPFLAGS += -D_XOPEN_SOURCE=700
endif
#--------------------- LDLIBS LD* and CCSHARED
#LDLIBS = -Wl,--defsym,BUILD_$(DEFSYM)=0 -lm -lpthread -lncurses -lcurl -mtune=generic -c -fPIC -mXX -march=XX
LDLIBS = -Wl,--defsym,BUILD_$(DEFSYM)=0 -lm -lpthread -lncurses -lcurl -mtune=generic
LDOBJ = -c -fPIC
LDUX64 = -m64 -march=x86-64
LDUX32 = -m32 -march=i386
CCSHARED = -shared
#--------------------- agent
o ?= aspipo

#--------------------- goals

libaspipo : $(SRCDIR)libaspipo-ux64.a $(SRCDIR)libaspipo-ux32.a $(SRCDIR)libaspipo-w64.lib $(SRCDIR)libaspipo-w32.lib

asp : .asp-ux64 .asp-ux32 .asp-w64 .asp-w32

.asp-ux64 : $(o)-ux64.x
.asp-ux32 : $(o)-ux32.x
.asp-w64 : $(o)-w64.exe
.asp-w32 : $(o)-w32.exe

#--------------------- Linux: libaspipo.a
$(SRCDIR)libaspipo-ux64.a : $(SRCDIR)libaspipo-ux64.o
	ar -cvq $(SRCDIR)libaspipo-ux64.a $(SRCDIR)libaspipo-ux64.o
$(SRCDIR)libaspipo-ux32.a : $(SRCDIR)libaspipo-ux32.o
	ar -cvq $(SRCDIR)libaspipo-ux32.a $(SRCDIR)libaspipo-ux32.o

#--------------------- Windows: libaspipo.lib
$(SRCDIR)libaspipo-w64.lib : $(SRCDIR)libaspipo-w64.obj
	x86_64-w64-mingw32-ar -cvq $(SRCDIR)libaspipo-w64.lib $(SRCDIR)libaspipo-w64.obj
$(SRCDIR)libaspipo-w32.lib : $(SRCDIR)libaspipo-w32.obj
	i586-mingw32msvc-ar -cvq $(SRCDIR)libaspipo-w32.lib $(SRCDIR)libaspipo-w32.obj

#--------------------- Linux: libaspipo.o
#%.x : %.c $(OBJ) $(SRC)
#	-$(CC) $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $^ -o $@ 2>&1 | tee errors.err

$(SRCDIR)libaspipo-ux64.o : $(SRCDIR)libaspipo.c $(SRCDIR)libaspipo.h
	#gcc -m64 -march=x86-64 -mtune=generic -Wall -O0 -g -c $(SRCDIR)libaspipo.c -fPIC -o $(SRCDIR)libaspipo-ux64.o
	-$(CC) $(LDUX64) $(LDLIBS) $(LDOBJ) $(CFLAGS) $(CPPFLAGS) $(SRCDIR)libaspipo.c -o $(SRCDIR)libaspipo-ux64.o 2>&1 | tee errors-ux64.err
ifeq "$(CCCOLOR)" "always"
	@sed -i -r "s/\x1B\[(([0-9]+)(;[0-9]+)*)?[m,K,H,f,J]//g" errors-ux64.err
endif
	-@[ ! -s errors-ux64.err ] && echo $@ version $(VERSION) > VERSION
$(SRCDIR)libaspipo-ux32.o : $(SRCDIR)libaspipo.c $(SRCDIR)libaspipo.h
	#gcc -m32 -march=i386 -mtune=generic -Wall -O0 -g -c $(SRCDIR)libaspipo.c -fPIC -o $(SRCDIR)libaspipo-ux32.o
	-$(CC) $(LDUX32) $(LDLIBS) $(LDOBJ) $(CFLAGS) $(CPPFLAGS) $(SRCDIR)libaspipo.c -o $(SRCDIR)libaspipo-ux32.o 2>&1 | tee errors-ux32.err
ifeq "$(CCCOLOR)" "always"
	@sed -i -r "s/\x1B\[(([0-9]+)(;[0-9]+)*)?[m,K,H,f,J]//g" errors-ux32.err
endif
	-@[ ! -s errors-ux32.err ] && echo $@ version $(VERSION) > VERSION

#--------------------- Windows: libaspipo.obj
$(SRCDIR)libaspipo-w64.obj : $(SRCDIR)libaspipo.c $(SRCDIR)libaspipo.h
	#x86_64-w64-mingw32-gcc -Wall -O0 -g -c $(SRCDIR)libaspipo.c -o $(SRCDIR)libaspipo-w64.obj
	-$(CCW64) $(LDOBJ) $(CFLAGS) $(CPPFLAGS) $(SRCDIR)libaspipo.c -o $(SRCDIR)libaspipo-w64.obj 2>&1 | tee errors-w64.err
ifeq "$(CCCOLOR)" "always"
	@sed -i -r "s/\x1B\[(([0-9]+)(;[0-9]+)*)?[m,K,H,f,J]//g" errors-w64.err
endif
	-@[ ! -s errors-w64.err ] && echo $@ version $(VERSION) > VERSION	
$(SRCDIR)libaspipo-w32.obj : $(SRCDIR)libaspipo.c $(SRCDIR)libaspipo.h
	#i586-mingw32msvc-gcc -Wall -O0 -g -c $(SRCDIR)libaspipo.c -o $(SRCDIR)libaspipo-w32.obj
	-$(CCW32) $(LDOBJ) $(CFLAGS) $(CPPFLAGS) $(SRCDIR)libaspipo.c -o $(SRCDIR)libaspipo-w32.obj 2>&1 | tee errors-w32.err
ifeq "$(CCCOLOR)" "always"
	@sed -i -r "s/\x1B\[(([0-9]+)(;[0-9]+)*)?[m,K,H,f,J]//g" errors-w32.err
endif
	-@[ ! -s errors-w32.err ] && echo $@ version $(VERSION) > VERSION

#--------------------- Linux: agente.x
$(o)-ux64.x : $(o).c $(SRCDIR)libaspipo.h $(SRCDIR)libaspipo-ux64.a
	#gcc -m64 -march=x86-64 -mtune=generic -Wall -O0 -g $(o).c $(SRCDIR)libaspipo-ux64.a -o $(o)-ux64.x 2>&1 | tee errors-ux64.err
	-$(CC) $(LDUX64) $(LDLIBS) $(CFLAGS) $(CPPFLAGS) $(TPTDIR)$(o).c $(SRCDIR)libaspipo-ux64.a -o $(SRCDIR)$(o)-ux64.x 2>&1 | tee errors-ux64.err
$(o)-ux32.x : $(o).c $(SRCDIR)libaspipo.h $(SRCDIR)libaspipo-ux32.a
	#gcc -m32 -march=i586 -mtune=generic -Wall -O0 -g $(o).c $(SRCDIR)libaspipo-ux32.a -o $(o)-ux32.x 2>&1 | tee errors-ux32.err
	-$(CC) $(LDUX32) $(LDLIBS) $(CFLAGS) $(CPPFLAGS) $(TPTDIR)$(o).c $(SRCDIR).libaspipo-ux32.a -o $(SRCDIR)$(o)-ux32.x 2>&1 | tee errors-ux32.err

#--------------------- Windows: agente.exe
$(o)-w64.exe : $(o).c $(SRCDIR)libaspipo.h $(SRCDIR)libaspipo-w64.lib
	#x86_64-w64-mingw32-gcc -Wall -O0 -g $(o).c $(SRCDIR)libaspipo-w64.lib -o $(o)-w64.exe 2>&1 | tee errors-w64.err
	-$(CCW64) $(LDLIBS) $(CFLAGS) $(CPPFLAGS) $(TPTDIR)$(o).c $(SRCDIR)libaspipo-w64.lib -o $(SRCDIR)$(o)-w64.exe 2>&1 | tee errors-w64.err

$(o)-w32.exe : $(o).c $(SRCDIR)libaspipo.h $(SRCDIR)libaspipo-w32.lib
	#i586-mingw32msvc-gcc -Wall -O0 -g $(o).c $(SRCDIR)libaspipo-w32.lib -o $(o)-w32.exe 2>&1 | tee errors-w32.err
	-$(CCW32) $(LDLIBS) $(CFLAGS) $(CPPFLAGS) $(TPTDIR)$(o).c $(SRCDIR)libaspipo-w64.lib -o $(SRCDIR)$(o)-w64.exe 2>&1 | tee errors-w64.err

#--------------------- Clean and copy
clean : .clean-ux .clean-w
	rm -f errors*.err

.clean-ux : 
	rm -f $(SRCDIR)libaspipo-*.o
	rm -f $(SRCDIR)libaspipo-*.a
	rm -f $(SRCDIR)$(o).x

.clean-w :
	rm -f $(SRCDIR)libaspipo-*.obj
	rm -f $(SRCDIR)libaspipo-*.lib
	rm -f $(SRCDIR)$(o).exe

copy :
	cp $(SRCDIR)libaspipo*.a $(BINDIR)
	cp $(SRCDIR)libaspipo*.lib $(BINDIR)
	cp $(SRCDIR)libaspipo*.o $(BINDIR)
	cp $(SRCDIR)libaspipo*.obj $(BINDIR)

#--------------------- tag file
tags :
	ctags -R
	ctags -R -x | less -F

#* ------------------------------------------------------------------- *
#* makefile config for Vim modeline                                    *
#* vi: set ai noet ts=4 sw=4 tw=0 wm=0 fo=croqlt :                     *
#* Template by Dr. Beco <rcb at beco dot cc> Version 20170506.191339   *

