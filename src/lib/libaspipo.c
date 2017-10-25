/* ASPIPO - aspirador de po inteligente                                   */
/* Autor: Ruben Carlo Benante                                             */
/* email: dr.beco@gmail.com                                               */
/* Copyright 2011. Licenca a definir mantenha sempre o nome do autor      */
/* Codico fechado                                                         */
/*                                                                        */
/* compile com c:\dev-cpp\bin\gcc.exe libaspipo.c -o libaspipo.o -c       */

/* Ambiente Aspipo:

Acoes do Aspipo:
      esquerda() - perde 2 ponto de energia.
      direita() - perde 2 ponto de energia.
      aspira() - perde 45 pontos de energia. Ganha 10 pontos se realmente limpar algo.
      assopra() -  perde 95 pontos de energia.
      passar_vez()- nao perde pontos, mas gasta uma acao.

Sensores do Aspipo:
      ler_sujeira() - perde 1 ponto de energia.
      ler_posicao() - perde 1 ponto de energia.
      terremoto - variavel inteira que sente terremotos.

Reacoes do Ambiente:
      Barra o Aspipo, se ele trombar na parede ao tentar andar para fora do limite.
      Limpa o ambiente, se o Aspipo aspirar.
      Suja o ambiente, se o Aspipo assoprar.
      Diz a condicao de sujeira, se o aspipo ler_sujeira(). 0=limpo, 1=sujo.
      Diz a posicao do Aspipo, se ele ler_posicao. Resposta eh o numero da sala.

Acoes independentes do Ambiente:
      Aleatoriamente suja uma ou mais salas
      Terremoto! Muda o ASPIPO de sala e suja todas salas. O aspipo sente na variavel externa terremoto.
      Calcula os pontos!  pontos()


Bugs:
  Como se comporta se ligar DETTERREMOTOBASICO e DETTERREMOTOTOTAL? Fica valendo o total

*/

#include "libaspipo.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define LIMPA 0
#define SUJA 1
#define DEBUG 1
#define MAXACOES 1000

static int terremoto;
static int inicializou = 0;
static int temnosaco; /*zera quando assopra*/
static int acoes=0, acao_andar, acao_ler, acao_aspirar, acao_assoprar, acao_passarvez, qtd_bonus, qtd_limpou, qtd_descarregou, qtd_tempolimpo; /*contadores*/
static int pos_aspipo;
static int sala[QTDSALA]; /* sala[A]=sala[B]=suja */
static int v_andar, v_ler, v_aspirar, v_assoprar, v_passarvez, v_limpar, v_desc, v_bonus, v_tempolimpo; /*pesos*/
static int qtd_sala, menor_sala, maior_sala, descarga_sala, capasaco; /*configuracoes*/
static float p_sujar, p_terremoto, p_succao, p_movimento, p_sensorial; /*configuracoes*/
static int flag_bonus;
observavel obs;
desempenho md;
probabilidade pr;
static int NIVELDET, NIVELOBS, APRIORI, FUNCDES, FOLGA;

/*
funcao void inicializar_apriori(void)
Tipo: interna
entrada: nada
saida: coloca na struct externa observavel valores de conhecimento a priori, conforme parametro APRIORI
*/
void inicializar_apriori(void)
{
  int i;
  /* zerar todo conhecimento do mundo */
  obs.mpos=-1;       /*posicao do aspipo*/
  obs.capasaco=-1;   /*capacidade do saco*/
  obs.terremoto=0;  /*flag de terremoto*/
  for(i=menor_sala; i<maior_sala; i++)
    obs.sala[QTDSALA]=-1; /*flag de condicao de limpeza da sala*/
  obs.v_andar=0;         /*peso de andar*/
  obs.v_ler=0;           /*peso de ler*/
  obs.v_aspirar=0;       /*peso de aspirar*/
  obs.v_assoprar=0;      /*peso de assoprar*/
  obs.v_passarvez=0;     /*peso de passar a vez*/
  obs.v_limpar=0;        /*peso de aspirar com sucesso*/
  obs.v_desc=0;          /*peso de assoprar com sucesso*/
  obs.v_bonus=0;         /*peso do bonus de todas salas limpas*/
  obs.v_tempolimpo=0;    /*peso do bonus de cada sala limpa por unidade de tempo*/
  obs.qtd_sala=-1;       /*quantidade de salas no ambiente*/
  obs.menor_sala=-1;     /*numero da menor sala*/
  obs.maior_sala=-1;     /*numero da maior sala*/
  obs.descarga_sala=-1;  /*numero da sala de descarga*/
  obs.p_sujar=0.0;       /*probabilidade de sujar sala*/
  obs.p_terremoto=0.0;   /*probabilidade de terremoto*/
  obs.p_succao=0.0;      /*probabilidade de erro em assoprar/aspirar*/
  obs.p_movimento=0.0;   /*probabilidade de erro em andar*/
  obs.p_sensorial=0.0;   /*probabilidade de erro em leitura de sensores*/

  /*atualizar o conhecimento de acordo com parametro*/
 /*conhecimento inicial do mundo:
 6-nada, 5-qtd_sala, 4-menor_sala, maior_sala, 3-mpos, 2-sujeiras, 1-capasaco, 0-md, proba, sala desc*/
  if(APRIORI<=APRIORI5) /*qtd_sala*/
    obs.qtd_sala=qtd_sala;
  if(APRIORI<=APRIORI4) /*sala menor e sala maior*/
  {
    obs.menor_sala=menor_sala;
    obs.maior_sala=maior_sala-1; /*Bug: confuso. No agente, sala valida. Aqui, limite superior 'exclusive' */
  }
  if(APRIORI<=APRIORI3) /*mpos*/
    obs.mpos=pos_aspipo;
  if(APRIORI<=APRIORI2) /*sujeira compartilhada*/
    for(i=menor_sala; i<maior_sala; i++)
      obs.sala[i]=sala[i];
  if(APRIORI<=APRIORI1) /*capacidade do saco*/
    obs.capasaco=capasaco;
  if(APRIORI==APRIORI0) /*sala de descarga, pesos e probabilidades*/
  {
    obs.v_andar=v_andar;
    obs.v_ler=v_ler;
    obs.v_aspirar=v_aspirar;
    obs.v_assoprar=v_assoprar;
    obs.v_passarvez=v_passarvez;
    obs.v_limpar=v_limpar;
    obs.v_desc=v_desc;
    obs.v_bonus=v_bonus;
    obs.v_tempolimpo=v_tempolimpo;
    obs.p_sujar=p_sujar;
    obs.p_terremoto=p_terremoto;
    obs.p_succao=p_succao;
    obs.p_movimento=p_movimento;
    obs.p_sensorial=p_sensorial;
    obs.descarga_sala=descarga_sala;
  }
}

