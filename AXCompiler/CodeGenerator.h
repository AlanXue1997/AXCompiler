#pragma once

#include <fstream>
#include <stack>

#include "trans_subs.h"

#define ASM_VARIABLE_LIST std::map<std::string, ASM_VARIABLE>
#define CALL_STACK std::stack<std::string>

struct ASM_VARIABLE {
	std::string type;
	int offset;
};



void generate_code(VARIABLE_LIST* , INT_LIST* , STRING_LIST* , FUNCTION_LIST* );
void init_code_generator();