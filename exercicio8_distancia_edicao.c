/* ===========================================================================
   Exercicio 8 - Distancia de Edicao (distancia de Levenshtein)
   Disciplina: Analise de Algoritmos - PUCRS Poli ES
   Autor: Caio Madeira

   Distancia de edicao = numero minimo de operacoes para transformar a string A
   na string B, usando tres operacoes, todas com custo 1:
       Remocao (R), Insercao (I) e Substituicao (S).
       Quando os caracteres ja sao iguais, ocorre um Match (M), de custo 0.

   O programa resolve o mesmo problema de duas maneiras:
     (a) FORCA BRUTA recursiva: testa todas as sequencias de operacoes possiveis
         (divisao-e-conquista: cada chamada gera tres subproblemas menores);
     (b) PROGRAMACAO DINAMICA: preenche a matriz do pseudocodigo da aula.

   Convencoes de contagem (as mesmas do exercicio 5):
     ITERACAO  = na versao recursiva, cada CHAMADA da funcao;
                 na versao com programacao dinamica, cada CELULA do miolo da
                 matriz que e preenchida (o corpo do laco mais interno).
     INSTRUCAO = cada operacao elementar: comparacao, atribuicao, soma/subtracao
                 ou acesso a uma posicao de vetor/matriz.

   Compilar:  gcc -O2 -o exercicio8_distancia_edicao exercicio8_distancia_edicao.c
   Executar:  ./exercicio8_distancia_edicao resultados_distancia_edicao.csv
   =========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* Custos das operacoes, conforme o enunciado. */
#define CUSTO_DE_REMOCAO      1
#define CUSTO_DE_INSERCAO     1
#define CUSTO_DE_SUBSTITUICAO 1
#define CUSTO_DE_MATCH        0

/* Acima deste tamanho (soma dos comprimentos) a forca bruta nao e executada,
   porque o numero de chamadas cresce de forma exponencial. */
#define LIMITE_PARA_FORCA_BRUTA 24

static unsigned long long iteracoesForcaBruta;
static unsigned long long instrucoesForcaBruta;
static unsigned long long iteracoesForcaBrutaComAtalho;
static unsigned long long instrucoesForcaBrutaComAtalho;
static unsigned long long iteracoesProgramacaoDinamica;
static unsigned long long instrucoesProgramacaoDinamica;

static int menorDeTres(int primeiro, int segundo, int terceiro)
{
    int menor = primeiro;
    if (segundo < menor)  menor = segundo;
    if (terceiro < menor) menor = terceiro;
    return menor;
}

/* ===========================================================================
   (a) FORCA BRUTA - testa todas as combinacoes de operacoes
   ---------------------------------------------------------------------------
   distanciaPorForcaBruta(A, i, B, j) = distancia entre os PREFIXOS
       A[1..i] (os i primeiros caracteres de A) e B[1..j].

   Casos base:
       i == 0 -> so resta inserir os j caracteres que faltam    -> custo j
       j == 0 -> so resta remover os i caracteres que sobraram  -> custo i

   Caso geral: olho o ultimo caractere de cada prefixo e testo as tres saidas:
       remover A[i]           -> 1 + distancia(i-1, j)
       inserir B[j]           -> 1 + distancia(i, j-1)
       substituir/casar       -> custoExtra + distancia(i-1, j-1)
   e fico com a menor. O problema e que os mesmos prefixos sao recalculados
   milhoes de vezes -> tempo exponencial.
   =========================================================================== */
