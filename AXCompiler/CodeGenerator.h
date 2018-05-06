#pragma once

#include <fstream>

#include "trans_subs.h"

#define ASM_VARIABLE_LIST std::map<std::string, ASM_VARIABLE>

struct ASM_VARIABLE {
	std::string type;
	int offset;
};



void generate_code(VARIABLE_LIST* global_variable_list, INT_LIST* int_list, FUNCTION_LIST* function_list);
void init_code_generator();