/* *********************************************************************** *
*    makefile for libaspipo v1.02                                          *
*                                                                          *
*    Copyright (C) 2017 by Ruben Carlo Benante                             *
*                                                                          *
*    This program is free software; you can redistribute it and/or modify  *
*    it under the terms of the GNU General Public License as published by  *
*    the Free Software Foundation; either version 2 of the License, or     *
*    (at your option) any later version.                                   *
*                                                                          *
*    This program is distributed in the hope that it will be useful,       *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*    GNU General Public License for more details.                          *
*                                                                          *
*    You should have received a copy of the GNU General Public License     *
*    along with this program; if not, write to the                         *
*    Free Software Foundation, Inc.,                                       *
*    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
*                                                                          *
*    To contact the author, please write to:                               *
*    Ruben Carlo Benante <rcb@beco.cc>                                     *
*    Webpage: http://www.beco.cc                                           *
*    Phone: +55 (81) 3184-7555                                             *
* ************************************************************************ *
* 
*/

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


.PHONY : libaspipo asp clean .aspux64 .aspux32 .aspw64 .aspw32 .clean-ux .clean-w

libaspipo : libaspipo-ux64.a libaspipo-ux32.a libaspipo-w64.lib libaspipo-w32.lib

asp : .asp-ux64 .asp-ux32 .asp-w64 .asp-w32

.asp-ux64 : $(o)-ux64.x

.asp-ux32 : $(o)-ux32.x

.asp-w64 : $(o)-w64.exe

.asp-w32 : $(o)-w32.exe

#--------------------- Linux: libaspipo.a
libaspipo-ux64.a : bin/libaspipo-ux64.o
	ar -cvq bin/libaspipo-ux64.a bin/libaspipo-ux64.o
libaspipo-ux32.a : bin/libaspipo-ux32.o
	ar -cvq bin/libaspipo-ux32.a bin/libaspipo-ux32.o

#--------------------- Windows: libaspipo.lib
libaspipo-w64.lib : libaspipo-w64.obj
	x86_64-w64-mingw32-ar -cvq bin/libaspipo-w64.lib bin/libaspipo-w64.obj
libaspipo-w32.lib : libaspipo-w32.obj
	i586-mingw32msvc-ar -cvq bin/libaspipo-w32.lib bin/libaspipo-w32.obj

#--------------------- Linux: libaspipo.o
libaspipo-ux64.o : libaspipo.c libaspipo.h
	gcc -m64 -march=x86-64 -mtune=generic -Wall -O0 -g -c src/lib/libaspipo.c -fPIC -o bin/libaspipo-ux64.o
libaspipo-ux32.o : libaspipo.c libaspipo.h
	gcc -m32 -march=i386 -mtune=generic -Wall -O0 -g -c src/lib/libaspipo.c -fPIC -o bin/libaspipo-ux32.o

#--------------------- Windows: libaspipo.obj
libaspipo-w64.obj : libaspipo.c libaspipo.h
	x86_64-w64-mingw32-gcc -Wall -O0 -g -c src/lib/libaspipo.c -o bin/libaspipo-w64.obj
libaspipo-w32.obj : libaspipo.c libaspipo.h
	i586-mingw32msvc-gcc -Wall -O0 -g -c src/lib/libaspipo.c -o bin/libaspipo-w32.obj

#--------------------- Linux: agente.x
$(o)-ux64.x : $(o).c libaspipo.h libaspipo-ux64.a
	gcc -m64 -march=x86-64 -mtune=generic -Wall -O0 -g $(o).c libaspipo-ux64.a -o $(o)-ux64.x 2>&1 | tee errors-ux64.err
$(o)-ux32.x : $(o).c libaspipo.h libaspipo-ux32.a
	gcc -m32 -march=i586 -mtune=generic -Wall -O0 -g $(o).c libaspipo-ux32.a -o $(o)-ux32.x 2>&1 | tee errors-ux32.err

#--------------------- Windows: agente.exe
$(o)-w64.exe : $(o).c libaspipo.h libaspipo-w64.lib
	x86_64-w64-mingw32-gcc -Wall -O0 -g $(o).c libaspipo-w64.lib -o $(o)-w64.exe 2>&1 | tee errors-w64.err
$(o)-w32.exe : $(o).c libaspipo.h libaspipo-w32.lib
	i586-mingw32msvc-gcc -Wall -O0 -g $(o).c libaspipo-w32.lib -o $(o)-w32.exe 2>&1 | tee errors-w32.err

clean : .clean-ux .clean-w
	rm -f errors*.err

.clean-ux : 
	rm -f libaspipo-*.o
	rm -f libaspipo-*.a
	rm -f $(o).x

.clean-w :
	rm -f libaspipo-*.obj
	rm -f libaspipo-*.lib
	rm -f $(o).exe

