/* ===========================================================================
   Exercicio 5 - Problema da Mochila (mochila binaria / knapsack 0-1)
   Disciplina: Analise de Algoritmos - PUCRS Poli ES
   Autor: Caio Madeira

   O que este programa faz:
     - resolve o MESMO problema de duas maneiras diferentes:
         (a) FORCA BRUTA, testando todas as combinacoes possiveis de itens,
             escrita de forma recursiva (divisao-e-conquista);
         (b) PROGRAMACAO DINAMICA, preenchendo a tabela maxTab[N+1][C+1];
     - conta, para cada uma, o numero de ITERACOES, o numero de INSTRUCOES
       e o TEMPO de execucao;
     - imprime uma tabela na tela e grava os mesmos numeros em um arquivo CSV.

   Convencoes de contagem usadas aqui (estao explicadas no relatorio):
     ITERACAO  = uma "passada" do algoritmo.
                 Na versao recursiva: cada CHAMADA da funcao (um no da arvore
                 de decisao "levo / nao levo o item").
                 Na versao com programacao dinamica: cada execucao do corpo do
                 laco mais interno, ou seja, cada CELULA da tabela preenchida.
     INSTRUCAO = uma operacao elementar executada: comparacao, atribuicao,
                 soma/subtracao ou acesso a uma posicao de vetor/matriz.
                 Cada linha do pseudocodigo soma o seu custo no contador.

   Compilar:  gcc -O2 -o exercicio5_mochila exercicio5_mochila.c
   Executar:  ./exercicio5_mochila resultados_mochila.csv
   =========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define QUANTIDADE_MAXIMA_DE_ITENS 40

/* Cada item da mochila tem um peso e um valor. */
typedef struct {
    int peso;
    int valor;
} Item;

/* ---------------------------------------------------------------------------
   Contadores globais. Sao zerados antes de cada execucao.
   "unsigned long long" = inteiro sem sinal de 64 bits: precisamos disso
   porque a forca bruta passa facilmente de 2 bilhoes de passos.
   --------------------------------------------------------------------------- */
static unsigned long long iteracoesForcaBruta;
static unsigned long long instrucoesForcaBruta;
static unsigned long long iteracoesProgramacaoDinamica;
static unsigned long long instrucoesProgramacaoDinamica;

/* ---------------------------------------------------------------------------
   Gerador de numeros pseudoaleatorios proprio (Congruencia Linear).
   Uso um gerador escrito a mao, e nao o rand() da biblioteca, para que os
   casos de teste sejam EXATAMENTE os mesmos em qualquer computador/compilador.
   --------------------------------------------------------------------------- */
static unsigned long long estadoDoGerador;

static void iniciarGerador(unsigned long long semente)
{
    estadoDoGerador = semente;
}

static int sortearInteiro(int valorMinimo, int valorMaximo)
{
    estadoDoGerador = estadoDoGerador * 6364136223846793005ULL + 1442695040888963407ULL;
    unsigned int bitsAltos = (unsigned int)(estadoDoGerador >> 33);
    return valorMinimo + (int)(bitsAltos % (unsigned int)(valorMaximo - valorMinimo + 1));
}

/* ===========================================================================
   (a) FORCA BRUTA - testa todas as combinacoes possiveis
   ---------------------------------------------------------------------------
   Ideia: para o item de indice "indiceAtual" existem apenas duas decisoes:
            1) NAO colocar o item na mochila;
            2) COLOCAR o item na mochila (se ele couber na capacidade que sobrou).
          Tomo as duas decisoes, resolvo o resto do problema em cada caso
          (isso e a divisao-e-conquista: dois subproblemas menores) e fico com
          a melhor das duas respostas.
   Custo: a arvore de decisao tem 2^N folhas -> tempo O(2^N).
   =========================================================================== */