/*
funcao void atualiza_observavel(void)
Tipo: interna
entrada: nada
saida: atualiza a struct externa observavel, conforme parametro NIVELOBS
*/
void atualiza_observavel(void)
{
  int i;
  /*atualizar o conhecimento de acordo com parametro*/
  if(NIVELOBS<=3) /*terremoto*/
    obs.terremoto=terremoto;
  if(NIVELOBS<=1) /*nivel 2 apenas libera ler_chamado()*/
  {

    obs.mpos=pos_aspipo;
    for(i=menor_sala; i<maior_sala; i++)
      obs.sala[i]=sala[i];
    obs.capasaco=capasaco;
    obs.qtd_sala=qtd_sala;
    obs.menor_sala=menor_sala;
    obs.maior_sala=maior_sala-1; /*Bug: confuso. No agente, sala valida. Aqui, limite superior 'exclusive' */
    obs.descarga_sala=descarga_sala;
  }

  if(NIVELOBS==0)
  {
    obs.v_andar=v_andar;
    obs.v_ler=v_ler;
    obs.v_aspirar=v_aspirar;
    obs.v_assoprar=v_assoprar;
    obs.v_passarvez=v_passarvez;
    obs.v_limpar=v_limpar;
    obs.v_desc=v_desc;
    obs.v_bonus=v_bonus;
    obs.v_tempolimpo=v_tempolimpo;
    obs.p_sujar=p_sujar;
    obs.p_terremoto=p_terremoto;
    obs.p_succao=p_succao;
    obs.p_movimento=p_movimento;
    obs.p_sensorial=p_sensorial;
  }
}

/*
funcao int naoiniciou(void)
Tipo: interna
entrada: nada
saida: 0 se iniciou, -1 se nao iniciou
testa se ambiente foi inicializado para comecar simulacao nova
*/
int naoiniciou(void)
{
  if(inicializou==0)
  {
    printf("Ambiente nao esta pronto para acoes.\n");
    printf("----------- agente (%d): \n", acoes);
    return -1;
  }
  return 0;
}

/*
| 63| 64| 65| 66|
| A |   |   |   |
|___|***|...|***|
*/
void imprimir_ambiente(void)
{
  int s, l;
//  if(naoiniciou()) return;
  if(inicializou==0)
    return;
  for(l=0; l<3; l++)
  {
    if(l==0)
    {
      for(s=menor_sala; s<maior_sala; s++)
        printf("|%3d", s);
      printf("|\n");
    }
    if(l==1)
    {
      for(s=menor_sala; s<maior_sala; s++)
        if(pos_aspipo==s)
          printf("| A " );
        else
          printf("|   " );
      printf("|\n");
    }
    if(l==2)
    {
      for(s=menor_sala; s<maior_sala; s++)
        if(s==descarga_sala)
          printf("|..." );
        else
          if(sala[s]==1)
            printf("|***" );
          else
            printf("|___" );
      printf("|\n");
    }
  }
}


