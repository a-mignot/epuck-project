#include <debug.h>
#include <stdlib.h>



//returns a string of a binary number n of length len
char* toBinary(int n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len+2);
    // !!! WARNING LEADS TO CRASH !!!

    int k = 0;
    for (unsigned i = (1 << len - 1); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
    }
    binary[k] = '\r';
    binary[k+1] = '\n';
    binary[k+2] = '\0';

    return binary;
}