int distanciaPorForcaBruta(const char *primeiraString, int tamanhoDoPrefixoA,
                           const char *segundaString, int tamanhoDoPrefixoB)
{
    int custoExtra;
    int custoRemovendo, custoInserindo, custoSubstituindo;

    iteracoesForcaBruta++;       /* esta chamada e um no da arvore de recursao */
    instrucoesForcaBruta += 2;   /* as duas comparacoes dos casos base */

    if (tamanhoDoPrefixoA == 0) {
        instrucoesForcaBruta += 1;
        return tamanhoDoPrefixoB * CUSTO_DE_INSERCAO;
    }
    if (tamanhoDoPrefixoB == 0) {
        instrucoesForcaBruta += 1;
        return tamanhoDoPrefixoA * CUSTO_DE_REMOCAO;
    }

    /* Os caracteres sao iguais? (as strings em C comecam no indice 0, por isso
       A[i] do pseudocodigo vira primeiraString[i-1] aqui) */
    instrucoesForcaBruta += 3;   /* 2 acessos as strings + 1 comparacao */
    if (primeiraString[tamanhoDoPrefixoA - 1] == segundaString[tamanhoDoPrefixoB - 1]) {
        custoExtra = CUSTO_DE_MATCH;          /* Match: nao pago nada */
    } else {
        custoExtra = CUSTO_DE_SUBSTITUICAO;   /* Substituicao: pago 1 */
    }
    instrucoesForcaBruta += 1;   /* atribuicao de custoExtra */

    custoRemovendo    = distanciaPorForcaBruta(primeiraString, tamanhoDoPrefixoA - 1,
                                               segundaString, tamanhoDoPrefixoB) + CUSTO_DE_REMOCAO;
    custoInserindo    = distanciaPorForcaBruta(primeiraString, tamanhoDoPrefixoA,
                                               segundaString, tamanhoDoPrefixoB - 1) + CUSTO_DE_INSERCAO;
    custoSubstituindo = distanciaPorForcaBruta(primeiraString, tamanhoDoPrefixoA - 1,
                                               segundaString, tamanhoDoPrefixoB - 1) + custoExtra;
    instrucoesForcaBruta += 9;   /* 3 subtracoes de indice, 3 somas de custo, 3 atribuicoes */

    instrucoesForcaBruta += 3;   /* as duas comparacoes do minimo + a devolucao */
    return menorDeTres(custoRemovendo, custoInserindo, custoSubstituindo);
}

/* ===========================================================================
   (a2) FORCA BRUTA COM ATALHO NO MATCH - a mesma ideia, com UMA diferenca:
        quando os dois ultimos caracteres ja sao iguais, nao adianta testar
        remocao nem insercao; a melhor saida sempre e casar os dois caracteres
        e seguir na diagonal. Entao essa versao corta dois dos tres ramos.
        Continua sendo exponencial, mas faz menos chamadas. Coloquei essa
        variante na tabela porque e ela que reproduz o numero que aparece no
        modelo de tabela dos colegas (10.100.193 para Casablanca x Portentoso).
   =========================================================================== */
int distanciaPorForcaBrutaComAtalho(const char *primeiraString, int tamanhoDoPrefixoA,
                                    const char *segundaString, int tamanhoDoPrefixoB)
{
    int custoRemovendo, custoInserindo, custoSubstituindo;

    iteracoesForcaBrutaComAtalho++;
    instrucoesForcaBrutaComAtalho += 2;

    if (tamanhoDoPrefixoA == 0) {
        instrucoesForcaBrutaComAtalho += 1;
        return tamanhoDoPrefixoB * CUSTO_DE_INSERCAO;
    }
    if (tamanhoDoPrefixoB == 0) {
        instrucoesForcaBrutaComAtalho += 1;
        return tamanhoDoPrefixoA * CUSTO_DE_REMOCAO;
    }

    instrucoesForcaBrutaComAtalho += 3;
    if (primeiraString[tamanhoDoPrefixoA - 1] == segundaString[tamanhoDoPrefixoB - 1]) {
        instrucoesForcaBrutaComAtalho += 3;
        return distanciaPorForcaBrutaComAtalho(primeiraString, tamanhoDoPrefixoA - 1,
                                               segundaString, tamanhoDoPrefixoB - 1)
               + CUSTO_DE_MATCH;
    }

    custoRemovendo    = distanciaPorForcaBrutaComAtalho(primeiraString, tamanhoDoPrefixoA - 1,
                                                        segundaString, tamanhoDoPrefixoB) + CUSTO_DE_REMOCAO;
    custoInserindo    = distanciaPorForcaBrutaComAtalho(primeiraString, tamanhoDoPrefixoA,
                                                        segundaString, tamanhoDoPrefixoB - 1) + CUSTO_DE_INSERCAO;
    custoSubstituindo = distanciaPorForcaBrutaComAtalho(primeiraString, tamanhoDoPrefixoA - 1,
                                                        segundaString, tamanhoDoPrefixoB - 1) + CUSTO_DE_SUBSTITUICAO;
    instrucoesForcaBrutaComAtalho += 12;

    return menorDeTres(custoRemovendo, custoInserindo, custoSubstituindo);
}