int mochilaPorForcaBruta(const Item itens[], int quantidadeDeItens,
                         int indiceAtual, int capacidadeRestante)
{
    int valorSemOItemAtual;
    int valorComOItemAtual;

    iteracoesForcaBruta++;        /* esta chamada e um no da arvore de decisao */
    instrucoesForcaBruta += 1;    /* comparacao do caso base */

    /* Caso base: acabaram os itens -> nao da para ganhar mais valor nenhum.
       (os itens ocupam as posicoes 1..N do vetor, por isso o teste e "> N") */
    if (indiceAtual > quantidadeDeItens) {
        instrucoesForcaBruta += 1;                      /* o "devolva 0" */
        return 0;
    }

    /* Decisao 1: deixar o item de fora. */
    valorSemOItemAtual = mochilaPorForcaBruta(itens, quantidadeDeItens,
                                              indiceAtual + 1, capacidadeRestante);
    instrucoesForcaBruta += 2;    /* soma indiceAtual+1 e atribuicao do resultado */

    /* Decisao 2: levar o item, desde que ele caiba no espaco que sobrou. */
    valorComOItemAtual = -1;
    instrucoesForcaBruta += 2;    /* acesso a itens[indiceAtual].peso + comparacao */

    if (itens[indiceAtual].peso <= capacidadeRestante) {
        valorComOItemAtual = itens[indiceAtual].valor +
                             mochilaPorForcaBruta(itens, quantidadeDeItens,
                                                  indiceAtual + 1,
                                                  capacidadeRestante - itens[indiceAtual].peso);
        instrucoesForcaBruta += 4; /* acesso ao valor, subtracao da capacidade,
                                      soma do valor e atribuicao */
    }

    /* Fico com a melhor das duas decisoes. */
    instrucoesForcaBruta += 2;    /* comparacao + devolucao */
    if (valorComOItemAtual > valorSemOItemAtual) {
        return valorComOItemAtual;
    }
    return valorSemOItemAtual;
}

/* ===========================================================================
   (b) PROGRAMACAO DINAMICA - o algoritmo visto em aula
   ---------------------------------------------------------------------------
   tabelaDeMaximos[i][j] = melhor valor que consigo usando SOMENTE os itens
                           de 1 ate i, com uma mochila de capacidade j.
   Recorrencia:
       se peso(item i) <= j:
           tabelaDeMaximos[i][j] = max( tabelaDeMaximos[i-1][j],
                                        valor(item i) + tabelaDeMaximos[i-1][j - peso(item i)] )
       senao:
           tabelaDeMaximos[i][j] = tabelaDeMaximos[i-1][j]
   Custo: uma passada por celula -> tempo O(N * C).
   =========================================================================== */
int mochilaPorProgramacaoDinamica(const Item itens[], int quantidadeDeItens,
                                  int capacidadeDaMochila, int itensEscolhidos[])
{
    int linha, coluna;
    int valorSemOItem, valorComOItem;
    int valorMaximoFinal;
    int capacidadeQueSobra;
    int **tabelaDeMaximos;

    /* Aloco a tabela com (N+1) linhas e (C+1) colunas. A linha 0 e a coluna 0
       representam "nenhum item" e "capacidade zero", e valem 0. */
    tabelaDeMaximos = (int **)malloc((size_t)(quantidadeDeItens + 1) * sizeof(int *));
    for (linha = 0; linha <= quantidadeDeItens; linha++) {
        tabelaDeMaximos[linha] = (int *)calloc((size_t)(capacidadeDaMochila + 1), sizeof(int));
    }
    instrucoesProgramacaoDinamica += (unsigned long long)(quantidadeDeItens + 1)
                                   + (unsigned long long)(capacidadeDaMochila + 1);
    /* ^ custo da inicializacao com zero da linha 0 e da coluna 0 */

    for (linha = 1; linha <= quantidadeDeItens; linha++) {
        for (coluna = 1; coluna <= capacidadeDaMochila; coluna++) {

            iteracoesProgramacaoDinamica++;      /* uma celula preenchida */
            instrucoesProgramacaoDinamica += 2;  /* acesso ao peso + comparacao "cabe?" */

            if (itens[linha].peso <= coluna) {
                valorSemOItem = tabelaDeMaximos[linha - 1][coluna];
                valorComOItem = itens[linha].valor +
                                tabelaDeMaximos[linha - 1][coluna - itens[linha].peso];
                instrucoesProgramacaoDinamica += 6; /* 2 acessos a matriz, 1 acesso ao valor,
                                                       1 subtracao, 1 soma, 1 atribuicao */
                if (valorComOItem > valorSemOItem) {
                    tabelaDeMaximos[linha][coluna] = valorComOItem;
                } else {
                    tabelaDeMaximos[linha][coluna] = valorSemOItem;
                }
                instrucoesProgramacaoDinamica += 2; /* comparacao do Max + gravacao na celula */
            } else {
                tabelaDeMaximos[linha][coluna] = tabelaDeMaximos[linha - 1][coluna];
                instrucoesProgramacaoDinamica += 2; /* leitura da celula de cima + gravacao */
            }
        }
        instrucoesProgramacaoDinamica += (unsigned long long)capacidadeDaMochila + 1;
        /* ^ custo do controle do laco interno (incremento + teste de parada) */
    }
    instrucoesProgramacaoDinamica += (unsigned long long)quantidadeDeItens + 1;
    /* ^ custo do controle do laco externo */

    valorMaximoFinal = tabelaDeMaximos[quantidadeDeItens][capacidadeDaMochila];

    /* Volto pela tabela para descobrir QUAIS itens foram escolhidos.
       Esta parte e so para o relatorio; ela nao entra na contagem porque o
       pseudocodigo da aula termina ao devolver maxTab[N][C]. */
    capacidadeQueSobra = capacidadeDaMochila;
    for (linha = quantidadeDeItens; linha >= 1; linha--) {
        if (tabelaDeMaximos[linha][capacidadeQueSobra] != tabelaDeMaximos[linha - 1][capacidadeQueSobra]) {
            itensEscolhidos[linha] = 1;
            capacidadeQueSobra -= itens[linha].peso;
        } else {
            itensEscolhidos[linha] = 0;
        }
    }

    for (linha = 0; linha <= quantidadeDeItens; linha++) {
        free(tabelaDeMaximos[linha]);
    }
    free(tabelaDeMaximos);

    return valorMaximoFinal;
}