/*
funcao int inicializar_amb(int qtdsala, int vandar, int vler, int vaspirar, int vassoprar, int vpassarvez, int vlimpar, int vdesc, int vbonus, float psujar, float pterremoto)
Tipo: interna (a decidir. No momento, interna)
entrada: sala, andar, sentir, aspirar, assoprar, passarvez, limpar, descarrecar bonus, sujar, terremoto
saida: 0
*/
int inicializar_amb(int qtdsala, int vandar, int vler, int vaspirar, int vassoprar, int vpassarvez, int vlimpar, int vdesc, int vbonus, int vtempolimpo, float psujar, float pterremoto, float psuccao, float pmovimento, float psensorial)
{
  int i, capamin, capamax;
  printf("----------- agente (0): \n");
  printf("inicializar_ambiente()\n");
  printf("----------- ambiente: \n");
  printf("\n");
  printf("----------- Configuracoes do ambiente: \n\n");
  printf("--> Inicializacoes do ambiente: \n");
  if(DEBUG>=1) printf("qtdsala=%d\nMD: v_andar=%d, v_ler=%d, v_aspirar=%d, v_assoprar=%d, v_passarvez=%d, v_limpar=%d, v_desc=%d, v_bonus=%d, v_tempolimpo=%d\nProbabilidades: p_sujar=%.6f, p_terremoto=%.6f, p_succao=%.6f, p_movimento=%.6f, p_sensorial=%.6f\n", qtdsala, vandar, vler, vaspirar, vassoprar, vpassarvez, vlimpar, vdesc, vbonus, vtempolimpo, psujar, pterremoto, psuccao, pmovimento, psensorial);
  inicializou = 1;

  if(qtdsala<2||qtdsala>10)
  {
    qtd_sala=rand()%8+3; /*de 3 a 10 salas */
    if(DEBUG>=1) printf("Quantidade de salas ajustada para valor valido aleatorio %d\n", qtd_sala);
  }
  else /*escolhido pelo usuario, entre 2 a 10 */
    qtd_sala=qtdsala;

  /*salas minima, maxima, e sala de descarga */
  menor_sala=rand()%81+5; /*menor_sala de [5,85], maior de [15,95] */
  maior_sala=menor_sala+qtd_sala; /*maior_sala exclusive*/
  if(FUNCDES)
  {
    descarga_sala=menor_sala+rand()%qtd_sala;
    /*
    capasaco=qtd_sala; era
    Num. Salas / Cap. Min / Cap. Max
    02 / 1 / 1 |0.66 +0.7 = 1.16
    03 / 1 / 2 |1.00 +0.7 = 1.70
    04 / 2 / 3 |1.33 +0.7 = 2.03
    05 / 2 / 4 |1.66 +0.7 = 2.36
    06 / 2 / 5 |2.00 +0.7 = 2.70
    07 / 3 / 6 |2.33 +0.7 = 3.03
    08 / 3 / 7 |2.66 +0.7 = 3.36
    09 / 3 / 8 |3.00 +0.7 = 3.70
    10 / 4 / 9 |3.33 +0.7 = 4.03
    */
    if(NIVELDET&DETCAPACIDADE) /*se escolheu esta opcao estocastica*/
    {
      capamin=(int)(qtd_sala/3.0+0.7); //int trunca
      //evitar compilar com -lm soh por causa dessas duas funcoes:
      //capamin=round(qtd_sala/3.0+0.2);
      //capamin=ceil(qtd_sala/3.0);
      capamax=qtd_sala-1;
      capasaco=rand()%(capamax-capamin+1)+capamin;
    }
    else
      capasaco=qtd_sala-1;
  }
  else
  {
    descarga_sala=-1; /*eh o numero da sala e eh indice para sala[] */
    capasaco=-1;
  }

  if(DEBUG>=1) printf("Menor sala = %d, Maior sala = %d\nSala de Descarga = %d, Capacidade do saco = %d\n",menor_sala, maior_sala-1, descarga_sala, capasaco);
  if(psujar<0.000003||psujar>0.9)
  {
    //p_sujar=((float)rand()/(float)RAND_MAX);
    p_sujar= (rand()+0.1)/RAND_MAX*0.9; /* de 0.000003 a 0.9 */
    if(DEBUG>=1) printf("Probabilidade de sujar salas ajustada para valor valido aleatorio de [0,1[ = %.6f\n", p_sujar);
  }
  else
    p_sujar=psujar;
  if(pterremoto<0.000003||pterremoto>=0.9)
  {
    p_terremoto=(rand()+0.1)/RAND_MAX*0.9; /* de 0.000003 a 0.9 */
    if(DEBUG>=1) printf("Probabilidade de terremoto ajustada para valor valido aleatorio de [0,1[ = %.6f\n",p_terremoto);
  }
  else
    p_terremoto=pterremoto;

  if(psuccao<0.000003||psuccao>0.9)
  {
    p_succao= (rand()+0.1)/RAND_MAX*0.9; /* de 0.000003 a 0.9 */
    if(DEBUG>=1) printf("Probabilidade de problemas de succao ajustada para valor valido aleatorio de [0,1[ = %.6f\n", p_succao);
  }
  else
    p_succao=psuccao;

  if(pmovimento<0.000003||pmovimento>0.9)
  {
    p_movimento= (rand()+0.1)/RAND_MAX*0.9; /* de 0.000003 a 0.9 */
    if(DEBUG>=1) printf("Probabilidade de problemas nas rodas ajustada para valor valido aleatorio de [0,1[ = %.6f\n", p_movimento);
  }
  else
    p_movimento=pmovimento;

  if(psensorial<0.000003||psensorial>0.9)
  {
    p_sensorial= (rand()+0.1)/RAND_MAX*0.9; /* de 0.000003 a 0.9 */
    if(DEBUG>=1) printf("Probabilidade de leitura dos sensores ajustada para valor valido aleatorio de [0,1[ = %.6f\n", p_sensorial);
  }
  else
    p_sensorial=psensorial;

  /* sorteia a condicao de cada sala, se limpa ou suja */
  for(i=menor_sala; i<maior_sala; i++)
    sala[i]=((rand()%25)?1:0); /*nem todas iniciam todas salas sujas */

  qtd_limpou=qtd_descarregou=0;
  temnosaco=0; /*para calculo da capacidade do saco*/
  acao_andar=acao_ler=acao_aspirar=acao_assoprar=acao_passarvez=0;
  pos_aspipo=menor_sala+rand()%qtd_sala;
  if(DEBUG>=1) printf("Posicao Inicial do ASPIPO: %d\n",pos_aspipo);
  printf("\n");
  v_andar=vandar;
  v_ler=vler;
  v_aspirar=vaspirar;
  v_assoprar=vassoprar;
  v_passarvez=vpassarvez;
  v_limpar=vlimpar;
  v_desc=vdesc;
  v_bonus=vbonus;
  v_tempolimpo=vtempolimpo;
  terremoto=0;
  flag_bonus=qtd_bonus=0;
  qtd_tempolimpo=0;
  acoes=1;

  /* conhecimento a priori do mundo */
  inicializar_apriori();

  printf("--> Conhecimento a priori nivel (0-Tudo, 5-Nada): %d\n", APRIORI);
  /*conhecimento inicial do mundo: 5-nada, 4-mpos, qtd_sala,  3-menor_sala, maior_sala, 2-sujeiras, 1-capasaco, 0-md, proba, sala desc*/
  if(APRIORI==5)
    printf("Nadica de nada!\n");
  if(APRIORI<=4)
    printf("posicao inicial do agente e quantidade de salas\n");
  if(APRIORI<=3)
    printf("limites das salas\n");
  if(APRIORI<=2)
    printf("condicoes de sujeira/limpeza de cada sala\n");
  if(APRIORI<=1)
    printf("capacidade do saco do aspirador\n");
  if(APRIORI==0)
    printf("pesos da medida de desempenho e probabilidades dos eventos do ambiente\n");

  printf("\n--> Deterministico x Estocastico nivel (0-Deterministico, 127-Estocastico): %d\n", NIVELDET);
  printf("Adicione o valor da opcao para obter o nivel:\n");
  printf(" 0 - Completamente Deterministico\n");
  printf(" 1 - Ambiente se suja com probabilidade p_sujar\n");
  printf(" 2 - O mecanismo de aspirar e assoprar falha com probabilidade p_succao\n");
  printf(" 4 - As rodas falham com probabilidade p_movimento\n");
  printf(" 8 - As leituras dos sensores retornam erradas com probabilidade p_sensorial\n");
  printf("16 - Ocorrem terremotos com probabilidade p_terremoto, apos acoes, mas nao apos leitura de sensores\n");
  printf("32 - Ocorrem terremotos com probabilidade p_terremoto a qualquer tempo\n");
  printf("64 - Capacidade do saco variavel\n");

  printf("\n--> Observavel x Obscuro nivel (0-Observavel, 4-Obscuro): %d\n", NIVELOBS);
  if(NIVELOBS==4)
    printf("Completamente obscuro!\n");
  if(NIVELOBS<=3)
  {
    printf("Compartilhada na struct obs:\n");
    printf("obs.terremoto (flag de terremoto)\n");
  }
  if(NIVELOBS<=2)
    printf("ler_chamado() habilitada (retorna uma sala suja qualquer)\n");
  if(NIVELOBS<=1)
  {
    printf("obs.mpos (posicao do agente)\n");
    printf("obs.sala[i] (condicao da sala i de limpeza/sujeira)\n");
    printf("obs.capasaco (capacidade do saco do agente)\n");
    printf("obs.qtd_sala (quantidade de salas do ambiente)\n");
    printf("obs.menor_sala e obs.maior_sala (limites do ambiente)\n");
    printf("obs.descarga_sala (sala de descarga, se houver)\n");
  }
  if(NIVELOBS==0)
  {
    printf("mostrar_pontos() habilitada (retorna a pontuacao corrente)\n");
    printf("obs.v_andar, obs.v_ler, obs.v_aspirar, obs.v_assoprar, obs.v_passarvez, obs.v_limpar, obs.v_desc, obs.v_bonus, obs.v_tempolimpo ");
    printf("(valores dos pesos para calculo da Medida de Desempenho)\n");
    printf("obs.p_sujar, obs.p_terremoto, obs.p_succao, obs.p_movimento, obs.p_sensorial ");
    printf("(valores das probabilidades dos eventos do ambiente)\n");
  }

  printf("\n--> Sala de Descarga: %s\n", (FUNCDES?"sim":"nao"));
  if(FUNCDES)
    printf("Funcao ler_descarga() habilitada\nCapacidade do saco entre [teto(qtdsala/3), (qtdsala-1)]\n");
  else
    printf("Funcao ler_descarga() desabilitada\nCapacidade do saco infinita\n");
  /* desabilita a sala de descarga, ler_descarga(), capacidade do saco infinita */


  printf("\n--> Folga: %s\n", (FOLGA?"sim":"nao"));
  if(!FOLGA)
  {
    printf("Acao maxima util: %d\n", MAXACOES);
    printf("Programa ignora acoes entre %d e %d\n", MAXACOES+1, MAXACOESFINAL);
    printf("Programa aborta na acao %d se nao chamar finalizar_ambiente() no maximo na acao %d\n\n", MAXACOESFINAL+1, MAXACOESFINAL);
  }
  else
  {
    printf("Acao maxima util: %d\n", MAXACOESFINAL-1);
    printf("Entre %d e %d, o ambiente nao se suja, nem ocorrem terremotos caso habilitados\n", MAXACOES+1, MAXACOESFINAL);
    printf("Programa aborta na acao %d se nao chamar finalizar_ambiente() no maximo na acao %d\n\n", MAXACOESFINAL+1, MAXACOESFINAL);
  }

  /*
  | 63| 64| 65| 66|
  | A |   |   |   |
  |___|***|...|***|
  */
  imprimir_ambiente();

  printf("----------- agente (acao: %d): \n", acoes);
  return 0;
}

