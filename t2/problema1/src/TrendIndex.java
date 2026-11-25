import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/*
 * Extensão do Problema 1 (não exigida pelo enunciado)
 */
public class TrendIndex {

    private final Map<String, int[]> positions = new HashMap<>();

    public long steps = 0;

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


    public boolean hasTrend(String[] S_line) {
        int last = -1;
        for (String ev : S_line) {
            int[] pos = positions.get(ev);
            if (pos == null) return false;
            int k = firstGreater(pos, last);
            if (k == pos.length) return false;
            last = pos[k];
        }
        return true;
    }

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
