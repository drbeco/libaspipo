/* ASPIPO - aspirador de po inteligente                                   */
/* Autor: Ruben Carlo Benante                                             */
/* email: dr.beco@gmail.com                                               */
/* Copyright 2011. Licenca a definir mantenha sempre o nome do autor      */
/* Codico fechado                                                         */
/*                                                                        */
/* compile com c:\dev-cpp\bin\gcc.exe aspipo.c libaspipo.o -o aspipo.exe  */

/* Ambiente Aspipo:

sala, andar, sentir, aspirar, assoprar, passar_vez, limpar, sujar, terremoto
inicializa_ambiente = (2, -2, -1, -40, -95, 0, 100, 0.1, 0.01)

Acoes do Aspipo (nao retorna valores uteis):
      esquerda()- perde 2 pntos
      direita() - perde 2 ponto.
      aspirar() - perde 40 pontos. Ganha 100 pontos se realmente limpar algo.
      assoprar()- perde 95 pontos. Ganha 100 se assoprar na sala de descarga.
      passar_vez()- nao perde pontos, mas gasta uma acao.
      Todas acoes podem causar terremoto, com probabilidade dada

Sensores do Aspipo (retorna o valor do sensor):
      ler_sujeira()  - perde 1 ponto. Retorna 1 se sujo, 0 se sala limpa.
      ler_posicao()  - perde 1 ponto. Retorna o numero da sala atual.
      ler_descarga() - perde 1 ponto. Retorna 1 se esta for sala de descarga, e retorna 0 cc.
      ler_chamado()  - perde 1 ponto. Retorna uma sala suja necessitando de aspiracao.
      ler_pontos()   - perde 1 ponto. Retorna sua pontuacao atual. Se chamado apos finalizar_ambiente, da a pontuacao final.
      terremoto - variavel inteira que sente terremotos se igual a 1, e 0 cc.
      Os sensores nao causam terremotos.

Reacoes do Ambiente:
      Barra o Aspipo, se ele trombar na parede ao tentar andar para fora do limite.
      Limpa o ambiente, se o Aspipo aspirar.
      Suja o ambiente, se o Aspipo assoprar. O Aspipo fica com o saco vazio.
      Nao faz nada com o aspipo, se ele passar_vez(). Pode ter terremoto ou sujar outras salas.
      Diz a condicao de sujeira, se o aspipo ler_sujeira(). 1=sujo, 0=limpo.
      Diz a posicao do Aspipo, se ele ler_posicao. Responde o numero da sala. Nao ocorre terremoto apos esta funcao.
      Diz se a sala for de descarga, se ele ler_descarga(). 1=sim, 0=nao.
      Diz uma sala que precisa ser limpa, se houver. Retorna -1 cc.

Acoes independentes do Ambiente:
      Aleatoriamente suja uma ou mais salas
      Terremoto! Muda o ASPIPO de sala e suja todas salas. O aspipo sente na variavel externa terremoto==1.
      Terremotos nunca ocorrem apos leitura de sensores.
      Calcula os pontos! ler_pontos()
      Ganha um bonus caso consiga que todas as salas estejam limpas durante uma rodada.

Objetivos:
      Maximizar os pontos atraves da exploracao e limpeza das salas.
      Outras medidas de desempenho podem ser calculadas por equacoes externas a se definir
Condicoes:
      So sao permitidos realizar 1000 acoes.
      Para o caso de apenas x=2 salas no ambiente:
        - Nao ha sala de descarga.
        - Nao ha limite para o saco do Aspipo.
        - A funcao ler_descarga() eh inocua, nao perde pontos nem passa a vez para o ambiente
        - A posicao inicial do agente eh a sala 0.
        - Existem soh a sala 0 e 1
      Para o caso de x=(3 a 10) salas no ambiente:
        - O Aspipo tem um saco capaz de guardar ate x limpezas, sendo x o numero de salas
        - Com o saco cheio, aspirar nao limpa e nem ganha pontos. Mas perde o valor da acao
        - Para esvaziar o saco, procure a sala de descarga e assopre
        - Posicao inicial do agente ASPIPO desconhecida
        - Sala de descarga desconhecida, e sempre suja
        - Existem de 3 a 10 salas, iniciando em valor desconhecido e terminando em valor desconhecido
          numeradas em ordem crescente e sequencial iniciando aleatoriamente proximo de 2 a 90
*/

#define MAXACOES 1000
#define MAXACOESFINAL 1100
#define QTDSALA 100

