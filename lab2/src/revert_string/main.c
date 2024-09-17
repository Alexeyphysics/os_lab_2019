#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "revert_string.h"


int main(int argc, char *argv[])
{
	// проверка на ко-во аргументов
	if (argc != 2)
	{
		printf("Usage: %s string_to_revert\n", argv[0]);
		return -1;
	}
// выделение памяти под строку
	char *reverted_str = malloc(sizeof(char) * (strlen(argv[1]) + 1));

	//// проверка на успешность выделения памяти
	strcpy(reverted_str, argv[1]);

	RevertString(reverted_str);

	printf("Reverted: %s\n", reverted_str);

	// освобождение выделенной памяти
	free(reverted_str);
	return 0;
}

