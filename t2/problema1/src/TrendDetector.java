import java.util.Objects;

/**
 * Problema 1 — Detecção de tendências (subsequências) em séries de eventos.
 */
public class TrendDetector {

    /**
     * Contador de comparações entre eventos realizadas pela última(s) chamada(s)
     * de {@link #hasTrend}. Usado apenas para a análise experimental; não altera
     * o comportamento do algoritmo. Zere com {@link #resetCounter()}.
     */
    public long comparisons = 0;

    /** Zera o contador de comparações. */
    public void resetCounter() {
        comparisons = 0;
    }

    /**
     * Decide se {@code S_line} é subsequência de {@code S}.
     *
     * @param S      sequência longa de eventos (comprimento n)
     * @param S_line sequência curta de eventos procurada (comprimento m)
     * @return {@code true} se {@code S_line} é subsequência de {@code S};
     *         {@code false} caso contrário
     */
    public boolean hasTrend(String[] S, String[] S_line) {
        // Caso trivial: a sequência vazia é subsequência de qualquer sequência.
        if (S_line == null || S_line.length == 0) {
            return true;
        }
        // Se S não existe ou é menor que S_line, é impossível casar todos os eventos.
        if (S == null || S_line.length > S.length) {
            return false;
        }

        final int n = S.length;
        final int m = S_line.length;
        int j = 0; // índice do próximo evento de S_line a ser casado

        for (int i = 0; i < n; i++) {
            comparisons++;
            // Escolha gulosa: casa S_line[j] com a primeira ocorrência disponível.
            // Objects.equals trata eventos nulos sem lançar exceção.
            if (Objects.equals(S[i], S_line[j])) {
                j++;
                if (j == m) {
                    return true; // todos os m eventos foram casados, em ordem
                }
            } else if (n - i - 1 < m - j) {
                // Poda: restam menos eventos em S (n-i-1) do que faltam casar (m-j).
                return false;
            }
        }
        return false; // S terminou antes de casar todos os eventos de S_line
    }

    /** Pequena demonstração com o exemplo do enunciado. */
    public static void main(String[] args) {
        String[] S = {"buy Amazon", "buy Google", "buy Apple",
                      "buy Google", "buy Google", "buy NVIDIA"};
        String[] S_line = {"buy Google", "buy Apple", "buy Google", "buy NVIDIA"};

        TrendDetector td = new TrendDetector();
        boolean r = td.hasTrend(S, S_line);
        System.out.println("S      = " + String.join(", ", S));
        System.out.println("S_line = " + String.join(", ", S_line));
        System.out.println("hasTrend = " + r + "  (comparações: " + td.comparisons + ")");
    }
}
