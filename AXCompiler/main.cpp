#include "main.h"

void init() {
	init_parse();
	initScanner();
	init_test();
	init_trans_subs();
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
		if (words->code == SCAN_WRONG) {
			printf("SCANN ERROR");
			break;
		}
		outputWord(words);
		printf("\n");
		k = parse(words);
	} while (words->code!=END && k!=NONE && k != ACC);
	if (words->code == END || k==ACC)printf("ACCEPTED!!!\n");
	else printf("ERROR");
	
	output_global_variables();
	output_local_variables();
	output_all_quadruple();
	output_int_list();
	//-----------------------------------------------
	system("pause");
	return 0;
}
