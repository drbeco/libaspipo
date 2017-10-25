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
      Calcula os pontos! ler_pontos()
      
*/

#include "libaspipo.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define QTDSALA 100
#define LIMPA 0
#define SUJA 1
#define DEBUG 1
#define MAXACOES 1000

volatile int terremoto;
static int inicializou = 0;
static int limpou, limpouqtdsala;
static int acao_andar, acao_ler, acao_aspirar, acao_assoprar, acao_passarvez;
static int pos_aspipo;
static int sala[QTDSALA]; /* sala[A]=sala[B]=suja */
static int qtd_sala, v_andar, v_ler, v_aspirar, v_assoprar, v_passarvez, v_limpar, v_bonus;
static int menor_sala, maior_sala, descarga_sala;
static float p_sujar, p_terremoto;
static int acoes;
static int qtdsalalimpa, bonustudolimpo;
static int bonus;

/*
funcao int inicializa_amb(int qtdsala, int vandar, int vler, int vaspirar, int vassoprar, int vlimpar, float psujar, float pterremoto)
Tipo: interna (a decidir. No momento, interna)
entrada: sala, andar, sentir, aspirar, assoprar, passarvez, limpar, bonus, sujar, terremoto
saida: 0 
*/
int inicializar_amb(int qtdsala, int vandar, int vler, int vaspirar, int vassoprar, int vpassarvez, int vlimpar, int vbonus, float psujar, float pterremoto)
{
  int i;
  printf("-------------------------------------------------------\n");
  printf("----------- ambiente: \n");
  printf("inicializar_ambiente()\n");
  if(DEBUG>=1) printf("inicializar_ambiente(qtdsala=%d, v_andar=%d, v_ler=%d, v_aspirar=%d, v_assoprar=%d, v_passarvez=%d, v_limpar=%d, v_bonus=%d, p_sujar=%.2f, p_terremoto=%.2f\n", qtdsala, vandar, vler, vaspirar, vassoprar, vpassarvez, vlimpar, vbonus, psujar, pterremoto);
  inicializou = 1;

  if(qtdsala<2||qtdsala>10)
  {
    qtd_sala=rand()%8+3; /*de 3 a 10 salas */
    if(DEBUG>=1) printf("Quantidade de salas ajustada para valor valido aleatorio %d\n", qtd_sala);
  }
  else /*escolhido pelo usuario, entre 2 a 10 */
    qtd_sala=qtdsala;
  if(qtd_sala==2)
  {
    menor_sala=0;
    maior_sala=2;
    descarga_sala=-1;
  }
  else
  {
    menor_sala=rand()%81+5; /*menor_sala de [5,85], maior de [15,95] */
    maior_sala=menor_sala+qtd_sala; /*maior_sala exclusive*/
    descarga_sala=menor_sala+rand()%qtd_sala;
  }
  if(DEBUG>=1) printf("Menor sala = %d\nMaior sala = %d\nSala de Descarga = %d\nCapacidade do saco = %d\n",menor_sala, maior_sala, descarga_sala, qtd_sala);
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
    //p_terremoto=((float)rand()/(float)RAND_MAX);
    p_terremoto=(rand()+0.1)/RAND_MAX*0.9; /* de 0.000003 a 0.9 */
    if(DEBUG>=1) printf("Probabilidade de terremoto ajustada para valor valido aleatorio de [0,1[ = %.6f\n",p_terremoto);
  }
  else
    p_terremoto=pterremoto;
  for(i=menor_sala; i<maior_sala; i++)
    sala[i]=1; /*iniciam todas salas sujas */

  limpou=0;
  limpouqtdsala=0;
  acao_andar=acao_ler=acao_aspirar=acao_assoprar=acao_passarvez=0;
  if(qtd_sala==2)
    pos_aspipo=0;
  else
    pos_aspipo=menor_sala+rand()%qtd_sala;
  v_andar=vandar;
  v_ler=vler;
  v_aspirar=vaspirar;
  v_assoprar=vassoprar;
  v_passarvez=vpassarvez;
  v_limpar=vlimpar;
  v_bonus=vbonus;
  p_sujar=psujar;
  p_terremoto=pterremoto;
  terremoto=qtdsalalimpa=0;
  bonus=bonustudolimpo=0;
  acoes=1;
  printf("----------- agente (acao: %d): \n", acoes);
  return 0;
}

