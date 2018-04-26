#pragma once

#include <cstdio>
#include <iostream>
#include <stack>
#include <stdarg.h>
#include <fstream>

#include "scanner.h"
#include "tokens.h"

#define LOG_PRODUCTION

#define N_PRODUCTION 212
#define N_STATE 1572
#define TERMINATION 85

#define ACC -1
#define NONE -2

struct production {
	int L;
	int *R;
	int len;
};

struct item {
	int state, symble;
};

void add_var(name2int &n2i, int2name &i2n, production *pros);
void init_parse();
int parse(int t);