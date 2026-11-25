import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.Random;

/**
 * Experimentos de desempenho (contagem de comparações e tempo) do Problema 1.
 */
public class Benchmark {

    static String dir = "results";

    public static void main(String[] args) throws IOException {
        if (args.length > 0) dir = args[0];
        new java.io.File(dir).mkdirs();
        problem1Scaling();
        problem1Queries();
        System.out.println("CSV gerados em " + dir + "/");
    }

    // -------------------------------------------------------------- utilitários

    interface Task { Object run(); }

    static Object sink; // evita que a JIT elimine o trabalho medido

    /** Mediana (ms) de 'reps' execuções, após 'warm' execuções de aquecimento. */
    static double timeMs(Task t, int warm, int reps) {
        for (int i = 0; i < warm; i++) sink = t.run();
        double[] v = new double[reps];
        for (int i = 0; i < reps; i++) {
            long t0 = System.nanoTime();
            sink = t.run();
            v[i] = (System.nanoTime() - t0) / 1e6;
        }
        Arrays.sort(v);
        return v[reps / 2];
    }

    static PrintWriter csv(String name, String header) throws IOException {
        PrintWriter w = new PrintWriter(new FileWriter(dir + "/" + name));
        w.println(header);
        return w;
    }

    // --------------------------------------------------------------- Problema 1

    static final String[] TICKERS = new String[50];
    static {
        for (int i = 0; i < TICKERS.length; i++) TICKERS[i] = "buy TICKER" + (char) ('A' + i % 26) + i;
    }

    /** S aleatória sobre 50 tickers; S_line é criada com objetos String distintos
     *  para que equals compare o conteúdo (e não apenas referências). */
    static String[] randomS(Random rnd, int n) {
        String[] s = new String[n];
        for (int i = 0; i < n; i++) s[i] = TICKERS[rnd.nextInt(TICKERS.length)];
        return s;
    }

    static String fresh(String x) { return new String(x.toCharArray()); }

    /** Escalonamento em n para três cenários: melhor, aleatório e pior caso. */
    static void problem1Scaling() throws IOException {
        PrintWriter w = csv("p1_scaling.csv", "n,scenario,m,result,comparisons,time_ms");
        Random rnd = new Random(1);
        int m = 4;
        for (int n : new int[]{1_000, 10_000, 100_000, 1_000_000, 10_000_000}) {
            String[] S = randomS(rnd, n);

            // Melhor caso: S_line = primeiros m eventos de S → m comparações.
            String[] best = new String[m];
            for (int i = 0; i < m; i++) best[i] = fresh(S[i]);

            // Caso aleatório: m tickers sorteados (tipicamente casado bem cedo).
            String[] rand = new String[m];
            for (int i = 0; i < m; i++) rand[i] = fresh(TICKERS[rnd.nextInt(TICKERS.length)]);

            // Pior caso: o último evento de S_line só aparece na última posição
            // de S → o algoritmo precisa examinar todos os n eventos.
            String[] Sw = S.clone();
            Sw[n - 1] = "buy RARE";
            String[] worst = {fresh(Sw[0]), fresh(Sw[1]), fresh(Sw[2]), "buy RARE"};

            // Pior caso negativo: evento ausente → varre S inteira e devolve false.
            String[] absent = {fresh(S[0]), "buy ABSENT"};

            run(w, n, "melhor", S, best);
            run(w, n, "aleatorio", S, rand);
            run(w, n, "pior (true)", Sw, worst);
            run(w, n, "pior (false)", S, absent);
        }
        w.close();
    }

    static void run(PrintWriter w, int n, String scen, String[] S, String[] Sl) {
        TrendDetector td = new TrendDetector();
        boolean r = td.hasTrend(S, Sl);
        long comps = td.comparisons;
        int reps = n >= 1_000_000 ? 15 : 101;
        // Aquecimento proporcional: garante que a JIT compile o laço antes da medição.
        int warm = (int) Math.max(20, 50_000_000L / n);
        double ms = timeMs(() -> td.hasTrend(S, Sl), warm, reps);
        w.printf(java.util.Locale.US, "%d,%s,%d,%b,%d,%.6f%n", n, scen, Sl.length, r, comps, ms);
        System.out.printf(java.util.Locale.US, "P1 n=%d %-12s -> %b comps=%d %.4f ms%n", n, scen, r, comps, ms);
    }

    /** Q consultas sobre a mesma S: guloso repetido vs índice pré-processado. */
    static void problem1Queries() throws IOException {
        PrintWriter w = csv("p1_queries.csv", "n,Q,m,method,total_ms,ops,positives");
        Random rnd = new Random(2);
        int n = 1_000_000, m = 8;
        String[] S = randomS(rnd, n);
        for (int Q : new int[]{10, 100, 1000}) {
            String[][] queries = new String[Q][m];
            for (int q = 0; q < Q; q++) {
                // Metade das consultas termina com um evento ausente (força varredura total).
                for (int i = 0; i < m; i++) queries[q][i] = fresh(TICKERS[rnd.nextInt(TICKERS.length)]);
                if (q % 2 == 1) queries[q][m - 1] = "buy ABSENT";
            }
            TrendDetector td = new TrendDetector();
            int pos = 0;
            for (String[] ql : queries) if (td.hasTrend(S, ql)) pos++;
            long ops = td.comparisons;
            double tGreedy = timeMs(() -> { int c = 0; for (String[] ql : queries) if (td.hasTrend(S, ql)) c++; return c; }, 2, 5);

            double tBuild = timeMs(() -> new TrendIndex(S), 2, 5);
            TrendIndex idx = new TrendIndex(S);
            for (String[] ql : queries) idx.hasTrend(ql);
            long steps = idx.steps;
            double tQuery = timeMs(() -> { int c = 0; for (String[] ql : queries) if (idx.hasTrend(ql)) c++; return c; }, 2, 5);

            w.printf(java.util.Locale.US, "%d,%d,%d,guloso,%.4f,%d,%d%n", n, Q, m, tGreedy, ops, pos);
            w.printf(java.util.Locale.US, "%d,%d,%d,indice (consultas),%.4f,%d,%d%n", n, Q, m, tQuery, steps, pos);
            w.printf(java.util.Locale.US, "%d,%d,%d,indice (construcao),%.4f,%d,%d%n", n, Q, m, tBuild, (long) n, pos);
            System.out.printf(java.util.Locale.US, "P1 Q=%d guloso=%.2fms indice=%.2fms (+%.1fms construção)%n", Q, tGreedy, tQuery, tBuild);
        }
        w.close();
    }

}
