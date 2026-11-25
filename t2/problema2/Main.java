/*
 * Caio Madeira
 * PS: Minha versão em si mudava a assinatura pois C não tem o .length pro tamanho do vetor.
 *
 * Trabalho T2 - Projeto e Otimizacao de Algoritmos (PUCRS)
 * Problema 2: multiplicacao de matrizes pelo algoritmo de Strassen (1969).
 *
 * Compilar: javac Main.java
 * Executar: java Main            (salva os resultados em results/p2_strassen.csv)
 *
 * Ideia (divisao e conquista):
 *   - Divide A e B (n x n) em 4 blocos (quadrantes) de tamanho n/2 x n/2.
 *   - Em vez das 8 multiplicacoes de blocos do metodo "ingenuo", faz 7 (M1..M7),
 *     pagando com 18 somas/subtracoes de blocos, que sao baratas.
 *   - Recorrencia: T(n) = 7 T(n/2) + O(n^2)  =>  T(n) = O(n^log2(7)) ~ O(n^2.807).
 */
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.Locale;
import java.util.Random;

public class Main {

    // Blocos com tamanho <= este valor sao multiplicados pelo metodo classico.
    // Com valor 1 temos o Strassen "puro", que recursa ate blocos 1 x 1.
    static int limiarParaMetodoClassico = 64;

    // Conta as multiplicacoes de numeros feitas (usado so nos experimentos).
    static long contadorMultiplicacoes = 0;

