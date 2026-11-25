/*
 * Caio Madeira e Bruno Coliselli
 * PS: Minha versão em si mudava a assinatura pois C não tem o .length pro tamanho do vetor.
 *
 *
 * Compilar: javac Main.java
 * Executar: java Main
 *
 * problema: divisao e conquista):
 *   - Divide A e B (n x n) em 4 blocos (quadrantes) de tamanho n/2 x n/2.
 *   - Em vez das 8 multiplicacoes de blocos do metodo "ingenuo", faz 7 (M1..M7),
 *     pagando com 18 somas/subtracoes de blocos, que sao baratas.
 */
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.Locale;
import java.util.Random;

public class Main {

    static int limiarParaMetodoClassico = 64;

    static long contadorMultiplicacoes = 0;

    public static int[][] multiply(int[][] A, int[][] B) {
        int n = A.length;
        if (B.length != n) {
            throw new IllegalArgumentException("A e B devem ter o mesmo tamanho");
        }

        int tamanho = 1;
        while (tamanho < n) tamanho *= 2;

        int[][] ACompletada = new int[tamanho][tamanho];
        int[][] BCompletada = new int[tamanho][tamanho];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                ACompletada[i][j] = A[i][j];
                BCompletada[i][j] = B[i][j];
            }
        }
        int[][] CCompletada = strassenRecursivo(ACompletada, BCompletada);

        int[][] C = new int[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) C[i][j] = CCompletada[i][j];
        }
        return C;
    }

    static int[][] strassenRecursivo(int[][] A, int[][] B) {
        int n = A.length;
        if (n <= limiarParaMetodoClassico) {
            return multiplicacaoClassica(A, B);
        }
        int metade = n / 2;

        int[][] A11 = quadrante(A, 0, 0), A12 = quadrante(A, 0, metade);
        int[][] A21 = quadrante(A, metade, 0), A22 = quadrante(A, metade, metade);
        int[][] B11 = quadrante(B, 0, 0), B12 = quadrante(B, 0, metade);
        int[][] B21 = quadrante(B, metade, 0), B22 = quadrante(B, metade, metade);

        int[][] M1 = strassenRecursivo(somar(A11, A22), somar(B11, B22));
        int[][] M2 = strassenRecursivo(somar(A21, A22), B11);
        int[][] M3 = strassenRecursivo(A11, subtrair(B12, B22));
        int[][] M4 = strassenRecursivo(A22, subtrair(B21, B11));
        int[][] M5 = strassenRecursivo(somar(A11, A12), B22);
        int[][] M6 = strassenRecursivo(subtrair(A21, A11), somar(B11, B12));
        int[][] M7 = strassenRecursivo(subtrair(A12, A22), somar(B21, B22));

        int[][] C = new int[n][n];
        for (int i = 0; i < metade; i++) {
            for (int j = 0; j < metade; j++) {
                C[i][j]                   = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j]; // C11
                C[i][j + metade]          = M3[i][j] + M5[i][j];                       // C12
                C[i + metade][j]          = M2[i][j] + M4[i][j];                       // C21
                C[i + metade][j + metade] = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j]; // C22
            }
        }
        return C;
    }

    static int[][] multiplicacaoClassica(int[][] A, int[][] B) {
        int n = A.length;
        int[][] C = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int k = 0; k < n; k++)
                for (int j = 0; j < n; j++)
                    C[i][j] += A[i][k] * B[k][j];
        contadorMultiplicacoes += (long) n * n * n;
        return C;
    }

    static int[][] somar(int[][] X, int[][] Y) {
        int n = X.length;
        int[][] R = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) R[i][j] = X[i][j] + Y[i][j];
        return R;
    }

    static int[][] subtrair(int[][] X, int[][] Y) {
        int n = X.length;
        int[][] R = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) R[i][j] = X[i][j] - Y[i][j];
        return R;
    }

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

    static Random aleatorio = new Random(2026);

    static int[][] matrizAleatoria(int n) {
        int[][] M = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) M[i][j] = aleatorio.nextInt(21) - 10;
        return M;
    }

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
        int[][] A = {{1, 3}, {7, 5}};
        int[][] B = {{6, 8}, {4, 2}};
        limiarParaMetodoClassico = 1;
        contadorMultiplicacoes = 0;
        int[][] C = multiply(A, B);
        System.out.println("Exemplo 2x2: " + Arrays.deepToString(C)
                + " com " + contadorMultiplicacoes + " multiplicacoes\n");

        // 2) Testes: Strassen x classico para n = 1..70 
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

        // 3) Experimentos: multiplicacoes e tempo (menor de 5 execucoes)
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
            double msPuro = -1;
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
