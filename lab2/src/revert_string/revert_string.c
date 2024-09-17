#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
	// your code here

	// поочередно меняем символы строки местами
	int len = strlen(str);
	for (int i = 0; i < len / 2; i++) {
		char temp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = temp;
	}

	
}

