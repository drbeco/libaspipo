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

/* --------------------------- */

/* funcoes externas */

extern volatile int terremoto;
extern int esquerda(void);
extern int direita(void);
extern int aspirar(void);
extern int assoprar(void);
extern int passar_vez(void);
extern int ler_sujeira(void);
extern int ler_posicao(void);
extern int ler_descarga(void);
extern int ler_chamado(void);
extern int finalizar_ambiente(void);
extern int ler_pontos(void);
extern int qtd_acoes(void);

extern int inicializar_ambiente(int);
extern int inicializar_amb(int qtdsala, int vandar, int vler, int vaspirar, int vassoprar, int vpassarvez, int vlimpar, int vdesc, int vbonus, float psujar, float pterremoto);
extern void mostrar_pontos(void);
extern void gastos(int *qtd_andar, int *qtd_ler, int *qtd_aspirar, int *qtd_assoprar, int *qtd_passarvez, int *qtd_limpar, int *qtd_descarregar, int *qtd_bonus);


/*
funcao int inicializa_amb(int qtdsala, int vandar, int vler, int vaspirar, int vassoprar, int vlimpar, float psujar, float pterremoto)
Tipo: interna (a decidir. No momento, interna)
entrada: sala, andar, sentir, aspirar, assoprar, limpar, sujar, terremoto
saida: 0
*/
int inicializar_amb(int qtdsala, int vandar, int vler, int vaspirar, int vassoprar, int vpassarvez, int vlimpar, int vdesc, int vbonus, float psujar, float pterremoto);

/*
funcao int inicializa_ambiente(void)
Tipo: externa
entrada: quantidade de salas desejada, ou 0 para aleatorio
saida: 0
*/
int inicializar_ambiente(int qs);

/*
funcao void mostrar_pontos(void)
Tipo: externa
calcula e imprime a pontuacao parcial
nao gasta acoes, nem chama ambiente()
*/
void mostrar_pontos(void);

/*
funcao int esquerda(void)
Tipo: externa
entrada: nada
saida: 0 se ok, -1 se erro
move o agente para a esquerda, se nao for o limite da sala
*/
int esquerda(void);

/*
funcao int direita(void)
Tipo: externa
entrada: nada
saida: 0 se ok, -1 se erro
move o agente para a direita, se nao for o limite da sala
*/
int direita(void);

/*
funcao int ler_sujeira(void)
Tipo: externa
entrada: nada
saida: 0 se limpo, 1 se sujo, -1 se erro
indica a presenca de sujeira na sala atual
*/
int ler_sujeira(void);

/*
funcao int ler_posicao(void)
Tipo: externa
entrada: nada
saida: posicao do aspipo, um numero de 3 a 999, ou -1 se erro
indica a posicao do agente
*/
int ler_posicao(void);

/*
funcao int ler_descarga(void)
Tipo: externa
entrada: nada
saida: 1 se a sala eh de descarga, 0 se nao eh, e -1 se erro
indica verdadeiro ou falso para a sala atual ser de descarga.
Não é utilizada em ambientes com 2 salas, caso que retorna 0
*/
int ler_descarga(void);

/*
funcao int ler_chamado(void)
Tipo: externa
entrada: nada
saida: alguma sala suja, se disponivel, ou -1 se tudo limpo
indica uma sala aleatoria que esta suja, ou -1 se nao houver
*/
int ler_chamado(void);

/*
funcao int aspirar(void)
Tipo: externa
entrada: nada
saida: 0 ok, -1 se erro
Aspira uma sala
*/
int aspirar(void);

/*
funcao int passar_vez(void)
Tipo: externa
entrada: nada
saida: nada
nao faz nada
*/
int passar_vez(void);

/*
funcao int assoprar(void)
Tipo: externa
entrada: nada
saida: 0 ok, -1 se erro
Assopra sujeira na sala atual
*/
int assoprar(void);

/*
funcao int ler_pontos(void)
Tipo: externa
entrada: nada
saida: o numero de pontos se ok, ou -10000 se erro
So roda apos finalizada simulacao. Imprime os pontos finais
*/
int ler_pontos(void);

/*
funcao int finalizar_ambiente(void)
Tipo: externa
entrada: nada
saida: 0 para ok
usada para encerrar a simulacao
*/
int finalizar_ambiente(void);

/*
funcao int qtd_acoes(void)
entrada: nada
saida: numero de acoes restantes, ou -1 para erro
Esta funcao nao gasta acoes, nem chama o ambiente()
*/
int qtd_acoes(void);

/*
funcao void gastos( ... )
Tipo: externa
Retorna todas as acoes do agente, com a finalidade de ser usada com outros
pesos para calculo da medida de desempenho
*/
void gastos(int *qtd_andar, int *qtd_ler, int *qtd_aspirar, int *qtd_assoprar, int *qtd_passarvez, int *qtd_limpar, int *qtd_descarregar, int *qtd_bonus);


//--------------------------------


