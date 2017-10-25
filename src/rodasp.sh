#/bin/bash

#/* *********************************************************************** 
#*    rodasp.sh, version 1.0                                                *
#*    Roda X iteracoes do exX.x aspipo e calcula a media de pontos          *
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
#*    Ruben Carlo Benante <rcb@upe.br>                                      *
#*    Webpage: http://www.beco.cc                                           *
#*    Phone: +55 (81) 3184-7555                                             *
#* ************************************************************************ *
#* 
#*/

# X = numero do exercicio ($1)
# M = quantidade de simulacoes ($2)

X=$1
M=$2
A=0 
I=0 
while (( I < $M )) 
    do 
       P=$(((./ex${X}.x) > saida_ex${X}_it${I}.txt) 2>&1) 
       (( A = A + P )) 
       echo It:${I} Pt:${P}
       sleep 1 
       (( I++ ))
    done
(( M = A / I )) 
echo Media: ${M}