/*
funcao int inicializa_ambiente(void)
Tipo: externa
entrada: quantidade de salas desejada, ou 0 para aleatorio
saida: 0
*/
int inicializar_ambiente(int nivelobs, int niveldet, int qs, int apriori, int funcdes, int ffolga, desempenho *mdext, probabilidade *prext)
{
  int vandar, vler, vaspirar, vassoprar, vpassarvez, vlimpar, vdesc, vbonus, vtempolimpo;
  float psujar, pterremoto, psuccao, pmovimento, psensorial;
  srand(rand()%10+time(NULL));
  if(qs<2||qs>10)
    qtd_sala=rand()%9+2; /*de 2 a 10 salas */
  else
    qtd_sala=qs;

  /*
  0-completamente observavel,
  1-sem medida de desempenho e sem mostrar_pontos()
  2-parcialmente observavel com ler_chamada(),
  3-apenas indicacao de terremoto
  4-completamente no escuro
  */
  if(nivelobs<0||nivelobs>4)
    NIVELOBS=0;
  else
    NIVELOBS=nivelobs;

    /*
   0-deterministico
   1-suja com probabilidade p_sujar
   2-mecanismo de aspirar e assoprar falha com probabilidade p_succao
   4-mecanismo de andar falha com probabilidade p_movimento
   8-sensores falham com probabilidade p_sensorial de erro
  16-terremotos com probabilidade p_terremoto, apos acoes, nao apos leituras
  32-terremotos com probabilidade p_terremoto, a qualquer tempo
  64-Capacidade do saco variavel entre teto(qtd_sala/3) e (qtd_sala-1), ou fixa em (qtd_sala-1) c.c.

  Combinacoes binarias entre eles sao validas
  */
  if(niveldet<0||niveldet>127)
    NIVELDET=0;
  else
    NIVELDET=niveldet;

  /*conhecimento inicial do mundo: 5-nada, 4-mpos, qtd_sala,  3-menor_sala, maior_sala, 2-sujeiras, 1-capasaco, 0-md, proba, sala desc*/
  if(apriori<APRIORI0||apriori>APRIORI6)
    APRIORI=0; /*default sem conhecimento*/
  else
    APRIORI=apriori;

  /* desabilita a sala de descarga, ler_descarga(), capacidade do saco infinita */
  if(funcdes!=0&&funcdes!=1)
    FUNCDES=1; /*habilitada por default*/
  else
    FUNCDES=funcdes;

  /* desabilita a sala a folga de MAXACOESFINAL, terminando o agente em exatas MAXACOES */
  if(ffolga!=0&&ffolga!=1)
    FOLGA=0; /*desabilitada por default*/
  else
    FOLGA=ffolga;

  if(qtd_sala==2&&FUNCDES)
    fprintf(stderr,"%s","Aviso: ambiente de apenas 2 salas, sendo uma delas de descarga!\n");

  /* sala, andar, sentir, aspirar, assoprar, passarvez, limpar, descarregar, bonus, sujar, terremoto */
  //  inicializar_amb(qtd_sala, -2, -1, -40, -95, 0, 100, 1000, 0.05, 0.01);
  //   inicializar_amb(3, -10, -10, -10, 1000, 1000, -10, -10, 0.05, 0.01);
  //  if(mdext==NULL && prext==NULL)
  //    inicializar_amb(qtd_sala, -2, -1, -40, -95, 0, 100, 100, 100, 0.05, 0.01, 0.1, 0.25);
  if(mdext!=NULL)
  {
    vandar=mdext->v_andar;
    vler=mdext->v_ler;
    vaspirar=mdext->v_aspirar;
    vassoprar=mdext->v_assoprar;
    vpassarvez=mdext->v_passarvez;
    vlimpar=mdext->v_limpar;
    vdesc=mdext->v_desc;
    vbonus=mdext->v_bonus;
    vtempolimpo=mdext->v_tempolimpo;
  }
  else
  {
    vandar=-2;
    vler=-1;
    vaspirar=-40;
    vassoprar=-95;
    vpassarvez=0;
    vlimpar=100;
    vdesc=100;
    vbonus=100;
    vtempolimpo=1;
  }
  if(prext!=NULL)
  {
    psujar=prext->p_sujar;
    pterremoto=prext->p_terremoto;
    psuccao=prext->p_succao;
    pmovimento=prext->p_movimento;
    psensorial=prext->p_sensorial;
 //bug: como inicializar so alguns, e deixar outros default? probabilidade p={0};
//    printf("%f, %f, %f, %f\n\n", prext->p_terremoto, prext->p_succao, prext->p_movimento, prext->p_sensorial);
  }
  else
  {
    psujar=0.05;
    pterremoto=0.01;
    psuccao=0.25;
    pmovimento=0.1;
    psensorial=0.1;
  }
  inicializar_amb(qtd_sala, vandar, vler, vaspirar, vassoprar, vpassarvez, vlimpar, vdesc, vbonus, vtempolimpo, psujar, pterremoto, psuccao, pmovimento, psensorial);
  return 0;
}

