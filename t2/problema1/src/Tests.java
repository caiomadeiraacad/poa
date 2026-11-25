/* Antonio Coelho, Caio Madeira e Bruno Coliselli */
import java.util.Random;

/**
 * Testes de corretude do Problema 1.
 */
public class Tests {

    private static int passed = 0, failed = 0;

    private static void check(boolean cond, String msg) {
        if (cond) passed++;
        else { failed++; System.out.println("FALHOU: " + msg); }
    }

    // ------------------------------------------------------------ Problema 1

    /** Oráculo: LCS(S, T) == |T| (Θ(n·m), independente do guloso). */
    static boolean oracle(String[] S, String[] T) {
        int n = S.length, m = T.length;
        int[][] L = new int[n + 1][m + 1];
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= m; j++)
                L[i][j] = S[i - 1].equals(T[j - 1]) ? L[i - 1][j - 1] + 1
                                                    : Math.max(L[i - 1][j], L[i][j - 1]);
        return L[n][m] == m;
    }

    static String[] randomSeq(Random rnd, int len, int alphabet) {
        String[] s = new String[len];
        for (int i = 0; i < len; i++) s[i] = "buy E" + rnd.nextInt(alphabet);
        return s;
    }

    static void testProblem1() {
        TrendDetector td = new TrendDetector();

        // Exemplo do enunciado.
        String[] S = {"buy Amazon", "buy Google", "buy Apple", "buy Google", "buy Google", "buy NVIDIA"};
        check(td.hasTrend(S, new String[]{"buy Google", "buy Apple", "buy Google", "buy NVIDIA"}), "exemplo enunciado");
        check(!td.hasTrend(S, new String[]{"buy NVIDIA", "buy Google"}), "ordem invertida");
        check(!td.hasTrend(S, new String[]{"buy Google", "buy Google", "buy Google", "buy Google"}), "repetição demais");
        check(td.hasTrend(S, new String[]{"buy Google", "buy Google", "buy Google"}), "três Google");
        check(!td.hasTrend(S, new String[]{"buy Tesla"}), "evento inexistente");

        // Casos de borda.
        check(td.hasTrend(S, new String[]{}), "S_line vazia");
        check(td.hasTrend(new String[]{}, new String[]{}), "ambas vazias");
        check(!td.hasTrend(new String[]{}, new String[]{"a"}), "S vazia");
        check(td.hasTrend(S, S), "S_line == S");
        check(!td.hasTrend(new String[]{"a"}, new String[]{"a", "a"}), "m > n");
        check(td.hasTrend(new String[]{"a", null, "b"}, new String[]{null, "b"}), "eventos nulos");
        // Strings iguais mas objetos diferentes
        check(td.hasTrend(new String[]{new String("x")}, new String[]{new String("x")}), "equals vs ==");

        // Testes aleatórios contra
        Random rnd = new Random(42);
        for (int t = 0; t < 20000; t++) {
            int n = rnd.nextInt(30), m = rnd.nextInt(8), alpha = 1 + rnd.nextInt(4);
            String[] s = randomSeq(rnd, n, alpha), sl = randomSeq(rnd, m, alpha);
            boolean exp = oracle(s, sl);
            check(td.hasTrend(s, sl) == exp, "aleatório guloso n=" + n + " m=" + m);
            check(new TrendIndex(s).hasTrend(sl) == exp, "aleatório indexado n=" + n + " m=" + m);
        }
    }

    public static void main(String[] args) {
        testProblem1();
        System.out.println("Testes aprovados: " + passed + " | reprovados: " + failed);
        if (failed > 0) System.exit(1);
    }
}