    /**
     * Retorna C = A x B pelo algoritmo de Strassen.
     * A e B devem ser quadradas e do mesmo tamanho n x n (n qualquer, n >= 1).
     */
    public static int[][] multiply(int[][] A, int[][] B) {
        int n = A.length;
        if (B.length != n) {
            throw new IllegalArgumentException("A e B devem ter o mesmo tamanho");
        }

        // A divisao em quadrantes exige tamanho potencia de 2:
        // calcula a menor potencia de 2 que e maior ou igual a n.
        int tamanho = 1;
        while (tamanho < n) tamanho *= 2;

        // Padding: copia A e B para matrizes tamanho x tamanho completadas com zeros.
        // Os zeros extras nao alteram o produto.
        int[][] ACompletada = new int[tamanho][tamanho];
        int[][] BCompletada = new int[tamanho][tamanho];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                ACompletada[i][j] = A[i][j];
                BCompletada[i][j] = B[i][j];
            }
        }
        int[][] CCompletada = strassenRecursivo(ACompletada, BCompletada);

        // Recorta o canto n x n do resultado (descarta as linhas/colunas do padding).
        int[][] C = new int[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) C[i][j] = CCompletada[i][j];
        }
        return C;
    }

    /** Nucleo do algoritmo: A e B sao n x n, com n potencia de 2. */
    static int[][] strassenRecursivo(int[][] A, int[][] B) {
        int n = A.length;

        // Caso base: bloco pequeno -> metodo classico (mais rapido nesse tamanho).
        if (n <= limiarParaMetodoClassico) {
            return multiplicacaoClassica(A, B);
        }
        int metade = n / 2;

        // DIVISAO: separa cada matriz em 4 quadrantes de tamanho metade x metade.
        //   A = | A11 A12 |     B = | B11 B12 |
        //       | A21 A22 |         | B21 B22 |
        int[][] A11 = quadrante(A, 0, 0), A12 = quadrante(A, 0, metade);
        int[][] A21 = quadrante(A, metade, 0), A22 = quadrante(A, metade, metade);
        int[][] B11 = quadrante(B, 0, 0), B12 = quadrante(B, 0, metade);
        int[][] B21 = quadrante(B, metade, 0), B22 = quadrante(B, metade, metade);

        // CONQUISTA: as 7 multiplicacoes de Strassen, cada uma uma chamada recursiva
        // (10 somas/subtracoes de blocos para montar os fatores):
        //   M1 = (A11+A22)(B11+B22)    M5 = (A11+A12) B22
        //   M2 = (A21+A22) B11         M6 = (A21-A11)(B11+B12)
        //   M3 = A11 (B12-B22)         M7 = (A12-A22)(B21+B22)
        //   M4 = A22 (B21-B11)
        int[][] M1 = strassenRecursivo(somar(A11, A22), somar(B11, B22));
        int[][] M2 = strassenRecursivo(somar(A21, A22), B11);
        int[][] M3 = strassenRecursivo(A11, subtrair(B12, B22));
        int[][] M4 = strassenRecursivo(A22, subtrair(B21, B11));
        int[][] M5 = strassenRecursivo(somar(A11, A12), B22);
        int[][] M6 = strassenRecursivo(subtrair(A21, A11), somar(B11, B12));
        int[][] M7 = strassenRecursivo(subtrair(A12, A22), somar(B21, B22));

        // COMBINACAO: monta os 4 quadrantes de C so com somas e subtracoes
        // (8 somas/subtracoes de blocos), escrevendo direto na matriz C:
        //   C11 = M1 + M4 - M5 + M7    C12 = M3 + M5
        //   C21 = M2 + M4              C22 = M1 - M2 + M3 + M6
        int[][] C = new int[n][n];
        for (int i = 0; i < metade; i++) {
            for (int j = 0; j < metade; j++) {
                C[i][j]                   = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j];
                C[i][j + metade]          = M3[i][j] + M5[i][j];
                C[i + metade][j]          = M2[i][j] + M4[i][j];
                C[i + metade][j + metade] = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j];
            }
        }
        return C;
    }

    /**
     * Multiplicacao classica O(n^3): caso base do Strassen e referencia nos testes.
     * A ordem dos lacos i-k-j percorre B e C linha a linha, o que aproveita melhor
     * a cache do processador (mesmo numero de operacoes que a ordem i-j-k).
     */
    static int[][] multiplicacaoClassica(int[][] A, int[][] B) {
        int n = A.length;
        int[][] C = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int k = 0; k < n; k++)
                for (int j = 0; j < n; j++)
                    C[i][j] += A[i][k] * B[k][j];
        contadorMultiplicacoes += (long) n * n * n; // n^3 multiplicacoes
        return C;
    }

    /** Retorna a matriz X + Y (elemento a elemento). */
    static int[][] somar(int[][] X, int[][] Y) {
        int n = X.length;
        int[][] R = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) R[i][j] = X[i][j] + Y[i][j];
        return R;
    }

    /** Retorna a matriz X - Y (elemento a elemento). */
    static int[][] subtrair(int[][] X, int[][] Y) {
        int n = X.length;
        int[][] R = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) R[i][j] = X[i][j] - Y[i][j];
        return R;
    }

    /** Copia o quadrante (metade x metade) de M que comeca na linha e coluna indicadas. */
    static int[][] quadrante(int[][] M, int linha, int coluna) {
        int metade = M.length / 2;
        int[][] Q = new int[metade][metade];
        for (int i = 0; i < metade; i++)
            for (int j = 0; j < metade; j++) Q[i][j] = M[linha + i][coluna + j];
        return Q;
    }

    // ----------------------------------------------------------------------
    // Testes e experimentos
    // ----------------------------------------------------------------------

    // Semente fixa: os testes e experimentos usam sempre as mesmas matrizes.
    static Random aleatorio = new Random(2026);

    /** Matriz n x n com valores aleatorios de -10 a 10. */
    static int[][] matrizAleatoria(int n) {
        int[][] M = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) M[i][j] = aleatorio.nextInt(21) - 10;
        return M;
    }

    /**
     * Mede o menor tempo (ms) de 5 execucoes. limiar = 0 significa metodo classico.
     * Usamos o menor tempo porque interferencias externas (outros programas, coletor
     * de lixo) so aumentam o tempo medido. Ao final, contadorMultiplicacoes guarda
     * o numero de multiplicacoes de uma execucao.
     */
    static double medir(int[][] A, int[][] B, int limiar) {
        double menor = Double.MAX_VALUE;
        for (int r = 0; r < 5; r++) {
            contadorMultiplicacoes = 0;
            limiarParaMetodoClassico = limiar;
            long inicio = System.nanoTime();
            if (limiar == 0) multiplicacaoClassica(A, B);
            else multiply(A, B);
            menor = Math.min(menor, (System.nanoTime() - inicio) / 1e6);
        }
        return menor;
    }

    public static void main(String[] args) throws IOException {
        String pasta = (args.length > 0) ? args[0] : "results"; // pasta onde o CSV e salvo

        // 1) Exemplo 2 x 2: [[1,3],[7,5]] x [[6,8],[4,2]] = [[18,14],[62,66]]
        //    Com limiar 1 (Strassen puro) devem ser feitas exatamente 7 multiplicacoes.
        int[][] A = {{1, 3}, {7, 5}};
        int[][] B = {{6, 8}, {4, 2}};
        limiarParaMetodoClassico = 1;
        contadorMultiplicacoes = 0;
        int[][] C = multiply(A, B);
        System.out.println("Exemplo 2x2: " + Arrays.deepToString(C)
                + " com " + contadorMultiplicacoes + " multiplicacoes\n");

        // 2) Testes: compara Strassen com o classico para n = 1..70 (testes 0..69 com
        //    limiar 1, testes 70..139 com limiar 64) e para alguns n maiores.
        //    Muitos desses n nao sao potencia de 2, o que testa o padding.
        int testes = 0, falhas = 0;
        int[] maiores = {100, 129, 256, 300};
        for (int t = 0; t < 144; t++) {
            int n = (t < 140) ? (t % 70) + 1 : maiores[t - 140];
            limiarParaMetodoClassico = (t < 70) ? 1 : 64;
            A = matrizAleatoria(n);
            B = matrizAleatoria(n);
            testes++;
            if (!Arrays.deepEquals(multiplicacaoClassica(A, B), multiply(A, B))) {
                falhas++;
                System.out.println("ERRO: n = " + n + ", limiar = " + limiarParaMetodoClassico);
            }
        }
        System.out.println("Testes: " + testes + ", falhas: " + falhas + "\n");

        // 3) Experimentos: multiplicacoes e tempo (menor de 5 execucoes), salvos em CSV.
        //    Compara classico, Strassen com limiar 64 e Strassen puro (este so ate 256,
        //    porque e muito lento).
        new File(pasta).mkdirs();
        PrintWriter csv = new PrintWriter(new FileWriter(pasta + "/p2_strassen.csv"));
        csv.println("n,metodo,multiplicacoes,tempo_ms");
        System.out.printf("%6s %15s %15s %15s %12s %12s %12s%n", "n", "mult_classico",
                "mult_strassen", "mult_puro", "ms_classico", "ms_strassen", "ms_puro");
        for (int n = 64; n <= 2048; n *= 2) {
            A = matrizAleatoria(n);
            B = matrizAleatoria(n);
            double msClassico = medir(A, B, 0);
            long multClassico = contadorMultiplicacoes;
            double msStrassen = medir(A, B, 64);
            long multStrassen = contadorMultiplicacoes;
            double msPuro = -1;   // -1 = nao medido
            long multPuro = -1;
            if (n <= 256) {
                msPuro = medir(A, B, 1);
                multPuro = contadorMultiplicacoes;
            }
            System.out.printf(java.util.Locale.US, "%6d %15d %15d %15d %12.2f %12.2f %12.2f%n",
                    n, multClassico, multStrassen, multPuro, msClassico, msStrassen, msPuro);
            csv.printf(Locale.US, "%d,classico,%d,%.4f%n", n, multClassico, msClassico);
            csv.printf(Locale.US, "%d,strassen64,%d,%.4f%n", n, multStrassen, msStrassen);
            if (n <= 256) csv.printf(Locale.US, "%d,strassen_puro,%d,%.4f%n", n, multPuro, msPuro);
        }
        csv.close();
        System.out.println("\nResultados salvos em " + pasta + "/p2_strassen.csv");
    }
}