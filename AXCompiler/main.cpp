#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "test.h"
#include "tokens.h"

void init() {
	init_parse();
	initScanner();
	init_test();
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
	TOKEN * words;// = tokenScan(file);
	do {
		//printf("%d ", words->code);
		words = tokenScan(file);
		outputWord(words);
		printf("\n");
	}while (words->code != END);
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
	if (words->code == END)printf("ACCEPTED!!!\n");
	else printf("ERROR");
	
	//-----------------------------------------------
	system("pause");
	return 0;
}
