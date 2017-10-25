# libaspipo

A C library to simulate the Intelligent Agent Vacuum Cleaner and teach Artificial Intelligence

```
#!c
/* *********************************************************************** *
*    LIBASPIPO, an Artificial Intelligence Library to teach                *
*               agent programming.                                         *
*                                                                          *
*    Copyright (C) 2014-2017 by Ruben Carlo Benante                        *
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
* ************************************************************************ *
* 
*/

Welcome!

This is the libaspipo library!

```

## Compile

The following commands assume you are in the working directory:

```
cd $ROOT/libaspipo/src/lib
```

### Agent

```
make asp o=aspiraassopra
```

It will generate the libraries if needed

### Libraries

```
make libaspipo 
```

Generate all libraries for linux 32/64 bits and windows 32/64 bits into the current working directory `src/lib/`

### Copy

```
make copy
```

Copy the libraries to the release folder `$ROOT/libaspipo/bin` and the newest `libaspipo.h` from source folder to the template folder

### Tags

```
make tags 
```

Generate the file tag for the C source

### Clean

```
make clean o=aspiraassopra
```

Delete all:
    * libraries from the release and the current folder
    * all binaries from the example agent (aspiraassopra) for 32/64 bits for both linux and windows


