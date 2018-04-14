#include "test.h"

char* st[] = {
	"SEMI",
	"COMMA",
	"ASSIGNOP",
	"GT",
	"LT",
	"GE",
	"LE",
	"EQ",
	"NE",
	"PLUS",
	"MINUS",
	"STAR",
	"DIV",
	"AND",
	"OR",
	"[DEC]",
	"NOT",
	"INT",
	"DOUBLE",
	"LP",
	"RP",
	"LB",
	"RB",
	"LC",
	"RC",
	"STRUCT",
	"RETURN",
	"IF",
	"ELSE",
	"WHILE",
	"[ID]",
	"DO",
	"[DOUBLE]",
	"[INT]",
	"[OCT]",
	"[STRING]",
	"[CHAR]",
	"CHAR",
	"DOT",
	"END"
};

void output(TOKEN * words) {
	if (words == NULL) return;
	//output(words->p);
	//printf("%d ", words->code);
	printf("%s ", st[words->code]);
}

void outputWord(TOKEN * words) {
	std::string *s = (std::string*)words->p;
	if (s != NULL) {
		printf("(%s\t, \"%s\")", st[words->code], s->c_str());
	}
	else {
		printf("(%s\t, \"[NULL]\")", st[words->code]);
	}
}
