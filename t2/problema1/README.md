# Trabalho T2 — Problema 1 (detecção de tendências / subsequências)

- `src/TrendDetector.java`: `boolean hasTrend(String[] S, String[] S_line)`, algoritmo guloso O(n)
- `src/TrendIndex.java`: extensão com índice de posições + busca binária (muitas consultas)
- `src/Tests.java`: testes de corretude (40 012 verificações)
- `src/Benchmark.java`: experimentos de comparações e tempo (gera CSVs em `results/`)
- `report/`: fonte LaTeX do relatório (`latexmk -pdf relatorio_p1.tex`)

```bash
javac -encoding UTF-8 -d out src/*.java
java -cp out Tests
java -cp out TrendDetector
java -Xmx4g -cp out Benchmark results
```
