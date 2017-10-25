/* ASPIPO - aspirador de po inteligente                                   */
/* Autor: Ruben Carlo Benante                                             */
/* email: rcb@beco.cc                                                     */
/* Copyright 2011. Licenca GNU/GPL: mantenha sempre o nome do autor       */
/*                                                                        */
/* Template para exemplificar o agente minimo que aspira/assopra          */

#include <stdio.h>
#include "libaspipo.h"

int main(void)
{

    inicializar_ambiente(OBS0/*obs*/, DETERMINISTICO/*det*/, 2/*qs*/, APRIORI0/*priori*/, DESC0/*desc*/, FOLGA0, NULL/*md*/, NULL/*prob*/);

    while(qtd_acoes()<=MAXACOES)
    {
        aspirar();
        assoprar();
    }

    finalizar_ambiente();
    pontos(); 
	return 0;
}