/* OBS0 -> obs.v_andar, obs.v_ler, obs.v_aspirar, obs.v_assoprar, obs.v_passarvez, obs.v_limpar, obs.v_desc, obs.v_bonus, v_tempolimpo*/
/*         obs.p_sujar, obs.p_terremoto, obs.p_succao, obs.p_movimento, obs.p_sensorial*/
/*         mostrar_pontos() habilitada (retorna a pontuacao corrente)*/
/*         Alem de tudo do OBS1, OBS2 e OBS3*/
/* OBS1 -> obs.mpos (posicao do agente)*/
/*         obs.sala[i] (condicao da sala i de limpeza/sujeira)*/
/*         obs.capasaco (capacidade do saco do agente)*/
/*         obs.qtd_sala (quantidade de salas do ambiente)*/
/*         obs.menor_sala e obs.maior_sala (limites do ambiente)*/
/*         obs.descarga_sala (sala de descarga, se houver)*/
/*         Alem de tudo do OBS2 e OBS3*/
/* OBS2 -> ler_chamado() habilitada (retorna uma sala suja qualquer)*/
/*         Alem de tudo do OBS3*/
/* OBS3 -> obs.terremoto (flag de terremoto)*/
/* OBS4 -> completamente obscuro! */
/*
- OBS0 e OBS1 sao mundos completamente observaveis. A diferenca esta em conhecer a medida de desempenho e poder ler os
seus pontos durante a execucao (OBS0)
- OBS2 tem uma dica de sala suja, que eh um sensor que capta sujeira a distancia
- OBS3 e OBS4 sao identicos se os terremotos estao desabilitados (parametros DETTERREMOTOBASICO ou DETTERREMOTOTOTAL)
*/
#define OBS0 0
#define OBS1 1
#define OBS2 2
#define OBS3 3
#define OBS4 4

/*
  0-deterministico
  1-se suja com probabilidade p_sujar
  2-mecanismo de aspirar e assoprar falha com probabilidade p_sensorial
  4-mecanismo de movimento falha com probabilidade p_movimento
  8-sensores falham com probabilidade p_sensorial de erro
  16-terremoto com probabilidade p_terremoto, mas nao ocorrem apos leituras
  32-terremoto com probabilidade p_terremoto, ocorrem a qualquer tempo
  64-capacidade variavel do saco entre teto(qtd_sala/3) e (qtd_sala-1)

  Combine as opcoes com ou-binario.
  Exemplo: se deseja sujeira e terremoto:
  DETSUJEIRA|DETTERREMOTOBASICO
*/
#define DETERMINISTICO       0  /*0b000 0000 nada acontece*/
#define DETSUJEIRA           1  /*0b000 0001 p_suja*/
#define DETSUCCAO            2  /*0b000 0010 p_succao*/
#define DETMOVIMENTO         4  /*0b000 0100 p_movimento*/
#define DETSENSORES          8  /*0b000 1000 p_sensorial*/
#define DETTERREMOTOBASICO  16  /*0b001 0000 p_terremoto apos acoes, mas nao apos sensores*/
#define DETTERREMOTOTOTAL   32  /*0b010 0000 p_terremoto a qualquer momento*/
#define DETCAPACIDADE       64  /*0b100 0000 capacidade variavel do saco entre teto(qtd_sala/3) e (qtd_sala-1)*/

 /*conhecimento inicial do mundo:
 6-nada,
 5-qtd_sala
 4-menor_sala, maior_sala,
 3-mpos
 2-sujeiras,
 1-capasaco,
 0-md, proba, sala desc*/
#define APRIORI6 6
#define APRIORI5 5
#define APRIORI4 4
#define APRIORI3 3
#define APRIORI2 2
#define APRIORI1 1
#define APRIORI0 0

/* Funcao descarga desabilitada*/
#define DESC0 0
/* Funcao descarga habibilitada*/
#define DESC1 1

/* Folga de 100 iteracoes apos as 1000 originais*/
#define FOLGA1 1
/* Sem folga */
#define FOLGA0 0


/* --------------------------- */
typedef struct
{
  volatile int terremoto; /*volatil, dura uma acao, (1=teve terremoto, 0=nao teve terremoto) na acao passada */
  int mpos; //static int pos_aspipo;
  int capasaco; /*capacidade maxima do saco */
  int sala[QTDSALA]; /* sala[A]=sala[B]=suja */
  int qtd_sala, menor_sala, maior_sala, descarga_sala; /*configuracoes*/
  int v_andar, v_ler, v_aspirar, v_assoprar, v_passarvez, v_limpar, v_desc, v_bonus, v_tempolimpo; /*pesos*/
  float p_sujar, p_terremoto, p_succao, p_movimento, p_sensorial; /*configuracoes*/
} observavel;
extern observavel obs;

