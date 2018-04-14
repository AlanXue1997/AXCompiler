#include "parser.h"

static int table[N_STATE][N_TOTAL];

production pros[N_PRODUCTION];
static std::stack<item> s;

void new_pro(int len, ...) {
	static int num = 0;

	va_list args;
	va_start(args, len);
	pros[num].L = va_arg(args, int);
	pros[num].len = len;
	if (len > 0)pros[num].R = new int[len];
	for (int i = 0; i < len; ++i) {
		pros[num].R[i] = va_arg(args, int);
	}
	va_end(args);
	num++;
}

void init_parse() {
	s.push(item{ 0, END });//bottom of stack, END denotes '#'
	//FIRST 1
	new_pro(1, V_S0, V_S);

	//TOKENS 8
	/*
	new_pro(1, V_RELOP, GT);
	new_pro(1, V_RELOP, LT);
	new_pro(1, V_RELOP, GE);
	new_pro(1, V_RELOP, LE);
	new_pro(1, V_RELOP, EQ);
	new_pro(1, V_RELOP, NE);

	new_pro(1, V_TYPE, INT);
	new_pro(1, V_TYPE, DOUBLE);

	//High - level Definitions 8
	new_pro(1, V_PROGRAM, V_EXT_DEF_LIST);

	new_pro(2, V_EXT_DEF_LIST, V_EXT_DEF, V_EXT_DEF_LIST);
	new_pro(0, V_EXT_DEF_LIST);

	new_pro(3, V_EXT_DEF, V_SPECIFIER, V_EXT_DEF_LIST, SEMI);
	new_pro(2, V_EXT_DEF, V_SPECIFIER, SEMI);
	new_pro(3, V_EXT_DEF, V_SPECIFIER, V_FUN_DEC, V_COMP_ST);//***************************

	new_pro(1, V_EXT_DEC_LIST, V_VAR_DEC);
	new_pro(3, V_EXT_DEC_LIST, V_VAR_DEC, COMMA, V_EXT_DEF_LIST);

	//Specifiers 7
	new_pro(1, V_SPECIFIER, V_TYPE);
	new_pro(1, V_SPECIFIER, V_STRUCT_SPECIFIER);

	new_pro(5, V_STRUCT_SPECIFIER, STRUCT, V_OPT_TAG, LC, V_DEF_LIST, RC);
	new_pro(2, V_STRUCT_SPECIFIER, STRUCT, V_TAG);

	new_pro(1, V_OPT_TAG, ID);
	new_pro(0, V_OPT_TAG);

	new_pro(1, V_TAG, ID);

	//Declarators 7
	new_pro(1, V_VAR_DEC, ID);
	new_pro(4, V_VAR_DEC, V_VAR_DEC, LB, INT, RB);

	new_pro(4, V_FUN_DEC, ID, LP, V_VAR_LIST, RP);
	new_pro(3, V_FUN_DEC, ID, LP, RP);//***************************

	new_pro(3, V_VAR_LIST, V_PARAM_DEC, COMMA, V_VAR_LIST);
	new_pro(1, V_VAR_LIST, V_PARAM_DEC);

	new_pro(2, V_PARAM_DEC, V_SPECIFIER, V_VAR_DEC);

	//Statements 9
	new_pro(4, V_COMP_ST, LC, V_DEF_LIST, V_STMT_LIST, RC);

	new_pro(2, V_STMT_LIST, V_STMT, V_STMT_LIST);
	new_pro(0, V_STMT_LIST);

	new_pro(2, V_STMT, V_EXP, SEMI);
	new_pro(1, V_STMT, V_COMP_ST);
	new_pro(3, V_STMT, RETURN, V_EXP, SEMI);
	new_pro(5, V_STMT, IF, LP, V_EXP, RP, V_STMT);
	new_pro(7, V_STMT, IF, LP, V_EXP, RP, V_STMT, ELSE, V_STMT);
	new_pro(5, V_STMT, WHILE, LP, V_EXP, RP, V_STMT);

	//Local Definitions 7
	new_pro(2, V_DEF_LIST, V_DEF, V_DEF_LIST);
	new_pro(0, V_DEF_LIST);

	new_pro(3, V_DEF, V_SPECIFIER, V_DEC_LIST, SEMI);

	new_pro(1, V_DEC_LIST, V_DEC);
	new_pro(3, V_DEC_LIST, V_DEC, COMMA, V_DEC_LIST);

	new_pro(1, V_DEC, V_VAR_DEC);
	new_pro(3, V_DEC, V_VAR_DEC, ASSIGNOP, V_EXP);

	//Expressions 20
	new_pro(3, V_EXP, V_EXP, ASSIGNOP, V_EXP);
	new_pro(3, V_EXP, V_EXP, AND, V_EXP);
	new_pro(3, V_EXP, V_EXP, OR, V_EXP);
	new_pro(3, V_EXP, V_EXP, V_RELOP, V_EXP);
	new_pro(3, V_EXP, V_EXP, PLUS, V_EXP);
	new_pro(3, V_EXP, V_EXP, MINUS, V_EXP);
	new_pro(3, V_EXP, V_EXP, STAR, V_EXP);
	new_pro(3, V_EXP, V_EXP, DIV, V_EXP);
	new_pro(3, V_EXP, LP, V_EXP, RP);
	new_pro(2, V_EXP, MINUS, V_EXP);
	new_pro(2, V_EXP, NOT, V_EXP);
	new_pro(4, V_EXP, ID, LP, V_ARGS, RP);
	new_pro(3, V_EXP, ID, LP, RP);
	new_pro(4, V_EXP, V_EXP, LB, V_EXP, RB);
	new_pro(3, V_EXP, V_EXP, DOT, ID);
	new_pro(1, V_EXP, ID);
	new_pro(1, V_EXP, CONST_INT);
	new_pro(1, V_EXP, CONST_DOUBLE);

	new_pro(3, V_ARGS, V_EXP, COMMA, V_ARGS);
	new_pro(1, V_ARGS, V_EXP);
	*/

	std::ifstream f("table.dat", std::ios::binary);
	for (int i = 0; i < N_STATE; ++i) {
		for (int j = 0; j < N_TOTAL; ++j) {
			f.read((char*)&table[i][j], sizeof(int));
		}
	}
	f.close();
}

int parse(int t) {
	int k = table[s.top().state][t];
	while (k >= N_STATE) {//R
		k %= N_STATE;
		production &p = pros[k];
		for (int i = p.len - 1; i >= 0; i--) {
			if (s.top().symble == p.R[i]) {
				s.pop();
			}
			else {
				std::cout << "Error when using r" << std::endl;
			}
		}
		s.push(item{ table[s.top().state][p.L] % N_STATE,p.L });
		k = table[s.top().state][t];
	}
	if (k != ACC && k != NONE) s.push(item{ k, t });
	return k;
}
