/* ASPIPO - aspirador de po inteligente                                   */
/* Autor: Ruben Carlo Benante                                             */
/* email: rcb@beco.cc                                                     */
/* Copyright 2011-2017. Licenca GNU/GPL: mantenha sempre o nome do autor  */
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
    return 0; /* ou pontos() */
}

/* ---------------------------------------------------------------------- */
/* vi: set ai et ts=4 sw=4 tw=0 wm=0 fo=croql : C config for Vim modeline */
/* Template by Dr. Beco <rcb at beco dot cc> Version 20160612.142044      */

