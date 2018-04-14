#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "test.h"
#include "tokens.h"

void init() {
	init_parse();
}

int main(int argc, char* argv[]) {
	FILE *file;
	file = fopen("input.txt", "r");

	init();

	//-----------------------------------------------
	//Test scanner
	//-----------------------------------------------
	/*
	int k;
	TOKEN * words = wordScan(file);
	while (words != NULL) {
		outputWord(words);
		//printf("%d ", words->code);
		words = wordScan(file);
	}
	*/
	//-----------------------------------------------

	//-----------------------------------------------
	//Test parse
	//-----------------------------------------------
	TOKEN * words;
	int k;
	do {
		words = tokenScan(file);
		outputWord(words);
		k = parse(words->code);
		printf("\n");
	} while (words->code!=END && k!=NONE);
	//-----------------------------------------------
	if (words->code == END)printf("ACCEPTED!!!\n");
	else printf("ERROR");
	system("pause");
	return 0;
}