/*
funcao void ambiente(int acao)
Tipo: interna
entrada: acao do agente: 1 ler_posicao, 0 c.c.
saida: nada
*/
static void ambiente(int tacao)
{
  float ps, pt;
  int i, sujousala;
  long penal;

//  ++acoes; *contem o numero da proxima acao do agente * transferido para cada acao
  if(!FOLGA && acoes==MAXACOES+1) /* fara acao 1001! */
  {
    printf("Acabaram as acoes. Deve finalizar_ambiente() imediatamente!\n");
    atualiza_observavel(); /* atualiza variaveis observaveis por opcao NIVELOBS*/
    imprimir_ambiente();
    inicializou=0; /*nao chama mais a funcao ambiente() */
    printf("----------- agente (%d): \n", acoes);
    return;
  }

  if(acoes>MAXACOESFINAL) /*a proxima acao do agente sera 1101!*/
  {
    fprintf(stderr,"(stderr) %d %s", acoes, "acoes! Abortando...\n");
    imprimir_ambiente();
    printf("----------- agente (%d): \n", acoes);  /*1101*/
//    printf("%d acoes! Abortando!\n", acoes);
    finalizar_ambiente();
    printf("Finalizacao forcada! Perdeu todos pontos...\n");
    penal=(long)pontos();
    printf("Perda dos pontos: %d\n", -abs(penal));
    printf("Penalidade por estouro de acoes: -10000\n");
    printf("Resultado final: seu agente fez %ld pontos\n", penal-abs(penal)-10000);
    printf("Abortando o programa\n");
    exit(0);
  }

  terremoto=0;

  /*ainda atualiza a acao 1000, e estabiliza nela, porem apos bonus*/
  /*proxima acao 1002 em diante, ja esta estavel*/
  if(acoes>=MAXACOES+2) /*a proxima acao do agente sera a 1002*/
  {
    atualiza_observavel(); /* atualiza variaveis observaveis por opcao NIVELOBS*/
    imprimir_ambiente();
    printf("----------- agente (%d): \n", acoes);  /*1001*/
    return;
  }

  //calcular o bonus tudo limpo, e ponto por sala limpa por tempo - bonus nao conta no tempo extra!
  flag_bonus=1; /*sim, ganha!*/
  for(i=menor_sala; i<maior_sala; i++)
    if(sala[i]==0) /* achou limpa */
      qtd_tempolimpo++;
    else
      if(descarga_sala!=i) /*achou suja, nao ganha bonus, sala de descarga nao conta */
        flag_bonus=0;
  if(flag_bonus)
  {
    qtd_bonus++;
    if(v_bonus)
      if(DEBUG>=1) printf("Ganhou bonus por manter todas salas limpas!\n");
  }

  if(NIVELDET==DETERMINISTICO)
  {
    atualiza_observavel(); /* atualiza variaveis observaveis por opcao NIVELOBS*/
    imprimir_ambiente();
    printf("----------- agente (%d): \n", acoes);  /*1001*/
    return;
  }

  /*tipo de acao: nao fez leituras de sensores. Posso tentar terremoto */
  if(NIVELDET&(DETTERREMOTOTOTAL|DETTERREMOTOBASICO))
    if((NIVELDET&DETTERREMOTOTOTAL) || ((NIVELDET&DETTERREMOTOBASICO) && tacao!=1))
    {
      pt=((float)rand()/(float)RAND_MAX);
      if(pt < p_terremoto)
      {
        for(i=menor_sala; i<maior_sala; i++)
          sala[i]=1;
        pos_aspipo=menor_sala+rand()%qtd_sala;
        printf("terremoto!\n");
        if(DEBUG>=1) printf("Tudo sujo! Aspipo na sala %d\n", pos_aspipo);
        terremoto=1;
        /*a proxima acao do agente sera a 1001, entao avisa que estabiliou agora na 1000*/
        if(acoes==MAXACOES+1)
        {
          if(NIVELDET|DETSUJEIRA|DETTERREMOTOBASICO|DETTERREMOTOTOTAL)
            printf("O ambiente se tornou estavel para sujeiras e terremotos.\n");
          printf("O agente tem ate %d acoes para finalizar_ambiente() antes do programa abortar\n", MAXACOESFINAL);
        }
        atualiza_observavel(); /* atualiza variaveis observaveis por opcao NIVELOBS*/
        imprimir_ambiente();
        printf("----------- agente (%d): \n", acoes);
        return;
      }
    }

  /* Probabilidade de sujar as salas */
  if(NIVELDET&DETSUJEIRA)
  {
    sujousala=0;
    for(i=menor_sala; i<maior_sala; i++)
    {
      ps=((float)rand()/(float)RAND_MAX);
      if(ps < p_sujar)
      {
        if(DEBUG>=1) printf("sala %d, ", i);
        sala[i]=1;
        sujousala=1;
      }
    }
    if(sujousala==1)
      if(DEBUG>=1) printf("sujada(s)\n");
      else printf("Algumas salas se sujaram\n");
  }

  /*a proxima acao do agente sera a 1001, entao avisa que estabiliou agora na 1000*/
  if(acoes==MAXACOES+1)
  {
    /*se escolheu algo que altere o ambiente, esteja avisado */
    if(NIVELDET|DETSUJEIRA|DETTERREMOTOBASICO|DETTERREMOTOTOTAL)
      printf("O ambiente se tornou estavel para sujeiras e terremotos.\n");
    printf("O agente tem ate %d acoes para finalizar_ambiente() antes do programa abortar\n", MAXACOESFINAL);
  }
  atualiza_observavel(); /* atualiza variaveis observaveis por opcao NIVELOBS*/
  imprimir_ambiente();
  printf("----------- agente (%d): \n", acoes);
} /* ambiente() */

