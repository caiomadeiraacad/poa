import java.util.Objects;


public class TrendDetector {
    public long comparisons = 0;

    public void resetCounter() {
        comparisons = 0;
    }

    /**
     *
     * @param S      sequência longa de eventos (comprimento n)
     * @param S_line sequência curta de eventos procurada (comprimento m)
     * @return {@code true} se {@code S_line} é subsequência de {@code S};
     */
    public boolean hasTrend(String[] S, String[] S_line) {
        if (S_line == null || S_line.length == 0) {
            return true;
        }
        if (S == null || S_line.length > S.length) {
            return false;
        }

        final int n = S.length;
        final int m = S_line.length;
        int j = 0; 

        for (int i = 0; i < n; i++) {
            comparisons++;
            if (Objects.equals(S[i], S_line[j])) {
                j++;
                if (j == m) {
                    return true; 
                }
            } else if (n - i - 1 < m - j) {
                return false;
            }
        }
        return false; 
    }

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