/* ---------------------------------------------------------------------------
   Estimativa do numero de chamadas que a forca bruta FARIA, para os casos em
   que ela nao roda. Uso a mesma recorrencia da contagem,
        chamadas(i,j) = 1 + chamadas(i-1,j) + chamadas(i,j-1) + chamadas(i-1,j-1),
   mas guardando o LOGARITMO na base 10 de cada valor, senao o numero estoura
   qualquer tipo inteiro (e ate o double). Devolve log10(total de chamadas).
   --------------------------------------------------------------------------- */
static double estimarLog10DasChamadasDaForcaBruta(int tamanhoA, int tamanhoB)
{
    int linha, coluna;
    double *anterior = (double *)malloc((size_t)(tamanhoB + 1) * sizeof(double));
    double *atual    = (double *)malloc((size_t)(tamanhoB + 1) * sizeof(double));
    double maior, soma, resultado;

    for (coluna = 0; coluna <= tamanhoB; coluna++) anterior[coluna] = 0.0; /* log10(1) = 0 */

    for (linha = 1; linha <= tamanhoA; linha++) {
        atual[0] = 0.0;
        for (coluna = 1; coluna <= tamanhoB; coluna++) {
            double a = anterior[coluna];        /* chamadas(i-1, j)   */
            double b = atual[coluna - 1];       /* chamadas(i, j-1)   */
            double c = anterior[coluna - 1];    /* chamadas(i-1, j-1) */
            maior = a; if (b > maior) maior = b; if (c > maior) maior = c;
            soma = pow(10.0, a - maior) + pow(10.0, b - maior) + pow(10.0, c - maior)
                 + pow(10.0, -maior);
            atual[coluna] = maior + log10(soma);
        }
        for (coluna = 0; coluna <= tamanhoB; coluna++) anterior[coluna] = atual[coluna];
    }
    resultado = anterior[tamanhoB];
    free(anterior);
    free(atual);
    return resultado;
}

/* ===========================================================================
   (b) PROGRAMACAO DINAMICA - o pseudocodigo da aula
   ---------------------------------------------------------------------------
   matriz[i][j] = distancia de edicao entre A[1..i] e B[1..j].
   A primeira linha e a primeira coluna sao preenchidas na mao (transformar
   uma string vazia em outra de tamanho k custa k operacoes) e o resto da
   matriz e preenchido de cima para baixo, da esquerda para a direita.
   Custo: uma passada por celula -> tempo O(m * n).
   =========================================================================== */
