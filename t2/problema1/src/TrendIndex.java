import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/*
 * Extensão do Problema 1 (não exigida pelo enunciado)
 */
public class TrendIndex {

    /** evento -> posições (crescentes) onde ocorre em S */
    private final Map<String, int[]> positions = new HashMap<>();

    /** Contador de passos de busca binária (para análise experimental). */
    public long steps = 0;

    /** Pré-processamento O(n): constrói as listas de posições. */
    public TrendIndex(String[] S) {
        Map<String, List<Integer>> tmp = new HashMap<>();
        for (int i = 0; i < S.length; i++) {
            tmp.computeIfAbsent(S[i], k -> new ArrayList<>()).add(i);
        }
        for (Map.Entry<String, List<Integer>> e : tmp.entrySet()) {
            List<Integer> l = e.getValue();
            int[] arr = new int[l.size()];
            for (int k = 0; k < arr.length; k++) arr[k] = l.get(k);
            positions.put(e.getKey(), arr);
        }
    }

    /** Consulta O(m·log n): S_line é subsequência de S? */
    public boolean hasTrend(String[] S_line) {
        int last = -1; // posição em S do último evento casado
        for (String ev : S_line) {
            int[] pos = positions.get(ev);
            if (pos == null) return false;          // evento nunca ocorre em S
            int k = firstGreater(pos, last);         // 1ª ocorrência após 'last'
            if (k == pos.length) return false;       // não há ocorrência depois
            last = pos[k];
        }
        return true;
    }

    /** Menor índice k tal que a[k] > x (ou a.length se não existir). */
    private int firstGreater(int[] a, int x) {
        int lo = 0, hi = a.length;
        while (lo < hi) {
            steps++;
            int mid = (lo + hi) >>> 1;
            if (a[mid] > x) hi = mid; else lo = mid + 1;
        }
        return lo;
    }
}
