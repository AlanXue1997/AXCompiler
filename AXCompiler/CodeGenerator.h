#pragma once

#include <fstream>

#include "trans_subs.h"

void generate_code(VARIABLE_LIST* global_variable_list, INT_LIST* int_list);
void init_code_generator();