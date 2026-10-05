/*
Caio Madeira e Bruno

existe uma diferenca entre substring e subsequencia. Substring eh um bloco continuo
de caracteres. ex; gato -> ga, at, ato. 
Uma subsequencia os caracteres precisam aparecer na msm ordem, mas sem necessariamente estare
juntos.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int max(int a, int b)
{
    if (a > b) return a; 
    else return b;
}

int calculate_lcs(const char *str1, const char*str2, long long *iter)
{
    int len1 = strlen(str1); // m
    int len2 = strlen(str2); // n

    int **dp = (int**)malloc((len1 + 1) * sizeof(int*));
    for(int i = 0; i <= len1; i++) {
        dp[i] = (int*)calloc((len2+1), sizeof(int));
        if (dp[i] == NULL) { printf("error no calloc\n"); exit(-1); }
    }

    for(int i = 1; i <= len1; i++) {
        for(int j = 1; j <= len2; j++) {
            (*iter)++;

            if (str1[i-1] == str2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else { 
                dp[i][j] = max(dp[i-1][j], dp[i][j - 1]); 
            }
        }
    }
    
    int result = dp[len1][len2];
    for(int i = 0; i <= len1; i++) { free(dp[i]); }
    free(dp);
    
    return result;
}

void do_test(const char*testName, const char *str1, const char*str2) 
{
    long long iter = 0;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int len_lcs = calculate_lcs(str1, str2, &iter);

    clock_gettime(CLOCK_MONOTONIC, &end);

    long long secs = end.tv_sec - start.tv_sec;
    long long nanosecs = end.tv_nsec - start.tv_nsec;
    long long duration_microssecs = (secs * 1000000LL) + (nanosecs / 1000LL);
    
    printf("--- %s ---\n", testName);
    printf("str 1: %s\n", str1);
    printf("str 2: %s\n", str2);
    printf("size of LCS: %d\n", len_lcs);
    printf("dp matrix iterations: %lld\n", iter);
    printf("Execution time: %lld microsecs\n\n", duration_microssecs);
}

int main(void)
{
    do_test("test 1", "ABCBDAB", "BDCABA");
    do_test("test 2 - same str", "PROGRAMACAO", "PROGRAMACAO");
    do_test("test 3 - no lcs", "XYZ", "ABC");

    const char *str1 = "ACCGGTCGAGTGCGCGGAAGCCGGCCGAA";
    const char *str2 = "GTCGTTCGGAATGCCGTTGCTCTGTAAA";
    do_test("test 4 - dna sequence", str1, str2);

    return 0;
}