/* ---------------------------------------------------------------------------
   Mede o tempo de relogio em milissegundos.
   --------------------------------------------------------------------------- */
static double tempoAtualEmMilissegundos(void)
{
    struct timespec instante;
    clock_gettime(CLOCK_MONOTONIC, &instante);
    return instante.tv_sec * 1000.0 + instante.tv_nsec / 1000000.0;
}

/* ---------------------------------------------------------------------------
   Executa um caso de teste com os dois algoritmos e imprime os numeros.
   O vetor "itens" usa o indice 1 ate N (a posicao 0 fica vazia), exatamente
   como no pseudocodigo da aula.
   --------------------------------------------------------------------------- */
static void executarCasoDeTeste(FILE *arquivoCsv, const char *nomeDoCaso,
                                Item itens[], int quantidadeDeItens,
                                int capacidadeDaMochila, int limiteParaForcaBruta)
{
    int valorForcaBruta = -1;
    int valorProgramacaoDinamica;
    int itensEscolhidos[QUANTIDADE_MAXIMA_DE_ITENS + 1];
    double inicio, tempoForcaBruta = -1.0, tempoProgramacaoDinamica;
    int indice;
    int pesoTotalUsado = 0;

    printf("\n================================================================\n");
    printf("CASO: %s   (N = %d itens, C = %d de capacidade)\n",
           nomeDoCaso, quantidadeDeItens, capacidadeDaMochila);
    printf("----------------------------------------------------------------\n");
    printf("itens (peso, valor): ");
    for (indice = 1; indice <= quantidadeDeItens && indice <= 12; indice++) {
        printf("(%d,%d) ", itens[indice].peso, itens[indice].valor);
    }
    if (quantidadeDeItens > 12) printf("... (+%d itens)", quantidadeDeItens - 12);
    printf("\n");

    /* ---- Forca bruta ---- */
    iteracoesForcaBruta = 0;
    instrucoesForcaBruta = 0;
    if (quantidadeDeItens <= limiteParaForcaBruta) {
        inicio = tempoAtualEmMilissegundos();
        valorForcaBruta = mochilaPorForcaBruta(itens, quantidadeDeItens, 1, capacidadeDaMochila);
        tempoForcaBruta = tempoAtualEmMilissegundos() - inicio;
        /* obs.: comeco no indice 1 porque a posicao 0 do vetor esta vazia */
    }

    /* ---- Programacao dinamica ---- */
    iteracoesProgramacaoDinamica = 0;
    instrucoesProgramacaoDinamica = 0;
    inicio = tempoAtualEmMilissegundos();
    valorProgramacaoDinamica = mochilaPorProgramacaoDinamica(itens, quantidadeDeItens,
                                                            capacidadeDaMochila, itensEscolhidos);
    tempoProgramacaoDinamica = tempoAtualEmMilissegundos() - inicio;

    if (quantidadeDeItens <= limiteParaForcaBruta) {
        printf("Forca bruta      : valor = %d | iteracoes = %llu | instrucoes = %llu | tempo = %.3f ms\n",
               valorForcaBruta, iteracoesForcaBruta, instrucoesForcaBruta, tempoForcaBruta);
    } else {
        printf("Forca bruta      : NAO EXECUTADA (2^%d combinacoes seria inviavel)\n", quantidadeDeItens);
    }
    printf("Prog. dinamica   : valor = %d | iteracoes = %llu | instrucoes = %llu | tempo = %.3f ms\n",
           valorProgramacaoDinamica, iteracoesProgramacaoDinamica,
           instrucoesProgramacaoDinamica, tempoProgramacaoDinamica);

    printf("Itens escolhidos pela PD: ");
    for (indice = 1; indice <= quantidadeDeItens; indice++) {
        if (itensEscolhidos[indice]) {
            printf("#%d(p=%d,v=%d) ", indice, itens[indice].peso, itens[indice].valor);
            pesoTotalUsado += itens[indice].peso;
        }
    }
    printf("| peso total usado = %d de %d\n", pesoTotalUsado, capacidadeDaMochila);

    if (quantidadeDeItens <= limiteParaForcaBruta) {
        printf("Conferencia: os dois algoritmos deram o mesmo valor? %s\n",
               (valorForcaBruta == valorProgramacaoDinamica) ? "SIM" : "NAO (ERRO!)");
    }

    /* Linha do CSV: caso;N;C;algoritmo;resultado;iteracoes;instrucoes;tempo_ms */
    if (quantidadeDeItens <= limiteParaForcaBruta) {
        fprintf(arquivoCsv, "%s;%d;%d;Forca Bruta;%d;%llu;%llu;%.3f\n",
                nomeDoCaso, quantidadeDeItens, capacidadeDaMochila, valorForcaBruta,
                iteracoesForcaBruta, instrucoesForcaBruta, tempoForcaBruta);
    } else {
        fprintf(arquivoCsv, "%s;%d;%d;Forca Bruta;-1;0;0;-1\n",
                nomeDoCaso, quantidadeDeItens, capacidadeDaMochila);
    }
    fprintf(arquivoCsv, "%s;%d;%d;Programacao Dinamica;%d;%llu;%llu;%.3f\n",
            nomeDoCaso, quantidadeDeItens, capacidadeDaMochila, valorProgramacaoDinamica,
            iteracoesProgramacaoDinamica, instrucoesProgramacaoDinamica, tempoProgramacaoDinamica);
    fflush(arquivoCsv);
}