int distanciaPorProgramacaoDinamica(const char *primeiraString, const char *segundaString,
                                    int mostrarMatriz)
{
    int tamanhoDeA = (int)strlen(primeiraString);
    int tamanhoDeB = (int)strlen(segundaString);
    int linha, coluna, custoExtra, resultado;
    int **matriz;

    matriz = (int **)malloc((size_t)(tamanhoDeA + 1) * sizeof(int *));
    for (linha = 0; linha <= tamanhoDeA; linha++) {
        matriz[linha] = (int *)malloc((size_t)(tamanhoDeB + 1) * sizeof(int));
    }

    /* Primeira coluna: transformar A[1..i] em string vazia = i remocoes. */
    matriz[0][0] = 0;
    for (linha = 1; linha <= tamanhoDeA; linha++) {
        matriz[linha][0] = matriz[linha - 1][0] + CUSTO_DE_REMOCAO;
    }
    /* Primeira linha: transformar string vazia em B[1..j] = j insercoes. */
    for (coluna = 1; coluna <= tamanhoDeB; coluna++) {
        matriz[0][coluna] = matriz[0][coluna - 1] + CUSTO_DE_INSERCAO;
    }
    instrucoesProgramacaoDinamica += (unsigned long long)(tamanhoDeA + tamanhoDeB) * 3ULL + 1ULL;

    /* Miolo da matriz. */
    for (linha = 1; linha <= tamanhoDeA; linha++) {
        for (coluna = 1; coluna <= tamanhoDeB; coluna++) {

            iteracoesProgramacaoDinamica++;      /* uma celula preenchida */

            instrucoesProgramacaoDinamica += 4;  /* 2 acessos as strings, 1 comparacao,
                                                    1 atribuicao de custoExtra */
            if (primeiraString[linha - 1] == segundaString[coluna - 1]) {
                custoExtra = CUSTO_DE_MATCH;
            } else {
                custoExtra = CUSTO_DE_SUBSTITUICAO;
            }

            matriz[linha][coluna] = menorDeTres(matriz[linha - 1][coluna] + CUSTO_DE_REMOCAO,
                                                matriz[linha][coluna - 1] + CUSTO_DE_INSERCAO,
                                                matriz[linha - 1][coluna - 1] + custoExtra);
            instrucoesProgramacaoDinamica += 9;  /* 3 acessos a matriz, 3 somas,
                                                    2 comparacoes do minimo, 1 gravacao */
        }
        instrucoesProgramacaoDinamica += (unsigned long long)tamanhoDeB + 1;  /* controle do laco interno */
    }
    instrucoesProgramacaoDinamica += (unsigned long long)tamanhoDeA + 1;      /* controle do laco externo */

    /* Desenho da matriz, so para os casos pequenos (ajuda a entender). */
    if (mostrarMatriz && tamanhoDeA <= 12 && tamanhoDeB <= 12) {
        printf("\n   Matriz da programacao dinamica (linhas = \"%s\", colunas = \"%s\"):\n",
               primeiraString, segundaString);
        printf("        _");
        for (coluna = 1; coluna <= tamanhoDeB; coluna++) printf("%4c", segundaString[coluna - 1]);
        printf("\n");
        for (linha = 0; linha <= tamanhoDeA; linha++) {
            if (linha == 0) printf("     _  ");
            else            printf("     %c  ", primeiraString[linha - 1]);
            for (coluna = 0; coluna <= tamanhoDeB; coluna++) printf("%4d", matriz[linha][coluna]);
            printf("\n");
        }
        printf("   A resposta e a celula do canto inferior direito.\n");
    }

    resultado = matriz[tamanhoDeA][tamanhoDeB];

    for (linha = 0; linha <= tamanhoDeA; linha++) free(matriz[linha]);
    free(matriz);

    return resultado;
}

static double tempoAtualEmMilissegundos(void)
{
    struct timespec instante;
    clock_gettime(CLOCK_MONOTONIC, &instante);
    return instante.tv_sec * 1000.0 + instante.tv_nsec / 1000000.0;
}