/*
funcao void mostrar_pontos(void)
Tipo: externa
entrada: nada
saida: pontos calculados segundo medida de desempenho fornecida
calcula e imprime a pontuacao parcial
nao gasta acoes, nem chama ambiente()
Nao funciona para NIVELOBS!=0 (completamente observavel)
*/
int mostrar_pontos(void)
{
  int ez;
  if(NIVELOBS!=0)
  {
    if(DEBUG>=1) printf("Os pontos nao sao observaveis...\n");
    return -10000;
  }
  ez=acao_andar*v_andar + acao_ler*v_ler + acao_aspirar*v_aspirar + acao_assoprar*v_assoprar + acao_passarvez*v_passarvez + qtd_limpou*v_limpar + qtd_descarregou*v_desc + qtd_bonus*v_bonus + qtd_tempolimpo*v_tempolimpo;

  if(DEBUG>=1) printf("O agente esta com %d pontos\n", ez);
  return ez;
}

/*
funcao int esquerda(void)
Tipo: externa
entrada: nada
saida: 0 se ok, -1 se erro
move o agente para a esquerda, se nao for o limite da sala
*/
int esquerda(void)
{
  float s;
  printf("esquerda()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_andar++;
  if(NIVELDET&DETMOVIMENTO)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_movimento) /* deu azar, nao andou! */
    {
      if(DEBUG>=1) printf("Nao andou! A roda travou! Fica na sala %d\n", pos_aspipo);
      if(DEBUG>=1) printf("Total de acao andar: %d\n", acao_andar);
      if(DEBUG>=2) mostrar_pontos();
      ambiente(0);
      return 0;
    }
  }
  if(pos_aspipo>menor_sala)
  {
    pos_aspipo--;
    if(DEBUG>=1) printf("Andou para sala %d\n", pos_aspipo);
  }
  else
    if(DEBUG>=1) printf("Nao andou! Trombou na parede! Fica na sala %d\n", pos_aspipo);

  if(DEBUG>=1) printf("Total de acao andar: %d\n", acao_andar);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(0);
  return 0;
}

/*
funcao int direita(void)
Tipo: externa
entrada: nada
saida: 0 se ok, -1 se erro
move o agente para a direita, se nao for o limite da sala
*/
int direita(void)
{
  float s;
  printf("direita()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_andar++;
  if(NIVELDET&DETMOVIMENTO)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_movimento) /* deu azar, nao andou! */
    {
      if(DEBUG>=1) printf("Nao andou! A roda travou! Fica na sala %d\n", pos_aspipo);
      if(DEBUG>=1) printf("Total de acao andar: %d\n", acao_andar);
      if(DEBUG>=2) mostrar_pontos();
      ambiente(0);
      return 0;
    }
  }
  if(pos_aspipo<maior_sala-1)
  {
    pos_aspipo++;
    if(DEBUG>=1) printf("Andou para sala %d\n", pos_aspipo);
  }
  else
    if(DEBUG>=1) printf("Nao andou! Trombou na parede! Fica na sala %d\n", pos_aspipo);
  if(DEBUG>=1) printf("Total de acao andar: %d\n", acao_andar);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(0);
  return 0;
}

/*
funcao int ler_sujeira(void)
Tipo: externa
entrada: nada
saida: 0 se limpo, 1 se sujo, -1 se erro
indica a presenca de sujeira na sala atual
*/
int ler_sujeira(void)
{
  int r;
  float s;
  printf("ler_sujeira()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_ler++;
  r=sala[pos_aspipo]; /*sala de descarga sempre cheia*/
  if(NIVELDET&DETSENSORES)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_sensorial) /* deu azar! */
    {
      r= !r;
      if(DEBUG>=1) printf("Erro na leitura\n");
    }
  }
  if(DEBUG>=1) printf("A sala esta %s\nTotal de acao ler: %d\n", (r?"suja":"limpa"), acao_ler);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(1); /*nao causa terremoto*/
  return r;
}

/*
funcao int ler_posicao(void)
Tipo: externa
entrada: nada
saida: posicao do aspipo, um numero de 3 a 999, ou -1 se erro
indica a posicao do agente
*/
int ler_posicao(void)
{
  int r;
  float s;
  printf("ler_posicao()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_ler++;
  r=pos_aspipo;
  if(NIVELDET&DETSENSORES)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_sensorial) /* deu azar!*/
    {
      //r=rand()%qtd_sala + menor_sala;
      r=(rand()%2*2-1)+pos_aspipo; /*[-1 ou 1]+pos_aspipo*/
      if(DEBUG>=1) printf("Erro na leitura. Posicao lida %d\n", r);
    }
  }
  if(DEBUG>=1) printf("Posicao atual, sala %d\nTotal de acao ler: %d\n", pos_aspipo, acao_ler);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(1); /*nao causa terremoto*/
  return r;
}

/*
funcao int ler_descarga(void)
Tipo: externa
entrada: nada
saida: 1 se a sala eh de descarga, 0 se nao eh ou erro
indica verdadeiro ou falso para a sala atual ser de descarga
*/
int ler_descarga(void)
{
  int r;
  float s;
  printf("ler_descarga()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_ler++;
  if(!FUNCDES)
  {
    if(DEBUG>=1) printf("Sensor quebrado!\n");
    ambiente(1);/*nao causa terremoto*/
    return 0;
  }
  if(pos_aspipo==descarga_sala)
    r=1;
  else
    r=0;
  if(NIVELDET&DETSENSORES)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_sensorial) /* deu azar! */
    {
      if(DEBUG>=1) printf("Erro na leitura\n");
      r= !r;
    }
  }
  if(DEBUG>=1) printf("Esta sala %s de descarga\nTotal de acao ler: %d\n", (r?"eh":"nao eh"), acao_ler);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(1);/*nao causa terremoto*/
  return r;
}


