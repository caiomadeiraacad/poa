#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// int isPal(char str[])
// {
//     int len = strlen(str);
//     int match = 0;
//     for(int i = 0; i < len; i++) {
//         for(int j = len; j >= 0; j--) {
//             if (str[i] == str[j]) {
//                 match = match + 1;
//             }
//             else {
//                 match = match - 1;
//             }
//         }
//     }

//     if (match == len) { return 1; } 
//     else { return 0;}
// }

int isPal(char str[])
{
    int len = strlen(str);
    int i = 0;
    int j = len - 1;
    while(i < j) {
        if (str[i] != str[j]) { return 0; }
        j--;
        i++;
    }
    return 1;
}

int main(void) {

    char str1[] = "carro";
    char str2[] = "osso";
    char str3[] = "radar";

    printf("%s -> is palidrome? %d\n", str1, isPal(str1));
    printf("%s -> is palidrome? %d\n", str2, isPal(str2));
    printf("%s -> is palidrome? %d\n", str3, isPal(str3));

    return 0;
}