static void executarCasoDeTeste(FILE *arquivoCsv, const char *nomeDoCaso,
                                const char *primeiraString, const char *segundaString,
                                int mostrarMatriz)
{
    int tamanhoDeA = (int)strlen(primeiraString);
    int tamanhoDeB = (int)strlen(segundaString);
    int distanciaForcaBruta = -1, distanciaComAtalho = -1, distanciaProgramacaoDinamica;
    double inicio, tempoForcaBruta = -1.0, tempoComAtalho = -1.0, tempoProgramacaoDinamica;
    int forcaBrutaFoiExecutada = (tamanhoDeA + tamanhoDeB) <= LIMITE_PARA_FORCA_BRUTA;
    double log10DasChamadas;

    printf("\n================================================================\n");
    printf("CASO: %s\n", nomeDoCaso);
    printf("  A = \"%.60s%s\"  (%d caracteres)\n",
           primeiraString, tamanhoDeA > 60 ? "..." : "", tamanhoDeA);
    printf("  B = \"%.60s%s\"  (%d caracteres)\n",
           segundaString, tamanhoDeB > 60 ? "..." : "", tamanhoDeB);
    printf("----------------------------------------------------------------\n");

    iteracoesForcaBruta = 0;
    instrucoesForcaBruta = 0;
    if (forcaBrutaFoiExecutada) {
        inicio = tempoAtualEmMilissegundos();
        distanciaForcaBruta = distanciaPorForcaBruta(primeiraString, tamanhoDeA,
                                                     segundaString, tamanhoDeB);
        tempoForcaBruta = tempoAtualEmMilissegundos() - inicio;
        printf("Forca bruta      : distancia = %d | iteracoes = %llu | instrucoes = %llu | tempo = %.3f ms\n",
               distanciaForcaBruta, iteracoesForcaBruta, instrucoesForcaBruta, tempoForcaBruta);

        iteracoesForcaBrutaComAtalho = 0;
        instrucoesForcaBrutaComAtalho = 0;
        inicio = tempoAtualEmMilissegundos();
        distanciaComAtalho = distanciaPorForcaBrutaComAtalho(primeiraString, tamanhoDeA,
                                                             segundaString, tamanhoDeB);
        tempoComAtalho = tempoAtualEmMilissegundos() - inicio;
        printf("FB c/ atalho     : distancia = %d | iteracoes = %llu | instrucoes = %llu | tempo = %.3f ms\n",
               distanciaComAtalho, iteracoesForcaBrutaComAtalho, instrucoesForcaBrutaComAtalho, tempoComAtalho);
    } else {
        log10DasChamadas = estimarLog10DasChamadasDaForcaBruta(tamanhoDeA, tamanhoDeB);
        printf("Forca bruta      : NAO EXECUTADA - faria cerca de 10^%.0f chamadas\n", log10DasChamadas);
    }

    iteracoesProgramacaoDinamica = 0;
    instrucoesProgramacaoDinamica = 0;
    inicio = tempoAtualEmMilissegundos();
    distanciaProgramacaoDinamica = distanciaPorProgramacaoDinamica(primeiraString, segundaString, mostrarMatriz);
    tempoProgramacaoDinamica = tempoAtualEmMilissegundos() - inicio;
    printf("Prog. dinamica   : distancia = %d | iteracoes = %llu | instrucoes = %llu | tempo = %.3f ms\n",
           distanciaProgramacaoDinamica, iteracoesProgramacaoDinamica,
           instrucoesProgramacaoDinamica, tempoProgramacaoDinamica);

    if (forcaBrutaFoiExecutada) {
        printf("Conferencia: as duas deram a mesma distancia? %s\n",
               (distanciaForcaBruta == distanciaProgramacaoDinamica) ? "SIM" : "NAO (ERRO!)");
        fprintf(arquivoCsv, "%s;%d;%d;Forca Bruta;%d;%llu;%llu;%.3f\n",
                nomeDoCaso, tamanhoDeA, tamanhoDeB, distanciaForcaBruta,
                iteracoesForcaBruta, instrucoesForcaBruta, tempoForcaBruta);
        fprintf(arquivoCsv, "%s;%d;%d;Forca Bruta com atalho;%d;%llu;%llu;%.3f\n",
                nomeDoCaso, tamanhoDeA, tamanhoDeB, distanciaComAtalho,
                iteracoesForcaBrutaComAtalho, instrucoesForcaBrutaComAtalho, tempoComAtalho);
    } else {
        fprintf(arquivoCsv, "%s;%d;%d;Forca Bruta;-1;0;0;-1;10^%.0f\n",
                nomeDoCaso, tamanhoDeA, tamanhoDeB, log10DasChamadas);
        fprintf(arquivoCsv, "%s;%d;%d;Forca Bruta com atalho;-1;0;0;-1\n", nomeDoCaso, tamanhoDeA, tamanhoDeB);
    }
    fprintf(arquivoCsv, "%s;%d;%d;Programacao Dinamica;%d;%llu;%llu;%.3f\n",
            nomeDoCaso, tamanhoDeA, tamanhoDeB, distanciaProgramacaoDinamica,
            iteracoesProgramacaoDinamica, instrucoesProgramacaoDinamica, tempoProgramacaoDinamica);
    fflush(arquivoCsv);
}