int main(int argc, char *argv[])
{
    FILE *arquivoCsv;
    Item itens[QUANTIDADE_MAXIMA_DE_ITENS + 1];
    int indice;
    const char *nomeDoArquivoCsv = (argc > 1) ? argv[1] : "resultados_mochila.csv";

    arquivoCsv = fopen(nomeDoArquivoCsv, "w");
    fprintf(arquivoCsv, "caso;quantidade_de_itens;capacidade;algoritmo;valor_maximo;iteracoes;instrucoes;tempo_ms\n");

    /* ---------------- Caso 1: o exemplo pequeno, para conferir na mao ------- */
    itens[1].peso = 5;  itens[1].valor = 10;
    itens[2].peso = 4;  itens[2].valor = 40;
    itens[3].peso = 6;  itens[3].valor = 30;
    itens[4].peso = 3;  itens[4].valor = 50;
    executarCasoDeTeste(arquivoCsv, "A - exemplo da aula", itens, 4, 10, 30);

    /* ---------------- Caso 2: 12 itens ------------------------------------- */
    iniciarGerador(2026);
    for (indice = 1; indice <= 12; indice++) {
        itens[indice].peso  = sortearInteiro(1, 20);
        itens[indice].valor = sortearInteiro(5, 100);
    }
    executarCasoDeTeste(arquivoCsv, "B - 12 itens", itens, 12, 60, 30);

    /* ---------------- Caso 3: 20 itens ------------------------------------- */
    iniciarGerador(7);
    for (indice = 1; indice <= 20; indice++) {
        itens[indice].peso  = sortearInteiro(1, 25);
        itens[indice].valor = sortearInteiro(5, 150);
    }
    executarCasoDeTeste(arquivoCsv, "C - 20 itens", itens, 20, 100, 30);

    /* ---------------- Caso 4: 25 itens (a forca bruta ja sofre) ------------- */
    iniciarGerador(13);
    for (indice = 1; indice <= 25; indice++) {
        itens[indice].peso  = sortearInteiro(1, 30);
        itens[indice].valor = sortearInteiro(5, 200);
    }
    executarCasoDeTeste(arquivoCsv, "D - 25 itens", itens, 25, 150, 30);

    /* ---------------- Caso 5: 35 itens (forca bruta desligada) -------------- */
    iniciarGerador(99);
    for (indice = 1; indice <= 35; indice++) {
        itens[indice].peso  = sortearInteiro(1, 40);
        itens[indice].valor = sortearInteiro(5, 250);
    }
    executarCasoDeTeste(arquivoCsv, "E - 35 itens", itens, 35, 200, 30);

    fclose(arquivoCsv);
    printf("\nResultados gravados em %s\n", nomeDoArquivoCsv);
    return 0;
}
