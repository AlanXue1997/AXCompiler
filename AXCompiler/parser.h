#pragma once

#include <cstdio>
#include <iostream>
#include <stack>
#include <stdarg.h>
#include <fstream>

#include "scanner.h"
#include "tokens.h"
#include "trans_subs.h"

#define LOG_PRODUCTION
//#define DO_TRANS

#define N_PRODUCTION 212
#define N_STATE 1572
#define TERMINATION 85

#define ACC -1
#define NONE -2

//L->R L has only one variable, R may have 0,1,2 or more variables
struct production {
	int L;
	int *R;
	int len;//length of R
	int sub_index;//index of productions with the same L
};

//item in the syntax stack
struct item {
	int state, symble;
};

void add_var(name2int &n2i, int2name &i2n, production *pros);
void init_parse();
int parse(TOKEN* t);