typedef struct
{
  int v_andar, v_ler, v_aspirar, v_assoprar, v_passarvez, v_limpar, v_desc, v_bonus, v_tempolimpo; /*pesos*/
} desempenho;
extern desempenho md;

typedef struct
{
  float p_sujar, p_terremoto; /*configuracoes*/
  float p_succao, p_movimento, p_sensorial;
} probabilidade;
extern probabilidade pr;

/* funcoes externas */

/* Agente alterna com Ambiente */
extern int esquerda(void);
extern int direita(void);
extern int aspirar(void);
extern int assoprar(void);
extern int passar_vez(void);
extern int ler_sujeira(void);
extern int ler_posicao(void);
extern int ler_descarga(void);
extern int ler_chamado(void);

/* Configuracoes e consultas da simulacao */
extern int finalizar_ambiente(void);
extern int pontos(void);
extern int qtd_acoes(void);
extern int inicializar_ambiente(int nivelobs, int niveldet, int qs, int apriori, int funcdes, int folga, desempenho *mdext, probabilidade *prext);
extern int mostrar_pontos(void);
extern void gastos(int *qtd_andar, int *qtd_ler, int *qtd_aspirar, int *qtd_assoprar, int *qtd_passarvez, int *qtd_limpar, int *qtd_descarregar, int *qtd_bonus, int *qtd_tempolimpo);

/*
funcao int inicializa_ambiente(void)
Tipo: externa
entrada: quantidade de salas desejada, ou 0 para aleatorio
saida: 0
*/
//int inicializar_ambiente(int qs);

/*
funcao void mostrar_pontos(void)
Tipo: externa
calcula e imprime a pontuacao parcial
nao gasta acoes, nem chama ambiente()
*/
//void mostrar_pontos(void);

/*
funcao int esquerda(void)
Tipo: externa
entrada: nada
saida: 0 se ok, -1 se erro
move o agente para a esquerda, se nao for o limite da sala
*/
//int esquerda(void);

/*
funcao int direita(void)
Tipo: externa
entrada: nada
saida: 0 se ok, -1 se erro
move o agente para a direita, se nao for o limite da sala
*/
//int direita(void);

/*
funcao int ler_sujeira(void)
Tipo: externa
entrada: nada
saida: 0 se limpo, 1 se sujo, -1 se erro
indica a presenca de sujeira na sala atual
*/
//int ler_sujeira(void);

/*
funcao int ler_posicao(void)
Tipo: externa
entrada: nada
saida: posicao do aspipo, um numero de 3 a 999, ou -1 se erro
indica a posicao do agente
*/
//int ler_posicao(void);

/*
funcao int ler_descarga(void)
Tipo: externa
entrada: nada
saida: 1 se a sala eh de descarga, 0 se nao eh, e -1 se erro
indica verdadeiro ou falso para a sala atual ser de descarga.
Não é utilizada em ambientes com 2 salas, caso que retorna 0
*/
//int ler_descarga(void);

/*
funcao int ler_chamado(void)
Tipo: externa
entrada: nada
saida: alguma sala suja, se disponivel, ou -1 se tudo limpo
indica uma sala aleatoria que esta suja, ou -1 se nao houver
*/
//int ler_chamado(void);

/*
funcao int aspirar(void)
Tipo: externa
entrada: nada
saida: 0 ok, -1 se erro
Aspira uma sala
*/
//int aspirar(void);

/*
funcao int passar_vez(void)
Tipo: externa
entrada: nada
saida: nada
nao faz nada
*/
//int passar_vez(void);

/*
funcao int assoprar(void)
Tipo: externa
entrada: nada
saida: 0 ok, -1 se erro
Assopra sujeira na sala atual
*/
//int assoprar(void);

/*
funcao int ler_pontos(void)
Tipo: externa
entrada: nada
saida: o numero de pontos se ok, ou -10000 se erro
So roda apos finalizada simulacao. Imprime os pontos finais
*/
//int ler_pontos(void);

/*
funcao int finalizar_ambiente(void)
Tipo: externa
entrada: nada
saida: 0 para ok
usada para encerrar a simulacao
*/
//int finalizar_ambiente(void);

/*
funcao int qtd_acoes(void)
entrada: nada
saida: numero de acoes restantes, ou -1 para erro
Esta funcao nao gasta acoes, nem chama o ambiente()
*/
//int qtd_acoes(void);

/*
funcao void gastos( ... )
Tipo: externa
Retorna todas as acoes do agente, com a finalidade de ser usada com outros
pesos para calculo da medida de desempenho
*/
//void gastos(int *qtd_andar, int *qtd_ler, int *qtd_aspirar, int *qtd_assoprar, int *qtd_passarvez, int *qtd_limpar, int *qtd_descarregar, int *qtd_bonus);


//--------------------------------


