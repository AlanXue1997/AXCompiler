#include "CodeGenerator.h"

std::ofstream fout("generatedCode.txt");

void init_code_generator() {

}

void writeHead() {
	fout << "INCLUDE	ASM32.INC" << std::endl;
	fout << std::endl;
}

void writeData(VARIABLE_LIST* global_variable_list) {

}

void generate_code(VARIABLE_LIST* global_variable_list, INT_LIST* int_list) {

}