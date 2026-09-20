"""
Caio Madeira e Bruno

existe uma diferenca entre substring e subsequencia. Substring eh um bloco continuo
de caracteres. ex; gato -> ga, at, ato. 
Uma subsequencia os caracteres precisam aparecer na msm ordem, mas sem necessariamente estare
juntos.
"""
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int max(int a, int b)
{
    if (a > b) return a; 
    else return b;
}

int main(void)
{

    const char str1[100] = "Olá, bom dia. Poderia me ajudar?";
    const char str2[100] = "Bom dia. Quer me ajudar?";

    printf("str1: %s\n", str1);
    printf("str2: %s\n", str2);



    return 0;
}