/*
funcao int ler_chamado(void)
Tipo: externa
entrada: nada
saida: alguma sala suja, se disponivel, ou -1 se tudo limpo ou erro
So esta disponivel para NIVELOBS<=2
*/
int ler_chamado(void)
{
  int r, i, ssuja=-1, serro=-1;
  float s;
  printf("ler_chamado()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;

  acao_ler++;
  if(NIVELOBS>2)
  {
    if(DEBUG>=1) printf("Nenhum chamado se ouviu...\n");
    ambiente(1);/*nao causa terremoto*/
    return -1;
  }

  serro=descarga_sala; //erro pede para limpar sala de descarga (que pode ser -1)
  for(i=menor_sala; i<maior_sala; i++)
  {
    if(i==descarga_sala)
      continue;
    if(sala[i]==1) /*se sala suja*/
    {
      if(ssuja==-1)
        ssuja=i;      /* ja guarda um possivel primeiro retorno */
      else
        if(rand()%2) /*50% de chance de trocar por outra suja*/
          ssuja=i;   /* troca o retorno */
    }
    else /*achou sala limpa*/
    {
      if(serro==-1||serro==descarga_sala)
        serro=i;      /* ja guarda um possivel primeiro retorno limpo */
      else
        if(rand()%2) /* 50% de chance de escolher outra sala limpa */
          serro=i;   /* troca o retorno */
    }
  }
  r=ssuja; /* retornara a sala suja que achou */
  if(NIVELDET&DETSENSORES)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_sensorial) /* deu azar! */
    {
      r= serro; /* deu erro, retornara uma sala limpa */
      if(DEBUG>=1) printf("Erro na leitura. Chamou a sala %d\n", r);
    }
  }
  if(DEBUG>=1) printf("Sala %d esta suja\nTotal de acao ler: %d\n", r, acao_ler);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(1); /*nao causa terremoto*/
  return r;
}

/*
funcao int aspirar(void)
Tipo: externa
entrada: nada
saida: 0 ok, -1 se erro
Aspira uma sala
*/
int aspirar(void)
{
  float s;
  printf("aspirar()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_aspirar++;
  if(NIVELDET&DETSUCCAO)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_succao) /* deu azar! */
    {
      if(DEBUG>=1) printf("Nao aspirou! Mecanismo engasgou!\n");
      if(DEBUG>=1) printf("Total de acao aspirar: %d\n", acao_aspirar);
      ambiente(0);
      return 0;
    }
  }
  if(pos_aspipo==descarga_sala) /* se desabilitado FUNCDES, descarga_sala==-1 */
  {
    ; /*nao pode aspirar sala de descarga */
    if(DEBUG>=1) printf("Aspirou mas nao limpou! Nao pode limpar sala de descarga!\n");
  }
  else
    //if(temnosaco==qtd_sala && FUNCDES) /*saco cheio e tem descarga.*/
    if(temnosaco==capasaco && FUNCDES) /*saco cheio e tem descarga.*/
    {
      ; /*nao faz nada*/
      if(DEBUG>=1) printf("Aspirou mas nao limpou! Saco cheio com %d limpezas!\n", temnosaco);
    }
    else
      if(sala[pos_aspipo]==SUJA)
      {
        sala[pos_aspipo]=LIMPA;
        qtd_limpou++;
        temnosaco++;
        if(DEBUG>=1) printf("Aspirou e limpou a sala que estava suja!\nJa limpou %d salas\n", qtd_limpou);
        if(DEBUG>=1 && FUNCDES) printf("Capacidade do saco %d. Restante: %d\n", capasaco, capasaco-temnosaco);
      }
      else
        if(DEBUG>=1) printf("Aspirou mas nao limpou! Sala ja estava limpa!\n");
  if(DEBUG>=1) printf("Total de acao aspirar: %d\n", acao_aspirar);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(0);
  return 0;
}