/*
funcao int inicializa_ambiente(void)
Tipo: externa
entrada: quantidade de salas desejada, ou 0 para aleatorio
saida: 0 
*/
int inicializar_ambiente(int qs)
{
  srand(rand()%10+time(NULL));
  if(qs<2||qs>10)
    qtd_sala=rand()%8+3; /*de 3 a 10 salas */
  else
    qtd_sala=qs;
  /* sala, andar, sentir, aspirar, assoprar, passarvez, limpar, bonus, sujar, terremoto */
  //  inicializar_amb(qtd_sala, -2, -1, -40, -95, 0, 100, 1000, 0.05, 0.01);
 //   inicializar_amb(3, -10, -10, -10, 1000, 1000, -10, -10, 0.05, 0.01);
  inicializar_amb(qtd_sala, -2, -1, -40, -95, 0, 100, 1000, 0.05, 0.01);
  return 0;
}

/*
funcao void ambiente(int acao)
Tipo: interna
entrada: acao do agente: 1 ler_posicao, 0 c.c.
saida: nada 
*/
static void ambiente(int acao)
{
  float ps, pt;
  int i, sujousala;

  ++acoes;
//  printf("----------- ambiente: \n");
  if(acoes>=2000)
  {
    fprintf(stderr,"%s","Duas mil acoes! Abortando!\n");
    printf("Duas mil acoes! Abortando!\n");
    exit(0);
  }

  //calcular o bonus tudo limpo
  bonus=1;
  for(i=menor_sala; i<maior_sala; i++)
    if(sala[i]==1 && descarga_sala!=i) /*achou suja, nao ganha bonus */
    {
      bonus=0;
      break;
    }
  if(bonus)
  {
    bonustudolimpo++;
    if(DEBUG>=1) printf("Ganhou bonus por manter todas salas limpas!\n");
  }
  if(acoes>=MAXACOES+1)
  {
    //inicializou=0;
    if(acoes==MAXACOES+1)
      printf("Estabilizou e nao se sujara mais.\n");
    printf("----------- agente (%d): \n", acoes);
    return;
  }
  terremoto=0;
  sujousala=0;

  if(acao!=1) /*nao leu posicao*/
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
      printf("----------- agente (%d): \n", acoes);
      return;    
    }
  }

  for(i=menor_sala; i<maior_sala; i++)
  {
    ps=((float)rand()/(float)RAND_MAX);
    if(ps < p_sujar)
    {
      if(DEBUG>=1) printf("Sala %d, ", i);
      sala[i]=1;
      sujousala=1;
    }
  }
  if(sujousala==1)
    if(DEBUG>=1) printf("suja(s)\n");
    else printf("Sujou alguma(s) sala(s)\n");
  printf("----------- agente (%d): \n", acoes);
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
    printf("Favor inicializar o ambiente!\n");
    return -1;
  }
  return 0;
}

