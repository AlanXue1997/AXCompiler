#include "test.h"

int2name w;

void init_test() {
	w = getInt2name();
}

void outputWord(TOKEN * words) {
	std::string *s = (std::string*)words->p;
	if (s != NULL) {
		printf("(%s\t, \"%s\")", w[words->code].c_str(), s->c_str());
	}
	else {
		printf("(%s\t, \"[NULL]\")", w[words->code].c_str());
	}
}
