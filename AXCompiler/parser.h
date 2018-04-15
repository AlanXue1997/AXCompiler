#pragma once
#include <cstdio>
#include <iostream>
#include <stack>
#include <stdarg.h>
#include <fstream>

#include "scanner.h"
#include "tokens.h"

#define N_PRODUCTION 174
#define N_STATE 313

#define TERMINATION N_TYPE+1

#define ACC -1
#define NONE -2

//TOKENS
#define V_RELOP	TERMINATION+0
#define V_TYPE	TERMINATION+1

//High - level Definitions
#define V_PROGRAM TERMINATION+2
#define V_EXT_DEF_LIST TERMINATION+3
#define V_EXT_DEF TERMINATION+4
#define V_EXT_DEC_LIST TERMINATION+5

//Specifiers
#define V_SPECIFIER TERMINATION+6
#define V_STRUCT_SPECIFIER TERMINATION+7
#define V_OPT_TAG TERMINATION+8
#define V_TAG TERMINATION+9

//Declarators
#define V_VAR_DEC TERMINATION+10
#define V_FUN_DEC TERMINATION+11
#define V_VAR_LIST TERMINATION+12
#define V_PARAM_DEC TERMINATION+13

//Statements
#define V_COMP_ST TERMINATION+14
#define V_STMT_LIST TERMINATION+15
#define V_STMT TERMINATION+16

//Local Definitions
#define V_DEF_LIST TERMINATION+17
#define V_DEF TERMINATION+18
#define V_DEC_LIST TERMINATION+19
#define V_DEC TERMINATION+20

//Expressions
#define V_EXP TERMINATION+21
#define V_ARGS	TERMINATION+22
#define V_S0 TERMINATION+23

#define N_TOTAL V_S0+1
#define V_S V_PROGRAM
//#define V_S V_EXP
struct production {
	int L;
	int *R;
	int len;
};

struct item {
	int state, symble;
};

void init_parse();
int parse(int t);