/*
funcao void mostrar_pontos(void)
Tipo: externa
calcula e imprime a pontuacao parcial
nao gasta acoes, nem chama ambiente()
*/
void mostrar_pontos(void)
{
  int ez;
  ez=acao_andar*v_andar + acao_ler*v_ler + acao_aspirar*v_aspirar + acao_assoprar*v_assoprar + acao_passarvez*v_passarvez + limpou*v_limpar + bonustudolimpo*v_bonus;

  if(DEBUG>=1) printf("Seu agente esta com %d pontos\n", ez);
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
  printf("esquerda()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  if(pos_aspipo>menor_sala)
  {
    pos_aspipo--;
    if(DEBUG>=1) printf("Andou para sala %d\n", pos_aspipo);
  }
  else
    if(DEBUG>=1) printf("Nao andou! Fica na sala %d\n", pos_aspipo);

  acao_andar++;
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
  printf("direita()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  if(pos_aspipo<maior_sala-1)
  {
    pos_aspipo++;      
    if(DEBUG>=1) printf("Andou para sala %d\n", pos_aspipo);
  }
  else
    if(DEBUG>=1) printf("Nao andou! Fica na sala %d\n", pos_aspipo);
  acao_andar++;
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
  printf("ler_sujeira()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  if(pos_aspipo==descarga_sala) /*sala de descarga sempre cheia*/
    r=1;
  else
    r=sala[pos_aspipo];
  acao_ler++;
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
  printf("ler_posicao()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  r=pos_aspipo;
  acao_ler++;
  if(DEBUG>=1) printf("Posicao atual, sala %d\nTotal de acao ler: %d\n", pos_aspipo, acao_ler);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(1); /*nao causa terremoto*/
  return r;
}

/*
funcao int ler_descarga(void)
Tipo: externa
entrada: nada
saida: 1 se a sala eh de descarga, 0 se nao eh, e -1 se erro
indica verdadeiro ou falso para a sala atual ser de descarga
*/
int ler_descarga(void)
{
  int r;
  printf("ler_descarga()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  if(maior_sala==2) /*funcao desabilitada para o caso trivial de 2 salas*/
  {
    if(DEBUG>=1) printf("Funcao ler_descarga() desabilitada para ambiente de 2 salas.\n");
    return -1;
  }
  if(pos_aspipo==descarga_sala)
    r=1;
  else
    r=0;    
  acao_ler++;
  if(DEBUG>=1) printf("Esta sala %s de descarga\nTotal de acao ler: %d\n", (r?"eh":"nao eh"), acao_ler);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(1);/*nao causa terremoto*/
  return r;
}


/*
funcao int ler_chamado(void)
Tipo: externa
entrada: nada
saida: alguma sala suja, se disponivel, ou -1 se tudo limpo
indica uma sala aleatoria que esta suja, ou -1 se nao houver
*/
int ler_chamado(void)
{
  int r, i, s1=-1;
  printf("ler_chamado()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  if(maior_sala==2) /*funcao desabilitada para o caso trivial de 2 salas*/
  {
    if(DEBUG>=1) printf("Funcao ler_chamado() desabilitada para ambiente de 2 salas.\n");
    return -1;
  }
  
  for(i=menor_sala; i<maior_sala; i++)
  {
    if(i==descarga_sala)
      continue;
    if(sala[i]==1)
    {
      if(s1==-1)
        s1=i;      /* ja guarda um possivel primeiro retorno */
      else
        if(rand()%2)
          s1=i;   /* troca o retorno */
    }
  }
  acao_ler++;
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
  printf("aspirar()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  acao_aspirar++;
  if(pos_aspipo==descarga_sala) /* com 2 salas, descarga_sala==-1 */
  {
    ; /*nao pode aspirar sala de descarga */
    if(DEBUG>=1) printf("Aspirou mas nao limpou! Nao pode limpar sala de descarga!\n");
  }
  else
    if(limpouqtdsala==qtd_sala+1 && maior_sala!=2) /*saco cheio. inocuo se jogo com 2 salas*/
    {
      ; /*nao faz nada*/
      if(DEBUG>=1) printf("Aspirou mas nao limpou! Saco cheio com %d limpezas!\n", qtd_sala);
    }
    else
      if(sala[pos_aspipo]==SUJA)
      {
        sala[pos_aspipo]=LIMPA;
        limpou++;
        limpouqtdsala++;
        if(DEBUG>=1) printf("Aspirou e limpou a sala que estava suja!\nJa limpou %d salas\n", limpou);
        if(DEBUG>=1 && maior_sala>2) printf("Capacidade restante do saco: %d\n", qtd_sala-limpouqtdsala);
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
  printf("assoprar()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  acao_assoprar++;
  if(pos_aspipo==descarga_sala) /* descarregar o saco */
  {
    if(limpouqtdsala==0) /* saco vazio */
    {
      ;
      if(DEBUG>=1) printf("Assoprou mas nao esvaziou, pois o saco ja estava vazio\n");
    }
    else
    {
      limpouqtdsala=0; /* descarrega o saco do aspipo */
      limpou++; /* descarga ganha mesma pontuacao que limpeza */
      if(DEBUG>=1) printf("Esvaziou o saco\n");
    }
  }
  else /* esta em sala comum. caso jogo de 2 salas, descarga_sala==-1*/
  {
    limpouqtdsala=0; /* descarrega o saco do aspipo */
    sala[pos_aspipo]=SUJA;
    if(DEBUG>=1) printf("Assoprou e sujou a sala %d. Esvaziou o saco\n", pos_aspipo);
  }
  if(DEBUG>=1) printf("Total de acao assoprar: %d\n", acao_assoprar);
  if(DEBUG>=2) mostrar_pontos();
  ambiente(0);
  return 0;
}

/*
funcao int ler_pontos(void)
Tipo: externa
entrada: nada
saida: o numero de pontos se ok, ou -10000 se erro
So roda apos finalizada simulacao. Imprime os pontos finais
*/
int ler_pontos(void)
{
  int r;
  printf("ler_pontos()\n");
  printf("----------- ambiente: \n");
  r=acao_andar*v_andar + acao_ler*v_ler + acao_aspirar*v_aspirar + acao_assoprar*v_assoprar + acao_passarvez*v_passarvez + limpou*v_limpar + bonustudolimpo*v_bonus;
  if(inicializou==1)
  {
    printf("Favor chamar a funcao finalizar_ambiente()\n");
    return -10000;
  }
  if(DEBUG>=1)
  {
    printf("\nacao_andar      =%4d * %6d\n", acao_andar, v_andar);
    printf("acao_ler        =%4d * %6d\n", acao_ler, v_ler);
    printf("acao_aspirar    =%4d * %6d\n", acao_aspirar, v_aspirar);
    printf("acao_assoprar   =%4d * %6d\n", acao_assoprar, v_assoprar);
    printf("acao_passar_vez =%4d * %6d\n", acao_passarvez, v_passarvez);
    printf("Limpezas        =%4d * %6d\n", limpou, v_limpar);
    printf("Bonus           =%4d * %6d\n\n", bonustudolimpo, v_bonus);
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
  printf("finalizar_ambiente()\n");
  printf("----------- ambiente: \n");
  if(naoiniciou()) return -1;
  printf("Finalizada a simulacao.\n");
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
  if(DEBUG>=2) printf("qtd_acoes()\n");
  if(naoiniciou()) return -1;
  if(DEBUG>=2) printf("Restam %d acoes antes do ambiente estabilizar.\n",((MAXACOES-acoes)<0?0:(MAXACOES-acoes)));
  return acoes;
}

/*
funcao void gastos( ... )
Tipo: externa
Retorna todas as acoes do agente, com a finalidade de ser usada com outros
pesos para calculo da medida de desempenho
*/

void gastos(int *qtd_andar, int *qtd_ler, int *qtd_aspirar, int *qtd_assoprar, int *qtd_passarvez, int *qtd_limpar, int *qtd_bonus)
{
  *qtd_andar=acao_andar;
  *qtd_ler=acao_ler;
  *qtd_aspirar=acao_aspirar;
  *qtd_assoprar=acao_assoprar;
  *qtd_passarvez=acao_passarvez;
  *qtd_limpar=limpou;
  *qtd_bonus=bonustudolimpo;
}
