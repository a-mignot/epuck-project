#include <debug.h>
#include <stdlib.h>

//returns a string of a binary number n of length len
char* toBinary(int n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len+2);
    // !!! WARNING LEADS TO CRASH !!!
    // this function needs to be used very carefully and only for debug purpose

    int k = 0;
    for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
    }
    binary[k] = '\r';
    binary[k+1] = '\n';
    binary[k+2] = '\0';

    return binary;
}

char freqToPitchName(pitch note)
{
	switch(note)
	{
	case PITCH_D:
		return 'D';
	case PITCH_E:
		return 'E';
	case PITCH_F:
		return 'F';
	case PITCH_G:
		return 'G';
	case PITCH_A:
		return 'A';
	case PITCH_B:
		return 'B';
	default:
		return '0';
	}
}






