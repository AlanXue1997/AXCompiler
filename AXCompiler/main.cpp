#include "main.h"

void init() {
	init_parse();
	initScanner();
	init_test();
	init_trans_subs();
	init_code_generator();
}

int main(int argc, char* argv[]) {
	FILE *file;
	file = fopen("input.txt", "r");

	init();
	
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
	output_func_list();

	generate_code(get_global_variables(), get_int_list(), get_function_list());
	system("pause");
	return 0;
}