int main(int argc, char *argv[])
{
    FILE *arquivoCsv;
    const char *nomeDoArquivoCsv = (argc > 1) ? argv[1] : "resultados_distancia_edicao.csv";

    /* Textos longos do enunciado. Observacao: troquei "Pogancic" sem os acentos
       do original porque em C cada caractere acentuado ocuparia 2 bytes (UTF-8)
       e a contagem de caracteres ficaria errada. */
    const char *textoLongoA =
        "Maven, a Yiddish word meaning accumulator of knowledge, began as an attempt to "
        "simplify the build processes in the Jakarta Turbine project. There were several"
        " projects, each with their own Ant build files, that were all slightly different."
        "JARs were checked into CVS. We wanted a standard way to build the projects, a clear "
        "definition of what the project consisted of, an easy way to publish project information"
        "and a way to share JARs across several projects. The result is a tool that can now be"
        "used for building and managing any Java-based project. We hope that we have created "
        "something that will make the day-to-day work of Java developers easier and generally help "
        "with the comprehension of any Java-based project.";

    const char *textoLongoB =
        "This post is not about deep learning. But it could be might as well. This is the power of "
        "kernels. They are universally applicable in any machine learning algorithm. Why you might"
        "ask? I am going to try to answer this question in this article."
        "Go to the profile of Marin Vlastelica Pogancic"
        "Marin Vlastelica Pogancic Jun";

    arquivoCsv = fopen(nomeDoArquivoCsv, "w");
    fprintf(arquivoCsv, "caso;tamanho_A;tamanho_B;algoritmo;distancia;iteracoes;instrucoes;tempo_ms\n");

    /* Caso 1 - o do enunciado, serve para comparar com a tabela dos colegas. */
    executarCasoDeTeste(arquivoCsv, "1 - Casablanca x Portentoso", "Casablanca", "Portentoso", 0);

    /* Caso 2 - meu caso pequeno, da para conferir a matriz na mao. */
    executarCasoDeTeste(arquivoCsv, "2 - Madeira x Padeiro", "Madeira", "Padeiro", 1);

    /* Caso 3 - meu caso medio: so um caractere a mais de cada lado ja multiplica
       o trabalho da forca bruta por quase 6. */
    executarCasoDeTeste(arquivoCsv, "3 - Programacao x Dinamizacao", "Programacao", "Dinamizacao", 0);

    /* Caso 4 - os textos longos do enunciado: so a programacao dinamica roda. */
    executarCasoDeTeste(arquivoCsv, "4 - Textos longos (Maven x Kernels)", textoLongoA, textoLongoB, 0);

    fclose(arquivoCsv);
    printf("\nResultados gravados em %s\n", nomeDoArquivoCsv);
    return 0;
}