/*
funcao int passar_vez(void)
Tipo: externa
entrada: nada
saida: nada
nao faz nada
*/
int passar_vez(void)
{
  printf("passar_vez()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_passarvez++;
  if(DEBUG>=1) printf("Passou a vez.\n");
  if(DEBUG>=1) printf("Total de acao passar vez: %d\n", acao_passarvez);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(0);
  return 0;
}

/*
funcao int assoprar(void)
Tipo: externa
entrada: nada
saida: 0 ok, -1 se erro
Assopra sujeira na sala atual
*/
int assoprar(void)
{
  float s;
  printf("assoprar()\n");
  printf("----------- ambiente: \n");
  ++acoes;
  if(naoiniciou()) return -1;
  acao_assoprar++;
  if(NIVELDET&DETSUCCAO)
  {
    s=((float)rand()/(float)RAND_MAX);
    if(s<p_succao) /* deu azar! */
    {
      if(DEBUG>=1) printf("Nao assoprou! Mecanismo engasgou!\n");
      if(DEBUG>=1) printf("Total de acao assoprar: %d\n", acao_assoprar);
      if(DEBUG>=2) mostrar_pontos();
      ambiente(0);
      return 0;
    }
  }
  if(pos_aspipo==descarga_sala) /* descarregar o saco */
  {
    if(temnosaco==0) /* saco vazio */
    {
      ;
      if(DEBUG>=1) printf("Assoprou mas nao esvaziou, pois o saco ja estava vazio\n");
    }
    else
    {
      temnosaco=0; /* descarrega o saco do aspipo */
      qtd_descarregou++; /* para pontuacao das descargas corretas */
      if(DEBUG>=1) printf("Esvaziou o saco\n");
      if(DEBUG>=1 && FUNCDES) printf("Capacidade do saco: %d\n", capasaco);
    }
  }
  else /* esta em sala comum. caso jogo de 2 salas, descarga_sala==-1*/
  {
    temnosaco=0; /* descarrega o saco do aspipo */
    sala[pos_aspipo]=SUJA;
    if(DEBUG>=1) printf("Assoprou e sujou a sala %d. Esvaziou o saco\n", pos_aspipo);
  }
  if(DEBUG>=1) printf("Total de acao assoprar: %d\n", acao_assoprar);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(0);
  return 0;
}

/*
funcao int pontos(void)
Tipo: externa
entrada: nada
saida: o numero de pontos se ok, ou -10000 se erro
So roda apos finalizada simulacao. Imprime os pontos finais
*/
int pontos(void)
{
  int r;
  printf("pontos()\n");
  printf("----------- ambiente: \n");
  r=acao_andar*v_andar + acao_ler*v_ler + acao_aspirar*v_aspirar + acao_assoprar*v_assoprar + acao_passarvez*v_passarvez + qtd_limpou*v_limpar + qtd_descarregou*v_desc + qtd_bonus*v_bonus + qtd_tempolimpo*v_tempolimpo;
  if(inicializou==1)
  {
    printf("Favor chamar a funcao finalizar_ambiente()\n");
    return -10000;
  }
  if(DEBUG>=1)
  {
    printf("\nAndadas                        =%4d * %6d = %7d\n", acao_andar, v_andar, acao_andar*v_andar);
    printf("Leituras                       =%4d * %6d = %7d\n", acao_ler, v_ler, acao_ler*v_ler);
    printf("Aspiradas                      =%4d * %6d = %7d\n", acao_aspirar, v_aspirar, acao_aspirar*v_aspirar);
    printf("Assopradas                     =%4d * %6d = %7d\n", acao_assoprar, v_assoprar, acao_assoprar*v_assoprar);
    printf("Passadas de vez                =%4d * %6d = %7d\n", acao_passarvez, v_passarvez, acao_passarvez*v_passarvez);
    printf("Limpezas                       =%4d * %6d = %7d\n", qtd_limpou, v_limpar, qtd_limpou*v_limpar);
    printf("Descargas                      =%4d * %6d = %7d\n", qtd_descarregou, v_desc, qtd_descarregou*v_desc);
    printf("Bonus Tudo Limpo               =%4d * %6d = %7d\n", qtd_bonus, v_bonus, qtd_bonus*v_bonus);
    printf("Ponto por sala limpa por tempo =%4d * %6d = %7d\n\n", qtd_tempolimpo, v_tempolimpo, qtd_tempolimpo*v_tempolimpo);
    printf("Seu agente fez %d pontos\n\n", r);
  }
  return r;
}

/*
funcao int finalizar_ambiente(void)
Tipo: externa
entrada: nada
saida: 0 para ok
usada para encerrar a simulacao
*/
int finalizar_ambiente(void)
{
  int flag_bonus, i;
  printf("finalizar_ambiente()\n");
  printf("----------- ambiente: \n");

  if((inicializou==0 && FOLGA) || (acoes==0 && !FOLGA))
  {
    printf("Precisa inicializar_ambiente() primeiro.\n");
    return -1;
  }
  printf("Finalizada a simulacao.\n");

  //calcular o ultimo bonus tudo limpo apos finalizado
  flag_bonus=1; /*sim, ganha!*/
  for(i=menor_sala; i<maior_sala; i++)
    if(sala[i]==1&&descarga_sala!=i) /*achou suja, nao ganha bonus, sala de descarga nao conta */
      flag_bonus=0;
  if(flag_bonus)
  {
    if(FUNCDES) /*tem descarga, bonus mais dificil!*/
    {
      if(pos_aspipo==descarga_sala)
      {
        qtd_bonus+=10;
        if(v_bonus)
          if(DEBUG>=1) printf("Ganhou 10 X bonus por finalizar com todas salas limpas e agente na sala de descarga!\n");
      }
      else
      {
        qtd_bonus+=5;
        if(v_bonus)
          if(DEBUG>=1) printf("Ganhou 5 X bonus por finalizar com todas salas limpas!\n");
      }
    }
    else
    {
      if(pos_aspipo==menor_sala)
      {
        qtd_bonus+=10;
        if(v_bonus)
          if(DEBUG>=1) printf("Ganhou 10 X bonus por finalizar com todas salas limpas e agente na sala da esquerda!\n");
      }
      else
      {
        qtd_bonus+=5;
        if(v_bonus)
          if(DEBUG>=1) printf("Ganhou 5 X bonus por finalizar com todas salas limpas!\n");
      }
    }
  }

  printf("-------------------------------------------------------\n");
  inicializou = 0;
  return 0;
}

/*
funcao int qtd_acoes(void)
entrada: nada
saida: numero de acoes restantes, ou -1 para erro
Esta funcao nao gasta acoes, nem chama o ambiente()
*/
int qtd_acoes(void)
{
  long penal;
  if(DEBUG>=2) printf("qtd_acoes()\n");
  if(inicializou==0)
  {
    if(acoes>MAXACOESFINAL) /*a proxima acao do agente sera 1101!*/
    {
      fprintf(stderr,"(stderr) %d %s", acoes, "acoes! Abortando...\n");
      printf("Abortando programa apos %d acoes.\n", acoes);
      imprimir_ambiente();
      penal=(long)pontos();
      printf("Perda dos pontos: %d\n", -abs(penal));
      printf("Penalidade por estouro de acoes: -10000\n");
      printf("Resultado final: seu agente fez %ld pontos\n", penal-abs(penal)-10000);//-acoes);
      printf("Abortando o programa\n");
      exit(0);
    }
    else
      return acoes;
  }
  if(DEBUG>=2) printf("Restam %d acoes antes do ambiente estabilizar.\n",((MAXACOES-acoes)<0?0:(MAXACOES-acoes)));
  return acoes;
}

/*
funcao void gastos( ... )
Tipo: externa
Retorna todas as acoes do agente, com a finalidade de ser usada com outros
pesos para calculo da medida de desempenho
*/

void gastos(int *qandar, int *qler, int *qaspirar, int *qassoprar, int *qpassarvez, int *qlimpar, int *qdescarregar, int *qbonus, int *qtempolimpo)
{
  *qandar=acao_andar;
  *qler=acao_ler;
  *qaspirar=acao_aspirar;
  *qassoprar=acao_assoprar;
  *qpassarvez=acao_passarvez;
  *qlimpar=qtd_limpou;
  *qdescarregar=qtd_descarregou;
  *qbonus=qtd_bonus;
  *qtempolimpo=qtd_tempolimpo;
}
